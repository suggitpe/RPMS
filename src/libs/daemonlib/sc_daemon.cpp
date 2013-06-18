//#############################
#include "sc_daemon.hpp"
#include "sx_daemon.hpp"
#include "si_daemonlog.hpp"
#include "sc_heartbeatthread.hpp"
#include "sc_processmanager.hpp"
#include "sc_statequeue.hpp"

#include <directorlib/sc_directorthread.hpp>
#include <idllib/idls/sr_directortask.h>

#include <tools/st_guard.hpp>
#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>

#include <corbaimpl/ss_corbaimpl.hpp>
#include <corbaimpl/sx_corbaimpl.hpp>
#include <election/ss_electionfactory.hpp>
#include <election/sa_election.hpp>
#include <logging/si_logging.hpp>
#include <sockets/sc_udpsocket.hpp>
#include <sockets/sc_inetaddress.hpp>
#include <utilities/ss_ini.hpp>
#include <xmlutil/sc_xmlstatus.hpp>
#include <xmlutil/sc_xmlprocstats.hpp>

using rpms::SC_Daemon;

///////////////////////////////////////////
SC_Daemon::~SC_Daemon()
{
    TRY(SC_Daemon::~SC_Daemon());
        try
        {
            SS_CorbaImpl::instance()->deRegisterPoa(this);
        }
        catch( SX_CorbaImpl &ci )
        {
            RPMS_WARN( DMN_LOG, "Caught CorbaImpl exception in daemon dtor .. not rethrowing:\n" + ci.reason() );
        }
    CATCH;
}

///////////////////////////////////////////
SC_Daemon::SC_Daemon()
    : mHeartbeatThread(0), mDirectorThread(0), mDirectorHost(false), 
        mRun(true), mLocalHost("Unset"), mInstanceId(0), mStateQueue(0), 
        mStateLock("StateLock"), mAppRunning(false)
{
    TRY(SC_Daemon::SC_Daemon());
    CATCH;
}

///////////////////////////////////////////
SC_Daemon::SC_Daemon( const SC_Daemon &aRhs )
    : mStateLock("Dummy")
{
    TRY(SC_Daemon::SC_Daemon( const SC_Daemon &aRhs ));
        throw SX_Daemon("copy ctopr not allowed for daemon");      
    CATCH;
}

///////////////////////////////////////////
SC_Daemon& SC_Daemon::operator=( const SC_Daemon &aRhs )
{
    TRY(SC_Daemon::operator=( const SC_Daemon &aRhs ));
        throw SX_Daemon("no assignment operator allowed for daemon");
    CATCH;
}

///////////////////////////////////////////
void SC_Daemon::shutdown()
{
    TRY(SC_Daemon::shutdown());
        RPMS_DEBUG( DMN_LOG, "--------- > Shutting down Daemon application");
        // we have to be careful here as we may end up with seg faults with null pointers
        mRun = false;
        if( !mDirectorThread.empty() )
        {
            RPMS_DEBUG( DMN_LOG, "--------- > killing director");
            mDirectorThread->stop();
            mDirectorThread->join();
            mDirectorThread = 0;
        }

        mProcMgr->stopAllProcesses();

        if( !mHeartbeatThread.empty() )
        {
            RPMS_DEBUG( DMN_LOG, "--------- > killing heartbeat");
            mHeartbeatThread->stop();
            mHeartbeatThread->join();
            mHeartbeatThread = 0;
        }
        RPMS_DEBUG( DMN_LOG, "--------- > Daemon application shutdown completed");
    CATCH;
}

///////////////////////////////////////////
void SC_Daemon::init()
{
    TRY(SC_Daemon::init());
        RPMS_DEBUG( DMN_LOG, "Initialising Daemon");
        mInstanceId = SS_Ini::instance()->getULong("general", "system.instanceid");
        mProcMgr = new SC_ProcessManager;
        mStateQueue = new SC_StateQueue;
        SS_CorbaImpl::instance()->initPoa(this);
        mHeartbeatThread = SC_HeartbeatThread::create();
        mLocalHost = SC_InetAddress::getLocalHostName();
        RPMS_TRACE( DMN_LOG, "Daemon hostname ["+ mLocalHost + "]");
        SS_CorbaImpl::instance()->start();
        SS_ElectionFactory::instance()->getElection(true); // initialise
        RPMS_TRACE( DMN_LOG, "Daemon initialised");
    CATCH;
}

