#ifndef __sc_tcpsocket_hpp
#define __sc_tcpsocket_hpp

#include <sockets/sa_socket.hpp>

namespace rpms
{

    /** TCP socket implementation */
    class SC_TCPSocket : public SA_Socket
    {
        public:
            SC_TCPSocket();
            SC_TCPSocket( const long aTimeout );
            virtual void connect( const SC_InetAddress & aAddress );
            virtual std::string receive();
            virtual void send( const std::string &aMessage  );
    
        private:
    };

} // namespace

#endif
