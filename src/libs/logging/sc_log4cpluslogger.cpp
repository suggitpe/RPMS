//#############################
#include "sc_log4cpluslogger.hpp"
#include "ss_log4cplusadapter.hpp"
#include <si_macros.hpp>

using rpms::SC_Log4cplusLogger;

///////////////////////////////////////////
SC_Log4cplusLogger::~SC_Log4cplusLogger()
{
    TRY(SC_Log4cplusLogger::~SC_Log4cplusLogger());
    CATCH;
}

///////////////////////////////////////////
SC_Log4cplusLogger::SC_Log4cplusLogger( const std::string &aLoggerName )
    : SA_Logger(aLoggerName), mLogger(0)
{
    TRY(SC_Log4cplusLogger::SC_Log4cplusLogger( const std::string &aLoggerName ));
        // done here in case of exceptions thrown
        mLogger = SS_Log4cplusAdapter::instance()->getLogger(aLoggerName);
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusLogger::trace( const std::string &aMessage )
{
    TRY(SC_Log4cplusLogger::trace( const std::string &aMessage ));
        mLogger->log( SC_Log4cplusWrapper::TRACE, aMessage );
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusLogger::debug( const std::string &aMessage )
{
    TRY(SC_Log4cplusLogger::debug( const std::string &aMessage ));
        mLogger->log( SC_Log4cplusWrapper::DEBUG, aMessage );
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusLogger::info( const std::string &aMessage )
{
    TRY(SC_Log4cplusLogger::info( const std::string &aMessage ));
        mLogger->log( SC_Log4cplusWrapper::INFO, aMessage );
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusLogger::warn( const std::string &aMessage )
{
    TRY(SC_Log4cplusLogger::warn( const std::string &aMessage ));
        mLogger->log( SC_Log4cplusWrapper::WARN, aMessage );
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusLogger::error( const std::string &aMessage )
{
    TRY(SC_Log4cplusLogger::error( const std::string &aMessage ));
        mLogger->log( SC_Log4cplusWrapper::ERROR, aMessage );
    CATCH;
}

///////////////////////////////////////////
void SC_Log4cplusLogger::fatal( const std::string &aMessage )
{
    TRY(SC_Log4cplusLogger::fatal( const std::string &aMessage ));
        mLogger->log( SC_Log4cplusWrapper::FATAL, aMessage );
    CATCH;
}

///////////////////////////////////////////
