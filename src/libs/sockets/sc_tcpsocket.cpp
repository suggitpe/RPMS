//#############################
#include "sc_tcpsocket.hpp"
#include "si_socketlog.hpp"
#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <utilities/st_conversion.hpp>
#include "sx_socket.hpp"
#include "sx_socketconnectrefused.hpp"
#include "sx_socketnoendpoint.hpp"
#include <si_macros.hpp>

using rpms::SC_TCPSocket;

///////////////////////////////////////////
SC_TCPSocket::SC_TCPSocket()
    : SA_Socket(SOCK_STREAM)
{
    TRY(SC_TCPSocket::SC_TCPSocket());
    CATCH;
}

///////////////////////////////////////////
SC_TCPSocket::SC_TCPSocket( const long aTimeout )
    : SA_Socket(SOCK_STREAM, aTimeout)
{
    TRY(SC_TCPSocket::SC_TCPSocket( const long aTimeout ));
    CATCH;
}

///////////////////////////////////////////
void SC_TCPSocket::connect( const SC_InetAddress & aAddress )
{
    using rpms::convert;
    TRY(SC_TCPSocket::connect( const SC_InetAddress & aAddress ));
        if( isConnected() )
        {
            RPMS_ERROR( SKT_TCP_LOG, "Socket is already connected to an existing host" );
            throw SX_Socket("Socket is already connected to an existing host");
        }

        setOtherAddress( aAddress );
    
        RPMS_DEBUG( SKT_TCP_LOG, "Connecting to remote host [" + this->getOtherAddress()->getFullAddress() + "]");
        if( -1 == ( ::connect( getSockFD(), 
                           reinterpret_cast<struct sockaddr*>(getOtherAddress()->getRawData()), 
                           getOtherAddress()->getAddressSize() ) ) )
        {
            int numErr = errno;
            char errBuff[SS_Ini::instance()->getInt("sockets", "socket.error.buff")];
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failed to connect to remote host.  Error [" + convert<std::string>(numErr) + "]: " + errTxt;
            RPMS_ERROR( SKT_TCP_LOG, exc );
            if( numErr == ECONNREFUSED )
            {
                throw SX_SocketConnectRefused(exc);
            }
            else if( numErr == EHOSTUNREACH  )
            {
                throw SX_SocketNoEndPoint(exc);
            }
            throw SX_Socket(exc);
        }
        setConnected();
    CATCH;
}

///////////////////////////////////////////
std::string SC_TCPSocket::receive()
{
    using rpms::convert;
    TRY(SC_TCPSocket::receive());
        char buff[SS_Ini::instance()->getInt("sockets", "socket.tcp.maxstreamsize")];
    
        struct sockaddr_in their_addr;
        int addr_len = sizeof(struct sockaddr);
        int new_fd;
        if( -1 == (new_fd = ::accept( this->getSockFD(), 
                             reinterpret_cast<struct sockaddr*>(&their_addr),
                             reinterpret_cast<socklen_t*>(&addr_len))))
        {
            int numErr = errno;
            char errBuff[SS_Ini::instance()->getInt("sockets", "socket.error.buff")];
            // thread safe
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failed to accept from socket.  Error [" 
                    + convert<std::string>(numErr) + "]: " + errTxt;
            RPMS_ERROR( SKT_TCP_LOG, exc );
            throw SX_Socket(exc);
        }
        RPMS_TRACE(SKT_TCP_LOG, "Accepted a connection, now receiving data ..." );
        
        // now we have connection we need to receive the data
        ssize_t numBytes = 0;
        if( -1 == (numBytes = ::recv(new_fd, buff, sizeof(buff)-1, 0 )) )
        {
            int numErr = errno;
            char errBuff[SS_Ini::instance()->getInt("sockets", "socket.error.buff")];
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failed to receive from socket.  Error [" 
                    + convert<std::string>(numErr) + "]: " + errTxt;
            RPMS_ERROR( SKT_TCP_LOG, exc );
            throw SX_Socket(exc);
        }
        else if( numBytes == 0 )
        {
            RPMS_INFO(SKT_TCP_LOG, "Remote socket closed socket before sending any data");
        }
        buff[numBytes] = '\0';

        return buff;
    CATCH;
}

///////////////////////////////////////////
void SC_TCPSocket::send( const std::string &aMessage )
{
    using rpms::convert;
    TRY(SC_TCPSocket::send( const std::string &aMessage ));
        unsigned int bytes = 0;
        int size = aMessage.size();
        if( -1 == ( bytes = ::send( this->getSockFD(), aMessage.c_str(), size, 0 )))
        {
            int numErr = errno;
            char errBuff[SS_Ini::instance()->getInt("sockets", "socket.error.buff")];
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failed to send message.  Error [" + convert<std::string>(numErr) + "]: " + errTxt;
            RPMS_ERROR( SKT_TCP_LOG, exc );
            throw SX_Socket(exc);
        }

        if( bytes != size )
        {
            std::string exc = "Tried to send [" + convert<std::string>(size) 
                + "] bytes of data but only sent [" + convert<std::string>(bytes) + "] bytes";
            RPMS_ERROR( SKT_TCP_LOG, exc);
            throw SX_Socket(exc);
        }
        RPMS_TRACE( SKT_TCP_LOG, "Sent message [" + aMessage + "] ok");

    CATCH;
}

///////////////////////////////////////////


