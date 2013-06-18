//#############################
#include "ss_loggerfactory.hpp"
#include "sc_log4cpluslogger.hpp"
#include "sc_consolelogger.hpp"
#include "sx_logger.hpp"
#include <utilities/ss_ini.hpp>
#include <tools/st_guard.hpp>
#include <si_macros.hpp>

using rpms::SS_LoggerFactory;
using rpms::ST_SPointer;
using rpms::SA_Logger;

///////////////////////////////////////////
SS_LoggerFactory::SS_LoggerFactory()
    : mLock("SS_LoggerFactory"), mLoggers( new logMap )
{
    TRY(SS_LoggerFactory::SS_LoggerFactory());
        mLoggerType = SS_Ini::instance()->getString( "logging", "type" );
    CATCH;
}

///////////////////////////////////////////
SS_LoggerFactory::~SS_LoggerFactory()
{
    TRY(SS_LoggerFactory::~SS_LoggerFactory());
        delete mLoggers;
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SA_Logger> SS_LoggerFactory::getLogger( const std::string &aLoggerName )
{
    TRY(SS_LoggerFactory::getLogger( const std::string &aLoggerName ));
        // lock against other threads
        ST_Guard<SA_Mutex> m( &mLock );

        logMap::iterator i = mLoggers->find(aLoggerName);
        if( i != mLoggers->end() )
        {
            return i->second;
        }
        else
        {
            if( "log4cplus" == mLoggerType )
            {
                add( aLoggerName, new SC_Log4cplusLogger(aLoggerName));
            }
            else if( "console" == mLoggerType )
            {
                add( aLoggerName, new SC_ConsoleLogger(aLoggerName));
            }
            else
            {
                throw SX_Logger("No logger of type [" + mLoggerType + "] available");
            }
            return mLoggers->find(aLoggerName)->second;
        }
    CATCH;
}

///////////////////////////////////////////
void SS_LoggerFactory::add( const std::string &aName, const ST_SPointer<SA_Logger> &aLogger )
{
    TRY(SS_LoggerFactory::add( const std::string &aName, const ST_SPointer<SA_Logger> &aLogger ));
        logMap::iterator i = mLoggers->find(aName);
        if( i == mLoggers->end() )
        {
            mLoggers->insert( logPair(aName, aLogger) );
        }   
        else
        {
            throw SX_Logger("trying to add a duplicate logger [" + aName + "]" );
        }
    CATCH;
}

///////////////////////////////////////////
