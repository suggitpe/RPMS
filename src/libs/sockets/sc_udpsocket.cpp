//#############################
#include "sc_udpsocket.hpp"
#include "si_socketlog.hpp"
#include "sx_socket.hpp"
#include "sx_sockettimeout.hpp"
#include "sx_socketinterrupt.hpp"

#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>
#include <utilities/ss_ini.hpp>
#include <si_macros.hpp>

using rpms::SC_UDPSocket;

///////////////////////////////////////////
SC_UDPSocket::SC_UDPSocket()
    : SA_Socket(SOCK_DGRAM)
{
    TRY(SC_UDPSocket::SC_UDPSocket());
    CATCH;
}

///////////////////////////////////////////
SC_UDPSocket::SC_UDPSocket( const long aTimeout )
    : SA_Socket(SOCK_DGRAM, aTimeout)
{
    TRY(SC_UDPSocket::SC_UDPSocket( const long aTimeout ));
    CATCH;
}

///////////////////////////////////////////
void SC_UDPSocket::connect( const SC_InetAddress & aAddress )
{
    TRY(SC_UDPSocket::connect( const SC_InetAddress & aAddress ));
        // check to make sure that the socket is already connected elsewhere
        if( this->isConnected() )
        {
            RPMS_ERROR( SKT_UDP_LOG, "Socket is already connected to an existing host" );
            throw SX_Socket("Socket is already connected to an existing host");
        }

        // for UDP we just set up the connection details for the dest server
        this->setOtherAddress( aAddress );
        this->setConnected();

        RPMS_DEBUG( SKT_UDP_LOG, "Created connection to [" + getOtherAddress()->getFullAddress() + "]" );
    CATCH;
}

///////////////////////////////////////////
void SC_UDPSocket::send( const std::string &aMessage )
{
    using rpms::convert;
    TRY(SC_UDPSocket::send( const std::string &aMessage ));
        unsigned int bytes = 0;
        if( -1 == (bytes = ::sendto( this->getSockFD(), 
                                 aMessage.c_str(), 
                                 aMessage.size(), 
                                 0, 
                                 reinterpret_cast<struct sockaddr*>(getOtherAddress()->getRawData()), 
                                 getOtherAddress()->getAddressSize() ) ) )
        {
            int numErr = errno;
            char errBuff[SS_Ini::instance()->getInt("sockets", "socket.error.buff")];
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failed to send message.  Error [" + convert<std::string>(numErr) + "]: " + errTxt;
            RPMS_ERROR( SKT_UDP_LOG, exc );
            throw SX_Socket(exc);
        }
    CATCH;
}

///////////////////////////////////////////
std::string SC_UDPSocket::receive()
{
    using rpms::convert;
    TRY(SC_UDPSocket::receive());
        char buff[SS_Ini::instance()->getInt("sockets", "socket.udp.maxpacketsize")];

        struct sockaddr_in their_addr;
        ssize_t numBytes = 0;
        int addr_len = sizeof(struct sockaddr);
        if( -1 == (numBytes = recvfrom( this->getSockFD(), 
                                    buff, 
                                    sizeof(buff)-1, 
                                    0,
                                    reinterpret_cast<struct sockaddr*>(&their_addr), 
                                    reinterpret_cast<socklen_t*>(&addr_len) ) ) )
        {
            int numErr = errno;
            char errBuff[SS_Ini::instance()->getInt("sockets", "socket.error.buff")];
            // thread safe
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failed to receive from socket.  Error [" 
                    + convert<std::string>(numErr) + "]: " + errTxt;
            if( numErr == EAGAIN )// || numErr == EWOULDBLOCK  )
            {
                throw SX_SocketTimeout(exc);
            }
            else if( numErr == EINTR )
            {
                throw SX_SocketInterrupt(exc);
            }
            RPMS_ERROR( SKT_UDP_LOG, exc );
            throw SX_Socket(exc);
        }

        buff[numBytes] = '\0';

        setOtherAddress(SC_InetAddress(their_addr));

        return buff;
    CATCH;
}

///////////////////////////////////////////