///////////////////////////////////////////
void SC_Daemon::run()
{
    using rpms::convert;
    TRY(SC_Daemon::run());
        init();
        int registeringFailureSleep = SS_Ini::instance()->getInt("daemon", "register.failure.sleep");
        int heartbeatCheckSleep = SS_Ini::instance()->getInt("daemon", "heartbeat.check.sleep");
        int rank = SS_ElectionFactory::instance()->getElection()->getMyRank();
        RPMS_INFO(DMN_LOG, "Determined rank to be [" + convert<std::string>(rank) 
                + "] ... starting main loop");
        while( mRun )
        {
            if( 0 > rank)
            {
                RPMS_INFO(DMN_LOG,"This node has been configured never to run");
                return;
            }
            else if( SS_ElectionFactory::instance()->getElection()->shouldILead() )
            {
                if( mDirectorThread.empty() || !mDirectorThread->isRunning() )
                {
                    mDirectorThread = 0; // reset first
                    // start the director
                    RPMS_INFO(DMN_LOG,"Starting a director thread");
                    mDirectorThread = SC_DirectorThread::create();
                    mDirectorThread->start();
                    mDirectorHost = true;
                }
                else
                {
                    RPMS_INFO(DMN_LOG, "Director is already running");
                }
            
                // wait for the director to initialise
                RPMS_DEBUG(DMN_LOG, "Checking if Director Thread has been initialised or killed off");
                while( (!mDirectorThread.empty()) && (!mDirectorThread->isInitialised()) )
                {
                    if( !mDirectorThread->isRunning() )
                    {
                        throw SX_Daemon("Faild to run the director process, died in initialisation");
                    }
                    RPMS_DEBUG( DMN_LOG, "Waiting for the director to initialise");
                    usleep(100000); // just to stop the CPU being toasted
                }
            }
            else
            {
                RPMS_INFO(DMN_LOG,"This node will run as a slave node");
                if( !mDirectorThread.empty() )
                {
                    RPMS_INFO(DMN_LOG, "Cleaning up previous director thread");
                    mDirectorThread->stop();
                    mDirectorThread = 0;
                }
                mDirectorHost = false;
            }

            // this must be able to restart the election process
            try
            {
                while( mRun && !registerDaemon() )
                {
                    RPMS_WARN(DMN_LOG, "Failed to register director with director (refused) .. will retry in a moment");
                    sleep(registeringFailureSleep);
                }
            }
            catch( SX_Daemon &dex )
            {
                RPMS_WARN(DMN_LOG, "Failed to register daemon with leader:\n" + dex.reason());
                SS_ElectionFactory::instance()->getElection(true)->clearLeaderHost();
                usleep(500000);// non configurable sleep so that we do not churn all of the CPU time
                continue;
            }
            RPMS_DEBUG( DMN_LOG, "Successfully registered with Director");
            if( !mRun )
            {
                // this saves us having to worry about the internals causing cores
                break;
            }
        
            RPMS_DEBUG( DMN_LOG, "Starting heartbeat process thread");
            mHeartbeatThread->start();
            //when the heartbeatserver stops running we know that the director has stopped
            //  too. so we must start the search again for an elected leader. If the
            //  heartbeatThread has too many faild heartbeats, the thread will stop running
            //  which is why we can simply just check to see if the heartbeat thread is still
            //  running.
            while( mRun && !mHeartbeatThread.empty() && mHeartbeatThread->isRunning() )
            {
                if( ! mStateQueue->empty() )
                {
                    checkNewState();
                }
                sleep( heartbeatCheckSleep );
            }
            
            if(mRun)
            {
                RPMS_WARN(DMN_LOG, "Heartbeat thread is no longer running .. resetting it");
                mHeartbeatThread->reset();
            }
        }
    CATCH;
}

