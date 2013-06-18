//#############################

#include <testharness/si_testharness.hpp>

#include <sockets/sc_udpsocket.hpp>
#include <sockets/sc_tcpsocket.hpp>
#include <base/st_spointer.hpp>
#include <sockets/sc_inetaddress.hpp>
#include <si_macros.hpp>

TEST_BASE;
void testUDPClient();
void testTCPClient();

using rpms::SX_Unknown;
using rpms::SA_Socket;
using rpms::SC_UDPSocket;
using rpms::SC_TCPSocket;

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    TRY(execute());
    RPMS_TRACE( LOGNAME, "This is an application to test objects");
    RPMS_INFO( LOGNAME, "START ***************************");
    
    //RPMS_INFO(LOGNAME, "Hostname is    " + SC_InetAddress::getLocalHostName());
    //RPMS_INFO(LOGNAME, "Domain name is " + SC_InetAddress::getLocalDomainName());

    //testUDPClient();

    RPMS_INFO( LOGNAME, "*********************************");

    testTCPClient();
    
    RPMS_INFO( LOGNAME, "END   ***************************");
    CATCH;
}

//////////////////////////////////////////////////
void testUDPClient()
{
    TRY(testUDPClient());
    // DIRECTOR
    // set up the socket
    RPMS_TRACE(LOGNAME, "Creating a UDP Socket ... [DIRECTOR]" );
    ST_SPointer<SA_Socket> s =  new SC_UDPSocket(SS_Ini::instance()->getInt("sockets", "socket.recv.timeout"));
    // set up dest address at the socket
    RPMS_TRACE(LOGNAME, "Connecting to remote server..." );
    s->connect( "pgdsX00", SS_Ini::instance()->getInt("daemon", "heartbeat.udp.port") );
    
    // send data to the end point
    RPMS_TRACE(LOGNAME, "Sending data to server ..." );
    s->send( "Hello this is a test" );
    
    // now wait for a reply
    RPMS_TRACE(LOGNAME, "Receiving data from recipient ... ");
    std::string msg = s->receive();
    RPMS_TRACE(LOGNAME, "Received message from [" + s->getOtherAddress()->getFullAddress() + "] \n\t[" + msg + "]" );
    CATCH;
}


//////////////////////////////////////////////////
void testTCPClient()
{
    TRY(testTCPClient());
    // 
    RPMS_TRACE(LOGNAME, "Creating a TCP Socket ... [ELECTION_INITIATOR]" );
    ST_SPointer<SA_Socket> h = new SC_TCPSocket();
    h->connect( "pgdsX00", SS_Ini::instance()->getInt("election", "election.tcp.port") );
    h->send("ping");
    CATCH;
}


