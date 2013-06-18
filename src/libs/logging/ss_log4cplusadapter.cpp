//#############################
#include "ss_log4cplusadapter.hpp"
#include "sx_logger.hpp"
#include <utilities/ss_ini.hpp>
#include <iostream>
#include <si_macros.hpp>

using rpms::SS_Log4cplusAdapter;
using rpms::SC_Log4cplusWrapper;
using rpms::ST_SPointer;

///////////////////////////////////////////
SS_Log4cplusAdapter::SS_Log4cplusAdapter()
    : mConfigurator(0), mHierarchy(0)
{
    TRY(SS_Log4cplusAdapter::SS_Log4cplusAdapter());
        initialise();
    CATCH;
}

///////////////////////////////////////////
SS_Log4cplusAdapter::~SS_Log4cplusAdapter()
{
    TRY(SS_Log4cplusAdapter::~SS_Log4cplusAdapter());
        //first close all the existing connections to the loggers, 
        //      then delete the objects
        mHierarchy->shutdown();
        delete mConfigurator;
        delete mHierarchy;
    CATCH;
}

///////////////////////////////////////////
SS_Log4cplusAdapter::SS_Log4cplusAdapter( const SS_Log4cplusAdapter &aRhs )
{
    TRY(SS_Log4cplusAdapter::SS_Log4cplusAdapter( const SS_Log4cplusAdapter &aRhs ));
        // hidden
    CATCH;
}

///////////////////////////////////////////
SS_Log4cplusAdapter & SS_Log4cplusAdapter::operator=( const SS_Log4cplusAdapter &aRhs )
{
    //TRY(SS_Log4cplusAdapter::operator=( const SS_Log4cplusAdapter &aRhs ));
        // hidden
        return *this;
    //CATCH;
}

///////////////////////////////////////////
void SS_Log4cplusAdapter::initialise()
{
    TRY(SS_Log4cplusAdapter::initialise());
        mLog4cplusCfg = (SS_Ini::instance()->getString( "directories", "config" )) 
            + "/" + (SS_Ini::instance()->getString( "logging", "log4cplus.cfg.file" ));
        mHierarchy = new log4cplus::Hierarchy();
        mConfigurator = new log4cplus::PropertyConfigurator( mLog4cplusCfg, *mHierarchy );
        mConfigurator->configure();
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_Log4cplusWrapper> SS_Log4cplusAdapter::getLogger( const std::string &aLoggerName )
{
    TRY(SS_Log4cplusAdapter::getLogger( const std::string &aLoggerName ));
        return new SC_Log4cplusWrapper( mHierarchy->getInstance(aLoggerName.c_str()) );
    CATCH;
}

// ##############################################################
// ##############################################################
// ##############################################################
SC_Log4cplusWrapper::SC_Log4cplusWrapper()
    : mLogger(0), mLock("SC_Log4cplusWrapper_lock")
{
    TRY(SC_Log4cplusWrapper::SC_Log4cplusWrapper());
        // hidden
    CATCH;
}

///////////////////////////////////////////
SC_Log4cplusWrapper::SC_Log4cplusWrapper( const log4cplus::Logger &aLogger )
    : mLock("SC_Log4cplusWrapper_lock")
{
    TRY(SC_Log4cplusWrapper::SC_Log4cplusWrapper( const log4cplus::Logger &aLogger ));
        mLogger = new log4cplus::Logger(aLogger);
    CATCH;
}

///////////////////////////////////////////
SC_Log4cplusWrapper::~SC_Log4cplusWrapper()
{
    TRY(SC_Log4cplusWrapper::~SC_Log4cplusWrapper());
        delete mLogger;
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusWrapper::log( ELogLevel aLevel, const std::string &aMessage )
{
    TRY(SC_Log4cplusWrapper::log( ELogLevel aLevel, const std::string &aMessage ));
        mLock.lock();
        switch (aLevel)
        {
            case TRACE:
                mLogger->log(log4cplus::TRACE_LOG_LEVEL, aMessage.c_str() );
                break;
            case DEBUG:
                mLogger->log(log4cplus::DEBUG_LOG_LEVEL, aMessage.c_str() );
                break;
            case INFO:
                mLogger->log(log4cplus::INFO_LOG_LEVEL, aMessage.c_str() );
                break;
            case WARN:
                mLogger->log(log4cplus::WARN_LOG_LEVEL, aMessage.c_str() );
                break;
            case ERROR:
                mLogger->log(log4cplus::ERROR_LOG_LEVEL, aMessage.c_str() );
                break;
            case FATAL:
                mLogger->log(log4cplus::FATAL_LOG_LEVEL, aMessage.c_str() );
                break;
            default:
                mLock.unlock();
                throw SX_Logger("Unknown logging level to display log: " + aMessage );
        }
        mLock.unlock();
    CATCH;
}

///////////////////////////////////////////
