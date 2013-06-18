//#############################
#include "sa_socket.hpp"
#include "sx_socket.hpp"
#include "si_socketlog.hpp"

#include <utilities/st_conversion.hpp>
#include <sys/time.h>
#include <si_macros.hpp>
#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>

using rpms::SA_Socket;

///////////////////////////////////////////
SA_Socket::SA_Socket( const int aType, const long aTimeout )
    : mSocketType(aType), mConnected(false)
{
    using rpms::convert;
    TRY(SA_Socket::SA_Socket( const int aType, const long aTimeout ));
        int yes = 1;
        switch( aType )
        {
            case SOCK_DGRAM:
                RPMS_TRACE( SKT_BAS_LOG, "Creating [SOCK_DGRAM] socket" );
                break;
            case SOCK_STREAM:
                RPMS_TRACE( SKT_BAS_LOG, "Creating [SOCK_STREAM] socket" );
                break;
            default:
                RPMS_ERROR( SKT_BAS_LOG, "Attempted to create invalid socket type [" 
                        + convert<std::string>(aType) +  "]");
                throw SX_Socket("Attempted to create invalid socket type [" 
                        + convert<std::string>(aType) +  "]");
        }
    
        // create a socket
        mSockFD = ::socket( AF_INET, aType, 0 );
        if( -1 == mSockFD )
        {
            throw SX_Socket("Failed to create socket");
        }

        RPMS_TRACE( SKT_BAS_LOG, "Setting socket options ... ");
        if( aTimeout > 0 ) // 0 is default anyway
        {
            struct timeval t;
            t.tv_sec = aTimeout;
            t.tv_usec = 0;
        
            RPMS_TRACE( SKT_BAS_LOG, "Setting socket timeout to [" + convert<std::string>(aTimeout) + "]" );
            if( setsockopt( mSockFD, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(timeval) ) == -1 )
            {
                throw SX_Socket("Failed to set timeout on socket");
            }
        }   

        if( setsockopt( mSockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) ) == -1 )
        {
            throw SX_Socket("Failed to set address re-use option on socket");
        }
    CATCH;
    
}

///////////////////////////////////////////
SA_Socket::~SA_Socket()
{
    TRY(SA_Socket::~SA_Socket());
        //RPMS_TRACE( SKT_BAS_LOG, "Closing socket" );
        close(mSockFD);
    CATCH;
}


///////////////////////////////////////////
void SA_Socket::bind( unsigned short int aPortNumber )
{
    using rpms::convert;
    TRY(SA_Socket::bind( unsigned short int aPortNumber ));
        mLocalAddress = new SC_InetAddress(aPortNumber);
        
        if( -1 == ::bind( mSockFD, reinterpret_cast<struct sockaddr *>(mLocalAddress->getRawData()), mLocalAddress->getSize() ) )
        {
            throw SX_Socket("Failed to bind to port [" + convert<std::string>(aPortNumber) + "]");
        }
        std::string p = mLocalAddress->getFullAddress();
        RPMS_TRACE( SKT_BAS_LOG, "Bound to local port [" + p + "]" );

        // this could be done as a polimorphic method but hey what the hell .. more expensive to manage vtables than as below
        if( mSocketType == SOCK_STREAM )
        {
            RPMS_TRACE( SKT_BAS_LOG, "Calling listen on port [" + p + "]" );
            if( -1 == ::listen(mSockFD, SS_Ini::instance()->getInt("sockets", "socket.tcp.listenbacklog")) )
            {
                throw SX_Socket("Failed to listen on port [" + convert<std::string>(aPortNumber) + "]");
            }
        }
    CATCH;
}

///////////////////////////////////////////
void SA_Socket::connect( const char * aHost, const int aPort )
{
    TRY(SA_Socket::connect( const char * aHost, const int aPort ));
        this->connect( SC_InetAddress(aHost, aPort) );
    CATCH;
}

///////////////////////////////////////////
void SA_Socket::setOtherAddress( const SC_InetAddress& aOther )
{
    TRY(SA_Socket::setOtherAddress( const SC_InetAddress& aOther ));
        mOtherAddress = new SC_InetAddress(aOther);
    CATCH;
}

///////////////////////////////////////////
