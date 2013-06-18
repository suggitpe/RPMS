//#############################
#include "sc_inetaddress.hpp"
#include "sx_socket.hpp"

#include <string.h> // for memset
#include <utilities/st_conversion.hpp>
#include <netdb.h>
#include <si_macros.hpp>

using rpms::SC_InetAddress;

///////////////////////////////////////////
SC_InetAddress::SC_InetAddress( unsigned short int aPortNo )
{
    TRY(SC_InetAddress::SC_InetAddress( unsigned short int aPortNo ));
        mData.sin_family = AF_INET;  // host byte odr
        mData.sin_port = htons( aPortNo );
        mData.sin_addr.s_addr = 0x00000000; // blank IP .. will assign one when used
        memset( &mData.sin_zero, '\0', 8 );
    CATCH;
}

///////////////////////////////////////////
SC_InetAddress::SC_InetAddress( const struct sockaddr_in aSockAddr )
{
    TRY(SC_InetAddress::SC_InetAddress( const struct sockaddr_in aSockAddr ));
        mData.sin_family    = aSockAddr.sin_family;
        mData.sin_port      = aSockAddr.sin_port;
        mData.sin_addr      = aSockAddr.sin_addr;
        memset( &mData.sin_zero, '\0', 8 );
    CATCH;
}

///////////////////////////////////////////
SC_InetAddress::SC_InetAddress( const char * aHostName, unsigned short int aPortNo )
{
    TRY(SC_InetAddress::SC_InetAddress( const char * aHostName, unsigned short int aPortNo ));
        struct hostent *he;
        if( ( he=::gethostbyname( aHostName ) ) == NULL )
        {
            throw SX_Socket("Failed to get IPAddress for host [" + std::string(aHostName) + "]");
        }
    
        mData.sin_family    = AF_INET;  // host byte odr
        mData.sin_port      = htons( aPortNo );
        mData.sin_addr      = *( reinterpret_cast<struct in_addr*>(he->h_addr) );
        memset( &mData.sin_zero, '\0', 8 );
    CATCH;
}

///////////////////////////////////////////
SC_InetAddress::SC_InetAddress( const SC_InetAddress& aRhs )
{
    TRY(SC_InetAddress::SC_InetAddress( const SC_InetAddress& aRhs ));
        mData.sin_family    = aRhs.mData.sin_family;
        mData.sin_port      = aRhs.mData.sin_port;
        mData.sin_addr      = aRhs.mData.sin_addr;
        memset( &mData.sin_zero, '\0', 8 );
    CATCH;
}

///////////////////////////////////////////
struct sockaddr_in * SC_InetAddress::getRawData()
{
    return &mData;
}

///////////////////////////////////////////
int SC_InetAddress::getSize()
{
    return sizeof(struct sockaddr_in);
}

///////////////////////////////////////////
unsigned short int SC_InetAddress::getPortNumber()
{
    return mData.sin_port;
}

///////////////////////////////////////////
unsigned long SC_InetAddress::getAddress()
{
    return mData.sin_addr.s_addr;
}

///////////////////////////////////////////
std::string SC_InetAddress::getFullAddress()
{
    using rpms::convert;
    TRY(SC_InetAddress::getFullAddress());
        return convert<std::string>(inet_ntoa(mData.sin_addr)) + ":" 
            + convert<std::string>(htons(mData.sin_port));
    CATCH;
}

///////////////////////////////////////////
std::string SC_InetAddress::getLocalHostName()
{
    TRY(SC_InetAddress::getLocalHostName());
        int bufSize = 255;
        char buf0[bufSize];
        if( -1 == ::gethostname(buf0, bufSize) )
        {
            throw SX_Socket("Failed to get local hostname");
        }
        return buf0;
    CATCH;
}

///////////////////////////////////////////
std::string SC_InetAddress::getLocalDomainName()
{
    TRY(SC_InetAddress::getLocalDomainName());
        int bufSize = 255;
        char buf0[bufSize];
        if( -1 == ::getdomainname(buf0, bufSize) )
        {
            throw SX_Socket("Failed to get local domain name");
        }
        return buf0;
        
    CATCH;
}

///////////////////////////////////////////
