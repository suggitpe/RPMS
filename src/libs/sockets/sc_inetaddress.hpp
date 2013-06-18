#ifndef __sc_inetaddress_hpp
#define __sc_inetaddress_hpp

#include <netinet/in.h>
#include <base/sa_baseobject.hpp>
#include <arpa/inet.h>

namespace rpms
{

    /** Wrapper for the netinet address structs */
    class SC_InetAddress : public SA_BaseObject
    {
        public:
            SC_InetAddress( unsigned short int aPortNo );
            SC_InetAddress( const char * aHostName, unsigned short int aPortNo );
            SC_InetAddress( const SC_InetAddress& aRhs );
            SC_InetAddress( const struct sockaddr_in aSockAddr );
    
            struct sockaddr_in * getRawData();
            int getSize();
            unsigned short int getPortNumber();
            unsigned long getAddress();
            std::string getFullAddress();
            size_t getAddressSize() const { return sizeof(struct sockaddr);}
            static std::string getLocalHostName();
            static std::string getLocalDomainName();
    
        private:
            struct sockaddr_in mData;
            
    };

} // namespace

#endif
