//#############################
#include "sc_heartbeatthread.hpp"
#include "si_daemonlog.hpp"
#include "sx_daemon.hpp"

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <sockets/sc_udpsocket.hpp>
#include <sockets/sx_sockettimeout.hpp>
#include <sockets/sx_socketinterrupt.hpp>
#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>

using rpms::SC_HeartbeatThread;
using rpms::ST_SPointer;

////////////////////////////
ST_SPointer<SC_HeartbeatThread> SC_HeartbeatThread::create()
{
    TRY(SC_HeartbeatThread::create());
        return new SC_HeartbeatThread(SS_Ini::instance()->getString("threadnames", "heartbeat"));
    CATCH;
}

////////////////////////////
SC_HeartbeatThread::SC_HeartbeatThread( const std::string &aThreadName )
    : SA_ThreadBase(aThreadName), mShouldRun(true), mHeartbeatPort(0), mSocket(0)
{
    TRY(SC_HeartbeatThread::SC_HeartbeatThread( const std::string &aThreadName ));
    CATCH;
}

////////////////////////////
SC_HeartbeatThread::~SC_HeartbeatThread()
{
    TRY(SC_HeartbeatThread::~SC_HeartbeatThread());
    CATCH;
}

///////////////////////////////////////////
SC_HeartbeatThread::SC_HeartbeatThread( const SC_HeartbeatThread &aRhs )
    : SA_ThreadBase("dummy")
{
    TRY(SC_HeartbeatThread::SC_HeartbeatThread( const SC_HeartbeatThread &aRhs ));
        throw SX_Daemon("No copy ctor allowed for heartbeat thread");
    CATCH;
}

///////////////////////////////////////////
SC_HeartbeatThread& SC_HeartbeatThread::operator=( const SC_HeartbeatThread &aRhs )
{
    TRY(SC_HeartbeatThread::operator=( const SC_HeartbeatThread &aRhs ));
        throw SX_Daemon("no assignment operator allowed for heartbeat thread");
        return *this;
    CATCH;
}

////////////////////////////
void SC_HeartbeatThread::stop()
{
    TRY(SC_HeartbeatThread::stop());
        RPMS_TRACE(HBT_LOG, "Stopping heartbeat thread");
        mShouldRun = false;
        RPMS_TRACE( HBT_LOG, "Sending a dummy message to break the heartbeat timeout");
        SC_UDPSocket soc;
        soc.connect( SC_InetAddress(SC_InetAddress::getLocalHostName().c_str(), SS_Ini::instance()->getInt("daemon", "heartbeat.udp.port")) );
        soc.send("die");
    CATCH;
}

////////////////////////////
void SC_HeartbeatThread::run()
{
    using rpms::convert;
    TRY(SC_HeartbeatThread::run());
        init();
        std::string msg;
        int failedAttempts = 0;
        while( mShouldRun )
        {
            try
            {
                // wait for a message from the heartbeating process
                msg = mSocket->receive();
                RPMS_TRACE(HBT_LOG, "--------------- received heartbeat from leader");
                if( !mShouldRun )
                {
                    return;
                }

                // we have just received one
                if( failedAttempts > 0 ) // cheaper to test than assign
                {
                    failedAttempts = 0; // we are only interested in the failed attempts
                }

                if( !mSocket->isConnected() )
                {
                    mSocket->connect( *mSocket->getOtherAddress() );
                }
                mSocket->send(msg);
            }
            catch( SX_SocketTimeout &ex )
            {
                ++failedAttempts;
                RPMS_WARN( HBT_LOG, "Timed out waiting on socket [" + convert<std::string>(failedAttempts) 
                        + "/" + convert<std::string>(mMaxFailedAttempts) + "]");
                if( failedAttempts >= mMaxFailedAttempts )
                {
                    RPMS_WARN(HBT_LOG, "Exceeded the maximum number of failed heartbeat timeouts ... presuming director not running");
                    mShouldRun = false;
                }
            }
            catch( SX_SocketInterrupt &iex ) 
            {
                RPMS_INFO(HBT_LOG, "Socket was interrupted during the receive");
            }
            catch( SX_Socket &sex )
            {
                RPMS_WARN( HBT_LOG, "SX_Socket caught:\n" + sex.reason() );
                mShouldRun = false;
            }
        }
    CATCH;
}

////////////////////////////
void SC_HeartbeatThread::init()
{
    TRY(SC_HeartbeatThread::init());
        RPMS_TRACE( HBT_LOG, "Initialising heartbeat thread");
        mHeartbeatPort = SS_Ini::instance()->getInt("daemon", "heartbeat.udp.port");
        mMaxFailedAttempts = SS_Ini::instance()->getInt("daemon", "heartbeat.max.failedattempts" );
        mSocket = new SC_UDPSocket( SS_Ini::instance()->getInt("sockets", "socket.recv.timeout") );
        mSocket->bind( mHeartbeatPort );
        RPMS_TRACE( HBT_LOG, "Heartbeat thread ready");
    CATCH;
}

////////////////////////////
void SC_HeartbeatThread::reset()
{
    TRY(SC_HeartbeatThread::reset());
        // must call the base too so that we guarantee that the thread is cleaned up
        SA_ThreadBase::reset();
        mSocket = 0;
        mShouldRun = true;
        RPMS_TRACE( HBT_LOG, "Heartbeat thread has been reset");
    CATCH;
}

////////////////////////////
