//#############################
#include "sc_directorthread.hpp"
#include "sx_director.hpp"
#include "si_directorlog.hpp"
#include "sc_dirremotectrls.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <corbaimpl/ss_corbaimpl.hpp>
#include <corbaimpl/sx_corbaimpl.hpp>
#include <election/ss_electionfactory.hpp>
#include <election/sa_election.hpp>

#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>
#include <tools/st_guard.hpp>

using rpms::SC_DirectorThread;
using rpms::ST_SPointer;

///////////////////////////////////////////
ST_SPointer<SC_DirectorThread> SC_DirectorThread::create()
{
    TRY(SC_DirectorThread::create());
        return new SC_DirectorThread(SS_Ini::instance()->getString("threadnames", "director"));
    CATCH;
}

///////////////////////////////////////////
SC_DirectorThread::~SC_DirectorThread()
{
    TRY(SC_DirectorThread::~SC_DirectorThread());
        RPMS_TRACE( DIR_DIR_LOG, "Deregistering poa" );
        try
        {
            SS_CorbaImpl::instance()->deRegisterPoa(this);
        }
        catch( SX_CorbaImpl &ci )
        {
            RPMS_WARN( DIR_DIR_LOG, "Caught CorbaImpl exception in director dtor .. not rethrowing:\n" + ci.reason() );
        }
    CATCH;
}

///////////////////////////////////////////
SC_DirectorThread::SC_DirectorThread( const std::string &aName )
    : SA_ThreadBase(aName), mShouldRun(true), mInitialised(false), mLock("SC_DirectorThread"), 
        mInstanceId(0)
{
    TRY(SC_DirectorThread( const std::string &aName ));
        mControllers = new SC_DirRemoteCtrls;
    CATCH;
}

///////////////////////////////////////////
SC_DirectorThread::SC_DirectorThread( const SC_DirectorThread &aRhs )
    : SA_ThreadBase("dead"), mLock("dummy")
{
    TRY(SC_DirectorThread::SC_DirectorThread( const SC_DirectorThread &aRhs ));
        throw SX_Director("copy ctor not allowed in SC_DirectorThread");
    CATCH;
}

///////////////////////////////////////////
SC_DirectorThread& SC_DirectorThread::operator=( const SC_DirectorThread &aRhs )
{
    TRY(SC_DirectorThread::operator=( const SC_DirectorThread &aRhs ));
        throw SX_Director("assignment operator not allowed in SC_DirectorThread");
        return *this;
    CATCH;
}

///////////////////////////////////////////
void SC_DirectorThread::stop()
{
    TRY( SC_DirectorThread::stop());
        ST_Guard<SA_Mutex> m( &mLock );
        RPMS_DEBUG( DIR_DIR_LOG, "Stopping Director Thread");
        mControllers->stopAll();
        mShouldRun = false;
    CATCH;
}

///////////////////////////////////////////
void SC_DirectorThread::run()
{
    TRY(SC_DirectorThread::run());
        init();
        while(mShouldRun)
        {
            mControllers->checkDaemons();
            sleep(1);
        }
        RPMS_DEBUG( DIR_DIR_LOG, "Director thread has completed work");
    CATCH;
}

///////////////////////////////////////////
void SC_DirectorThread::reset()
{
    TRY(SC_DirectorThread::reset());
        ST_Guard<SA_Mutex> m( &mLock );
        SA_ThreadBase::reset();
    CATCH;
}

/////////////////////////////////////////// DONE
void SC_DirectorThread::init()
{
    TRY(SC_DirectorThread::init());
        ST_Guard<SA_Mutex> m( &mLock );
        RPMS_DEBUG( DIR_DIR_LOG, "Initialising director");
        mInstanceId = SS_Ini::instance()->getULong("general", "system.instanceid");
        mControllers->initialise();
        SS_ElectionFactory::instance()->getElection(true)->allowMeToLead();
        SS_CorbaImpl::instance()->initPoa(this);
        // job done!
        mInitialised = true;
        RPMS_TRACE( DIR_DIR_LOG, "Initialised director");
    CATCH;
}

/////////////////////////////////////////// DONE
CORBA::Boolean SC_DirectorThread::clientStart()
{
    TRY(SC_DirectorThread::clientStart());
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientStart >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            return mControllers->startStopSystemComponents(true);
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientStart:\n" + ex.reason() );
            return false;
        }
    CATCH;
}