///////////////////////////////////////////
bool SC_Daemon::registerDaemon()
{
    TRY(SC_Daemon::registerDaemon());
        RPMS_DEBUG( DMN_LOG, "Registering daemon");
        std::string idlName = SS_Ini::instance()->getString("director", "director.idl.remotename");
        std::string port = SS_Ini::instance()->getString("CORBA", "corba.orb.port");
        std::string leader = SS_ElectionFactory::instance()->getElection(true)->findLeaderHost();
        CORBA::Object_var obj;
        try
        {
            obj = SS_CorbaImpl::instance()->getRemoteObject( leader, port, idlName );
        }
        catch( SX_CorbaImpl &x )
        {
            throw SX_Daemon("Corba comm failure with the leader host [" + leader + "]");
        }
        RPMS_TRACE( DMN_LOG, "Creating local interface to remote object" );
        SR_DirectorTask_var task = SR_DirectorTask::_narrow(obj);
        if( CORBA::is_nil(task) )
        {
            throw SX_Daemon("Failed to create remore object SR_DirectorTask_var");
        }

        if( ! task->registerDaemon(mLocalHost.c_str(), mInstanceId, mAppRunning ) )
        {
            return false;
        }
        return true;
    CATCH;
}

///////////////////////////////////////////
char * SC_Daemon::getDaemonStatus()
{
    TRY(* SC_Daemon::getDaemonStatus());
        try
        {
            RPMS_INFO( DMN_LOG, "<<<<<<<< Remote call for getDaemonStatus >>>>>>>>");
            // create status object
            ST_SPointer<SC_DaemonStatus> stat = new SC_DaemonStatus(mLocalHost, true, mAppRunning, mDirectorHost );
            mProcMgr->addCompStatuses( stat );
            // now convert it
            SC_XMLStatus xml(stat);
            xml.load();
            return CORBA::string_dup(xml.getSerializedDoc().c_str());
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN(DMN_LOG, "Exception caught in remote call for getDaemonStatus: \n" + ex.reason() );
            throw; // in this instance we want to rethrow and let the corba manage the excpetion for us
        }
    CATCH;
}

///////////////////////////////////////////
char * SC_Daemon::getProcessStats()
{
    TRY(* SC_Daemon::getProcessStats());
        try
        {
            RPMS_INFO( DMN_LOG, "<<<<<<<< Remote call for getProcessStats >>>>>>>>");
            // create proc stats object
            ST_SPointer<SC_ProcStats> stats = new SC_ProcStats(mLocalHost);
            mProcMgr->addProcessStatistics(stats);
            SC_XMLProcStats xml( stats );
            xml.load();
            return CORBA::string_dup(xml.getSerializedDoc().c_str());
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN(DMN_LOG, "Exception caught in remote call for getProcessStats: \n" + ex.reason() );
            throw; // in this instance we want to rethrow and let the corba manage the excpetion for us
        }
        
    CATCH;
}

///////////////////////////////////////////
void SC_Daemon::registerWithNewDirector( const char * aHost )
{
    TRY(SC_Daemon::registerWithNewDirector( const char * aHost ));
        RPMS_INFO( DMN_LOG, "<<<<<<<< Remote call for registerWithNewDirector >>>>>>>>");
        RPMS_TRACE( DMN_LOG, "registerWithNewDirector: new host [" + std::string(aHost) + "] has taken control of the system");
        if( !SS_ElectionFactory::instance()->getElection(true)->suggestLeader(aHost) )
        {
            RPMS_TRACE( DMN_LOG, "Election has deemed suggested leader as nonesense .. doing nothing");
            return;
        }
        deRegisterCurrentDirector();
    CATCH;
}

///////////////////////////////////////////
void SC_Daemon::deRegisterCurrentDirector()
{
    TRY(SC_Daemon::deRegisterCurrentDirector());
        RPMS_INFO( DMN_LOG, "<<<<<<<< Remote call for deRegisterCurrentDirector >>>>>>>>");
        if( !mDirectorThread.empty() )
        {
            mDirectorThread->stop();
             // have to wait until it has completed its work else we will have a destroyed thread trying to access itself
            mDirectorThread->join();
            mDirectorThread = 0;
        }
        mHeartbeatThread->stop();
    CATCH;
}

