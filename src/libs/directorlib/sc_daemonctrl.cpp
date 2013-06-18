//#############################
#include "sc_daemonctrl.hpp"
#include "sx_director.hpp"
#include "si_directorlog.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <sockets/sc_udpsocket.hpp>
#include <sockets/sx_sockettimeout.hpp>
#include <sockets/sx_socketinterrupt.hpp>
#include <corbaimpl/ss_corbaimpl.hpp>
#include <corbaimpl/sx_corbaimpl.hpp>

#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>

using rpms::SC_DaemonCtrl;
using rpms::ST_SPointer;

///////////////////////////////////////////
ST_SPointer<SC_DaemonCtrl> SC_DaemonCtrl::create( const std::string &aDaemonHost, 
        const std::string &aThreadName )
{
    TRY(SC_DaemonCtrl::create( const std::string &aDaemonHost, const std::string &aThreadName ));
        return new SC_DaemonCtrl(aDaemonHost, aThreadName);
    CATCH;
}

///////////////////////////////////////////
SC_DaemonCtrl::SC_DaemonCtrl( const std::string &aDaemonHost, const std::string &aThreadName)
    :SA_ThreadBase(aThreadName), mSocket(0), mDaemonHost(aDaemonHost), mShouldRun(true), mInitialised(false)
{
    TRY(SC_DaemonCtrl::SC_DaemonCtrl( const std::string &aDaemonHost, const std::string &aThreadName));
    CATCH;
}

///////////////////////////////////////////
SC_DaemonCtrl::SC_DaemonCtrl( const SC_DaemonCtrl &aRhs )
    : SA_ThreadBase("dummy")
{
    TRY(SC_DaemonCtrl::SC_DaemonCtrl( const SC_DaemonCtrl &aRhs ));
        throw SX_Director("no copy stor allowed for SC_DaemonCtrl");
    CATCH;
}

///////////////////////////////////////////
SC_DaemonCtrl& SC_DaemonCtrl::operator=( const SC_DaemonCtrl &aRhs )
{
    TRY(SC_DaemonCtrl::operator=( const SC_DaemonCtrl &aRhs ));
        throw SX_Director("no assignment operator allowed for SC_DaemonCtrl");
        return *this;
    CATCH;
}

///////////////////////////////////////////
SC_DaemonCtrl::~SC_DaemonCtrl()
{
    TRY(SC_DaemonCtrl::~SC_DaemonCtrl());
    CATCH;
}

///////////////////////////////////////////
void SC_DaemonCtrl::stop()
{
    mShouldRun = false;
}

///////////////////////////////////////////
void SC_DaemonCtrl::reset()
{
}

///////////////////////////////////////////
void SC_DaemonCtrl::init()
{
    TRY(SC_DaemonCtrl::init());
        RPMS_DEBUG(DIR_DMN_CTRL, "Initialising SC_DaemonCtrl");
        mHeartbeatPort = SS_Ini::instance()->getInt("daemon", "heartbeat.udp.port");
        mSocket = new SC_UDPSocket( SS_Ini::instance()->getInt("sockets", "socket.recv.timeout") );
        mSocket->connect( mDaemonHost.c_str(), mHeartbeatPort );
        
        std::string port = SS_Ini::instance()->getString("CORBA", "corba.orb.port");
        std::string remoteName = SS_Ini::instance()->getString("daemon", "daemon.idl.remotename");
        // initialise the remote object
        try
        {
            CORBA::Object_var obj = SS_CorbaImpl::instance()->getRemoteObject( mDaemonHost, port, remoteName );
            mRemote = SR_DaemonTask::_narrow(obj);
        }
        catch( SX_CorbaImpl &cex )
        {
            RPMS_WARN(DIR_DMN_CTRL,"Failed to connect to remote object [" + remoteName + "] on host [" + mDaemonHost + ":" + port + "]:\n" + cex.reason() );
            throw;
        }
        
        if( CORBA::is_nil(mRemote) )
        {
            throw SX_Director("Failed to get remote object [" + remoteName + "] from host [" + mDaemonHost + ":" + port + "]");
        }
        mInitialised = true;
    CATCH;
}

