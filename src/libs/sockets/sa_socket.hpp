#ifndef __sa_socket_hpp
#define __sa_socket_hpp

#include <base/sa_baseobject.hpp>
#include <base/st_spointer.hpp>
#include <sockets/sc_inetaddress.hpp>
#include <netdb.h>

namespace rpms
{

    /** Base class for all socket types */
    class SA_Socket : public SA_BaseObject
    {
        public:
            /// virtual dtor
            virtual ~SA_Socket();
            /// bind to a local port
            void bind( unsigned short int aPortNumber );
            /// connect to the remote host
            void connect( const char * aHost, const int aPort  );
            /// connect to the remote host
            virtual void connect( const SC_InetAddress & aAddress ) = 0;
            /// send data to the dest connection
            virtual void send( const std::string &aMessage  ) = 0;
            /// standard receive .. receive data
            virtual std::string receive() = 0;
    
            /// simple getter
            bool isConnected() const { return mConnected; }
            /// returns a copy of the dest address
            ST_SPointer<SC_InetAddress> getOtherAddress() const { return new SC_InetAddress(*mOtherAddress); }
         
        protected:
            /// true base ctor .. timeout is in seconds
            SA_Socket( const int aType, const long aTimeout = 0 );
            void setOtherAddress( const SC_InetAddress& aOther );
            void setConnected() { mConnected = true; }
            int getSockFD() const { return mSockFD; }
            int getSocketType() {return mSocketType;}
    
        private:
            /// socket file descriptor
            int mSockFD;
            /// socket type (DGRAM/STREAM)
            int mSocketType;
            /// INET address ref for localk process
            ST_SPointer<SC_InetAddress> mLocalAddress;
            /// INET address ref for destination
            ST_SPointer<SC_InetAddress> mOtherAddress;
            /// connection boolean .. a socket can only connect once
            bool mConnected;
    };

} // namespace

#endif

