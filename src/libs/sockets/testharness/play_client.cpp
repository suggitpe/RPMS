//#############################

#include <testharness/si_testharness.hpp>
#include <utilities/st_conversion.hpp>

#include <netdb.h>
#include <arpa/inet.h>

TEST_BASE;

void simpleTCPSocketTest();
void simpleUDPSocketTest();
//////////////////////////////////////////////////
#define MY_TCP_PORT 3490
#define MY_UDP_PORT 4950
#define MAX_DATASIZE 100
//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    RPMS_TRACE(LOGNAME, "------------------------ START ---------------------");
    
    //simpleTCPSocketTest();
    simpleUDPSocketTest();

    RPMS_TRACE(LOGNAME, "------------------------- END ----------------------");
}

//////////////////////////////////////////////////
// ********************** SIMPLE SOCKET TEST *****
void simpleUDPSocketTest()
{
    int sock_fd;
    struct sockaddr_in their_addr;
    struct hostent *he;
    int numbytes;

    RPMS_TRACE(LOGNAME, "Setting up the socket");
    if( (sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
    {
        perror("socket");
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "Getting host info");
    if( ( he=gethostbyname("pgdsX02") ) == NULL )
    {
        perror("gethostbyname");
        exit(-1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(MY_UDP_PORT);
    their_addr.sin_addr = *( reinterpret_cast<struct in_addr*>(he->h_addr) );
    memset(&(their_addr.sin_zero), '\0', 8);

    RPMS_TRACE(LOGNAME, "Sending ...");
    if( (numbytes = sendto( sock_fd, "hello", strlen("hello"), 0, reinterpret_cast<struct sockaddr*>(&their_addr), sizeof(struct sockaddr) ) ) == -1 )
    {
        perror("sento");
        exit(-1);
    }

    RPMS_INFO(LOGNAME, "Sent [" + rpms::convert<std::string>(numbytes) 
            + "] bytes to [" + rpms::convert<std::string>(inet_ntoa(their_addr.sin_addr)) + "]");

    close(sock_fd);

}

//////////////////////////////////////////////////
void simpleTCPSocketTest()
{
    int sock_fd;
    int numbytes;
    char buf[MAX_DATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr;

    RPMS_TRACE(LOGNAME, "Setting up the socket");
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        RPMS_ERROR(LOGNAME, "failed to set up socket");
        perror("socket");
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "Getting host info");
    if( (he=gethostbyname("pgdsX02")) == NULL )
    {
        RPMS_ERROR(LOGNAME, "failed to get the host info");
        perror("gethostbyname");
        exit(-1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons( MY_TCP_PORT );
    their_addr.sin_addr = *( reinterpret_cast<struct in_addr*>(he->h_addr) );
    memset(&(their_addr.sin_zero), '\0', 8);

    RPMS_TRACE(LOGNAME, "connecting");
    //if( connect( sock_fd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr) ) == -1 )
    if( connect( sock_fd, reinterpret_cast<struct sockaddr*>(&their_addr), sizeof(struct sockaddr) ) == -1 )
    {
        RPMS_ERROR(LOGNAME, "failed to connect");
        perror("connect");
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "receiving");
    if( ( numbytes = recv(sock_fd, buf, MAX_DATASIZE-1, 0) ) == -1 )
    {
        perror("recv");
        exit(-1);
    }
    
    buf[numbytes] = '\0';

    RPMS_INFO(LOGNAME, "received back [" + std::string(buf) + "]" );

    close( sock_fd);
}

