//#############################

#include <testharness/si_testharness.hpp>

#include <sockets/sc_udpsocket.hpp>
#include <sockets/sc_tcpsocket.hpp>
#include <base/st_spointer.hpp>

#include <si_macros.hpp>

TEST_BASE;
void testUDPServer();
void testTCPServer();

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

    //testUDPServer();

    RPMS_INFO( LOGNAME, "*********************************");

    testTCPServer();
    
    RPMS_INFO( LOGNAME, "END   ***************************");
    CATCH;
}

//////////////////////////////////////////////////
void testUDPServer()
{
    TRY(testUDPServer());
    // DAEMON
    // set up socket
    RPMS_TRACE(LOGNAME, "Creating a UDP Socket ... [DAEMON]" );
    ST_SPointer<SA_Socket> s = new SC_UDPSocket(SS_Ini::instance()->getInt("sockets", "socket.recv.timeout"));
    
    // bind to local port
    RPMS_TRACE(LOGNAME, "Binding to local port ..." );
    s->bind( SS_Ini::instance()->getInt("daemon", "heartbeat.udp.port") );
    
    // keep it running
    while(1)
    {
        // wait for a message
        RPMS_TRACE(LOGNAME, "Receiving from client ..." );
        std::string msg = s->receive();

        // only set up the end port once we receive something (we do not know who is sending until we receive)
        RPMS_TRACE(LOGNAME, "Received message  from [" + s->getOtherAddress()->getFullAddress() + "] \n\t[" + msg + "]" );
        if( ! s->isConnected())
        {
            s->connect(*s->getOtherAddress());
        }
        
        // now we reply to the sender
        RPMS_TRACE(LOGNAME, "Replying with new message");
        s->send("Got your message {" + msg + "}");
    }
    CATCH;
}


//////////////////////////////////////////////////
void testTCPServer()
{
    TRY(testTCPServer());
    // create a local socket
    RPMS_DEBUG(LOGNAME, "Creating a TCP Socket ... [ELECTION_RELAY]" );
    ST_SPointer<SA_Socket> h = new SC_TCPSocket();

    // bind the local socket to a port
    RPMS_TRACE(LOGNAME, "Binding to local port ..." );
    h->bind( SS_Ini::instance()->getInt("election", "election.tcp.port") );

    std::string msg;
    while(1)
    {
        RPMS_DEBUG(LOGNAME, "Calling receive ... " );
        msg = h->receive();
        RPMS_DEBUG(LOGNAME, "msg received [" + msg + "]" );
    }
    
    CATCH;
}


