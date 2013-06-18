//#############################
#include "sc_electioncommthread.hpp"
#include "si_electionlog.hpp"
#include "sa_election.hpp"

#include <si_macros.hpp>

#include <sockets/sc_tcpsocket.hpp>
#include <sockets/sx_socketconnectrefused.hpp>
#include <logging/si_logging.hpp>

using rpms::SC_ElectionCommThread;
using rpms::ST_SPointer;

///////////////////////////////////////////
SC_ElectionCommThread::~SC_ElectionCommThread()
{
    TRY(SC_ElectionCommThread::~SC_ElectionCommThread());
    CATCH;
}

///////////////////////////////////////////
SC_ElectionCommThread::SC_ElectionCommThread( const std::string &aThreadName, const ST_SPointer<SA_Election> &aElector )
    : SA_ThreadBase(aThreadName), mShouldRun(true), mRecvSocket(0), mOwner(aElector)
{
    TRY(SC_ElectionCommThread::SC_ElectionCommThread( const std::string &aThreadName, const ST_SPointer<SA_Election> &aElector ));
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_ElectionCommThread> SC_ElectionCommThread::create(const ST_SPointer<SA_Election> &aElector)
{
    TRY(SC_ElectionCommThread::create());
        return new SC_ElectionCommThread("ElectionCommThread", aElector);
    CATCH;
}

///////////////////////////////////////////
void SC_ElectionCommThread::stop()
{
    TRY(SC_ElectionCommThread::stop());
        // we need to do two things firstly set the run state to false
        //      then we need to send the TCP socket a 0 byte message to 
        //      get it to process it
        mShouldRun = false;
        ST_SPointer<SA_Socket> h = new SC_TCPSocket();
        try
        {
            h->connect( mOwner->getLocalHost().c_str(), mOwner->getElectionPort() );
        }
        catch( SX_SocketConnectRefused &s )
        {
            RPMS_INFO( ELC_COM_LOG, "Could not connect to local socket to get it to stop:\n" + s.reason() );
        }
    CATCH;
}


///////////////////////////////////////////
void SC_ElectionCommThread::reset()
{
    TRY(SC_ElectionCommThread::reset());
        stop();
        init();
        mShouldRun = true;
    CATCH;
}

///////////////////////////////////////////
void SC_ElectionCommThread::run()
{
    TRY(SC_ElectionCommThread::run());
        init();
        std::string msg;
        while( mShouldRun )
        {
            RPMS_DEBUG( ELC_COM_LOG, "Waiting for election message ...");
            msg = mRecvSocket->receive();
            if( 0 > msg.size() )
            {
                RPMS_DEBUG( ELC_COM_LOG, "Election connection made but 0 bytes sent");
            }
            else
            {
                mOwner->processElectionMsg(msg);
            }
        }
    CATCH;
}

///////////////////////////////////////////
void SC_ElectionCommThread::init()
{
    TRY(SC_ElectionCommThread::init());
        mRecvSocket = new SC_TCPSocket();
        mRecvSocket->bind(mOwner->getElectionPort());
    CATCH;
}

///////////////////////////////////////////
