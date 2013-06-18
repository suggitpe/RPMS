//#############################

#include <testharness/si_testharness.hpp>
#include <utilities/st_conversion.hpp>

#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

TEST_BASE;

void simpleTCPSocketTest();
void simpleUDPSocketTest();
void sigchld_handler(int s);
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
#define MY_TCP_PORT 3490
#define MY_UDP_PORT 4950
#define BACKLOG 10
#define MAX_BUFLEN 100

void simpleUDPSocketTest()
{
    int sock_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int addr_len;
    int numbytes;
    char buf[MAX_BUFLEN];

    RPMS_TRACE(LOGNAME, "Setting up socket");
    if( (sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
    {
        perror("socket");
        exit(-1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MY_UDP_PORT);
    my_addr.sin_addr.s_addr = 0x00000000;
    //my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // implements c-style cast
    memset(&(my_addr.sin_zero), '\0', 8);

    RPMS_TRACE(LOGNAME, "binding ...");
    if( bind(sock_fd, reinterpret_cast<struct sockaddr *>(&my_addr), sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(-1);
    }

    addr_len = sizeof(struct sockaddr);
    RPMS_TRACE(LOGNAME, "receiving ...");
    if( (numbytes = recvfrom(sock_fd, buf, MAX_BUFLEN-1, 0, reinterpret_cast<struct sockaddr *>(&their_addr), reinterpret_cast<socklen_t*>(&addr_len))) == -1)
    {
        perror("recvfrom");
        exit(-1);
    }

    RPMS_INFO(LOGNAME, "got packet from [" + rpms::convert<std::string>(inet_ntoa(their_addr.sin_addr)) + "]");
    RPMS_INFO(LOGNAME, "packet is [" + rpms::convert<std::string>(numbytes) + "] long");
    buf[numbytes] = '\0';
    RPMS_INFO(LOGNAME, "packet contains [" + std::string(buf) + "]");
    close(sock_fd);
}

//////////////////////////////////////////////////
void simpleTCPSocketTest()
{
    int sock_fd;
    int new_fd;
    struct sockaddr_in  my_addr;
    struct sockaddr_in  their_addr;
    int                 sin_size;
    struct sigaction    sa;
    int yes = 1;

    RPMS_TRACE(LOGNAME, "Setting up socket");
    if( (sock_fd = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
    {
        perror( "socket" );
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "Setting socket options");
    if( ( setsockopt( sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) ) ) == -1 )
    {
        perror( "setsockopt" );
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "Setting up IP address");
    my_addr.sin_family = AF_INET; // host byte odr
    my_addr.sin_port = htons(MY_TCP_PORT); // short, ntwk byte odr
    //my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // blank IP .. uses c-style cast
    my_addr.sin_addr.s_addr = 0x00000000; // blank IP
    memset( &my_addr.sin_zero, '\0', 8 );

    RPMS_TRACE(LOGNAME, "binding");
    if( bind(sock_fd, reinterpret_cast<struct sockaddr *>(&my_addr), sizeof(struct sockaddr)) == -1)
    {
        perror( "bind" );
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "listening");
    if( listen( sock_fd, BACKLOG) == -1 )
    {
        perror( "listen" );
        exit(-1);
    }

    RPMS_TRACE(LOGNAME, "setting up err handler");
    sa.sa_handler = sigchld_handler; // reap dead procs
    sigemptyset( &sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if( sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror( "listen" );
        exit(-1);
    }

    RPMS_INFO(LOGNAME, "server: my connection is [" 
                + rpms::convert<std::string>(inet_ntoa(my_addr.sin_addr)) + "]");
    
    // main accept loop
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        RPMS_DEBUG(LOGNAME, "accepting message ... ");
        if( (new_fd = accept( sock_fd, reinterpret_cast<struct sockaddr *>(&their_addr), reinterpret_cast<socklen_t*>(&sin_size) )) == -1 )
        {
            perror( "accept" );
            exit(-1);
        }
        RPMS_INFO(LOGNAME, "server: got connection from [" 
                + rpms::convert<std::string>(inet_ntoa(their_addr.sin_addr)) + "]");

        RPMS_DEBUG(LOGNAME, "sending return message ... ");
        if( !fork() ) // this is the child
        {
            close(sock_fd); // child no need listener
            if( send( new_fd, "Hello, world!", 13, 0 ) == -1 )
            {
                perror("send");
            }
            close(new_fd);
            exit(0);
        }
        
        close( new_fd );
    }
    
}

//////////////////////////////////////////////////
void sigchld_handler(int s)
{
    while( wait(0) > 0);
}