///////////////////////////////////////////
void SC_DaemonCtrl::run()
{
    using rpms::convert;
    TRY(SC_DaemonCtrl::run());
        init();
        int pulse = SS_Ini::instance()->getInt("director", "heartbeat.pulse");
        int maxFails = SS_Ini::instance()->getInt("director", "heartbeat.max.failedattempts");
        std::string msg;
        int failedAttempts = 0;
        sleep(1); // this allows the other end to catch up for a moment .. stops timeouts when the first heartbeat message is missed
        while( mShouldRun)
        {
            mSocket->send("ping");
            try
            {
                msg = mSocket->receive();
                failedAttempts = 0;
            }
            catch( SX_SocketInterrupt &iex )
            {
                RPMS_INFO(DIR_DMN_CTRL, "Socket receive was interrupted by signal");
            }
            catch( SX_SocketTimeout &ex )
            {
                ++failedAttempts;
                RPMS_WARN(DIR_DMN_CTRL, "Timed out waiting for heartbeat reply from [" + mDaemonHost + "] --> fail [" 
                        + convert<std::string>(failedAttempts) + "/" + convert<std::string>(maxFails) + "]");
                // increment and test
                if( failedAttempts >= maxFails )
                {
                    RPMS_ERROR(DIR_DMN_CTRL, "Exceeded maximum heartbeat reply failures .. presuming Daemon [" + mDaemonHost + "] not running");
                    mShouldRun = false;
                }
            }
            sleep(pulse); // sleep to start with
        }
        mSocket = 0;
    CATCH;
}

///////////////////////////////////////////
std::string SC_DaemonCtrl::getStatus()
{
    TRY(SC_DaemonCtrl::getStatus());
        try
        {
            return mRemote->getDaemonStatus();
        }
        catch( SX_Exception &sex )
        {
            RPMS_ERROR(DIR_DMN_CTRL, sex.stack());
            throw SX_Director("Failed to call getDaemonStatus() on remote daemon [" + mDaemonHost + "]:\n" + sex.reason() );
        }
        catch(...)
        {
            RPMS_WARN( DIR_DMN_CTRL, "Caught unknown exception in SC_DaemonCtrl::getStatus()");
            throw SX_Director("Failed to call getDaemonStatus() on remote daemon [" + mDaemonHost + "]");
        }
    CATCH
}

///////////////////////////////////////////
std::string SC_DaemonCtrl::getDaemonProcStats()
{
    TRY(SC_DaemonCtrl::getDaemonProcStats());
        try
        {
            return mRemote->getProcessStats();
        }
        catch( SX_Exception &sex )
        {
            RPMS_ERROR(DIR_DMN_CTRL, sex.stack());
            throw SX_Director("Failed to call getProcessStats() on remote daemon [" + mDaemonHost + "]:\n" + sex.reason() );
        }
        catch( ... )
        {
            RPMS_WARN( DIR_DMN_CTRL, "Caught unknown exception in SC_DaemonCtrl::getDaemonProcStats()");
            throw SX_Director("Failed to call getDaemonProcStats() on remote daemon [" + mDaemonHost + "]");
        }
    CATCH;
}

///////////////////////////////////////////
void SC_DaemonCtrl::addToDaemonStateQueue( const std::string &aState, const std::string &aArg )
{
    TRY(SC_DaemonCtrl::addToDaemonStateQueue( const std::string &aState, const std::string &aArg ));
        try
        {
            mRemote->changeState( aState.c_str(), aArg.c_str() );
        }
        catch( SX_Exception &sex )
        {
            RPMS_ERROR(DIR_DMN_CTRL, sex.stack());
            throw SX_Director("Failed to call changeState() on remote daemon [" 
                    + mDaemonHost + "]\n" + sex.reason() );
        }
        catch(...)
        {
            RPMS_WARN( DIR_CRTL_LOG, "Caught unknown exception in SC_DaemonCtrl::addToDaemonStateQueue()");
            throw SX_Director("Failed to call changeState() on remote daemon [" 
                    + mDaemonHost + "]");
        }
    CATCH;
}

///////////////////////////////////////////