/////////////////////////////////////////// DONE
CORBA::Boolean SC_DirectorThread::clientStop()
{
    TRY(SC_DirectorThread::clientStop());
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientStop >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            return mControllers->startStopSystemComponents(false);
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientStop:\n" + ex.reason() );
            return false;
        }
    CATCH;
}

/////////////////////////////////////////// DONE
char * SC_DirectorThread::clientGetStatus()
{
    TRY(SC_DirectorThread::clientGetStatus());
        try
        {
            ST_Guard<SA_Mutex> m( &mLock );
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientGetStatus >>>>>>>>");
            RPMS_TRACE( DIR_DIR_LOG, "clientGetStatus" );
            return CORBA::string_dup( mControllers->getDaemonStatus().c_str() );
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientGetStatus:\n" + ex.reason() );
            return CORBA::string_dup("<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>");
        }
    CATCH;
}


/////////////////////////////////////////// DONE
char * SC_DirectorThread::distGetStats()
{
    TRY(SC_DirectorThread::distGetStats());
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for distGetStats >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            RPMS_TRACE( DIR_DIR_LOG, "distGetStats" );
            return CORBA::string_dup( mControllers->getProcessStats().c_str() );
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from distGetStats:\n" + ex.reason() );
            return CORBA::string_dup("<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"no\" ?>");
        }
    CATCH;
}

/////////////////////////////////////////// DONE
CORBA::Boolean SC_DirectorThread::clientReconfigure( const char * aForce )
{
    TRY(SC_DirectorThread::clientReconfigure( const char * aForce ));
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientReconfigure force=[" + std::string(strlen(aForce) > 0?"true":"false") + "] >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            return mControllers->reconfigureSystem( (strlen(aForce) > 0) );
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientReconfigure:\n" + ex.reason() );
            return false;
        }
    CATCH;
}

/////////////////////////////////////////// DONE
CORBA::Boolean SC_DirectorThread::clientRestartFailedApp( const char * aName )
{
    TRY(SC_DirectorThread::clientRestartFailedApp( const char * aName ));
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientRestartFailedApp name=[" + std::string(aName) + "] >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            return mControllers->restartSystemComponent( aName, false );
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientRestartFailedApp:\n" + ex.reason() );
            return false;
        }
    CATCH;
}

/////////////////////////////////////////// DONE
CORBA::Boolean SC_DirectorThread::clientRestartApp( const char * aName )
{
    TRY(SC_DirectorThread::clientRestartApp( const char * aName ));
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientRestartApp name=[" + std::string(aName) + "] >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            return mControllers->restartSystemComponent( aName, true );
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientRestartApp:\n" + ex.reason() );
            return false;
        }
    CATCH;
}

/////////////////////////////////////////// DONE
void SC_DirectorThread::clientStopDirector()
{
    TRY(SC_DirectorThread::clientStopDirector());
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for clientStopDirector >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            RPMS_TRACE( DIR_DIR_LOG, "clientStopDirector - stopping the director thread" );
            stop();
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from clientStopDirector:\n" + ex.reason() );
        }
    CATCH;
}

/////////////////////////////////////////// DONE
CORBA::Boolean SC_DirectorThread::registerDaemon( const char * aHostName, const unsigned long aInstanceId, CORBA::Boolean aIsAppRunning )
{
    using rpms::convert;
    TRY(SC_DirectorThread::registerDaemon( const char * aHostName, const unsigned long aInstanceId, CORBA::Boolean aIsAppRunning ));
        try
        {
            RPMS_INFO( DIR_DIR_LOG, "<<<<<<<< Remote call for registerDaemon >>>>>>>>");
            ST_Guard<SA_Mutex> m( &mLock );
            RPMS_TRACE( DIR_DIR_LOG, "Registering Daemon on [" + std::string(aHostName) + "]" );
            if( mInstanceId != aInstanceId )
            {
                RPMS_ERROR(DIR_DIR_LOG, "Trying to register daemon with different instance id: daemon [" 
                    + convert<std::string>(aInstanceId) + "] director [" 
                    + convert<std::string>(mInstanceId) + "]");
                return false;
            }
            return mControllers->addDaemon( aHostName, aIsAppRunning );
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_DIR_LOG, "Exception caught from registerDaemon:\n" + ex.reason() );
            return false;
        }
    CATCH;
}

///////////////////////////////////////////
