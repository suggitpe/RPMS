#ifndef __sc_udpsocket_hpp
#define __sc_udpsocket_hpp

#include <sockets/sa_socket.hpp>

namespace rpms
{

    /** Datagram socket implementation */
    class SC_UDPSocket : public SA_Socket
    {
        public:
            SC_UDPSocket();
            SC_UDPSocket( const long aTimeout );
            virtual void connect( const SC_InetAddress & aAddress );
            virtual void send( const std::string &aMessage  );
            virtual std::string receive();
    
        private:
    };

} // namespace

#endif