///////////////////////////////////////////
CORBA::Boolean SC_Daemon::changeState( const char* aNewState, const char* aArg )
{
    TRY(changeState( const char* aNewState, const char* aArg ));
        RPMS_INFO( DMN_LOG, "<<<<<<<< Remote call for changeState >>>>>>>>");
        ST_Guard<SA_Mutex> m(&mStateLock);
        if( 0 == strcmp(aNewState, "_START") )
        {
            mStateQueue->push( SC_StateQueueObject::_START, aArg);
        }
        else if( 0 == strcmp(aNewState, "_STOP") )
        {
            mStateQueue->push( SC_StateQueueObject::_STOP, aArg);
        }
        else if( 0 == strcmp(aNewState, "_CONFIGURE") )
        {
            mStateQueue->push( SC_StateQueueObject::_CONFIGURE, aArg);
        }
        else if( 0 == strcmp(aNewState, "_RESTART") )
        {
            mStateQueue->push( SC_StateQueueObject::_RESTART, aArg);
        }
        else if( 0 == strcmp(aNewState, "_FAILSTART") )
        {
            mStateQueue->push( SC_StateQueueObject::_FAILSTART, aArg);
        }
        else
        {
            RPMS_WARN( DMN_LOG, "Unknown state [" + std::string(aNewState) + "] ... ignoring it");
            return false;
        }
        RPMS_TRACE( DMN_LOG, "Added new state transition to the queue [" + std::string(aNewState) + "]");
        return true;
    CATCH;
}

///////////////////////////////////////////
void SC_Daemon::checkNewState()
{
    TRY(SC_Daemon::checkNewState());
        ST_Guard<SA_Mutex> m(&mStateLock);
        RPMS_TRACE( DMN_LOG, "New states in the queue, analysing");
        size_t sz = mStateQueue->size();
        for( int i = 0; i < sz; ++i )
        {
            SC_StateQueueObject o = mStateQueue->front();
            switch( o.getState() )
            {
                // start whole system
                case SC_StateQueueObject::_START:
                    if( mAppRunning == false )
                    {
                        //RPMS_TRACE( DMN_LOG, "Changing state to [_START] with arg [" + o.getArg() + "]");
                        RPMS_TRACE( DMN_LOG, "Changing state to [_START] with arg [xml clob]");
                        mProcMgr->startProcesses(o.getArg());
                        mAppRunning = true;
                        // this is a blunt way to get around the issue of issuing a stop right after a start
                        sleep( SS_Ini::instance()->getInt(
                                    "daemon", "daemon.process.warmup"));
                    }
                    else
                    {
                        RPMS_TRACE( DMN_LOG, "System is already running .. no state change");
                    }
                    break;
                // stop whole system
                case SC_StateQueueObject::_STOP:
                    if( mAppRunning == true )
                    {
                        RPMS_TRACE( DMN_LOG, "Changing state to [_STOP]");
                        mProcMgr->stopAllProcesses();
                        mAppRunning = false;
                    }
                    else
                    {
                        RPMS_TRACE( DMN_LOG, "System is already stopped .. no state change");
                    }
                    break;
                // reconfigure with new xml
                case SC_StateQueueObject::_CONFIGURE:
                    if( mAppRunning == true )
                    {
                        RPMS_TRACE( DMN_LOG, "Changing state to [_RECONFIG] with arg [xml clob]");
                        mProcMgr->adaptToReconfiguration( o.getArg() );
                        sleep( SS_Ini::instance()->getInt(
                                    "daemon", "daemon.process.warmup"));
                    }
                    else
                    {
                        RPMS_INFO( DMN_LOG, "Ignoring configure command as the system state is not RUNNING");
                    }
                    break;
                // restart apps (ALL)
                case SC_StateQueueObject::_RESTART:
                    if( mAppRunning == false )
                    {
                        RPMS_INFO( DMN_LOG, "System is not running so ignoring RESTART [" + o.getArg() + "]");
                    }
                    else
                    {
                        mProcMgr->restartProcess( o.getArg(), true);
                    }
                    break;
                // restart failed apps
                case SC_StateQueueObject::_FAILSTART:
                    if( mAppRunning == false )
                    {
                        RPMS_INFO( DMN_LOG, "System is not running so ignoring FAILSTART [" + o.getArg() + "]");
                    }
                    else
                    {
                        mProcMgr->restartProcess( o.getArg(), false);
                    }
                    break;
                // ignore anything we do not understand!
                default:
                    RPMS_TRACE( DMN_LOG, "Unknow state transition");
            }
            mStateQueue->pop();
        }
    CATCH;
}

///////////////////////////////////////////
