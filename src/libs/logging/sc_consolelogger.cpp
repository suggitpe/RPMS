//#############################
#include "sc_consolelogger.hpp"

#ifndef SETW_SIZE
    #define SETW_SIZE (25 - mName.length())
#endif

#include <iostream>
#include <iomanip>
#include <si_macros.hpp>
#include <tools/st_guard.hpp>

using rpms::SC_ConsoleLogger;

///////////////////////////////////////////
SC_ConsoleLogger::~SC_ConsoleLogger()
{
    TRY(SC_ConsoleLogger::~SC_ConsoleLogger());
    CATCH;
}

///////////////////////////////////////////
SC_ConsoleLogger::SC_ConsoleLogger( const std::string &aLoggerName )
    : SA_Logger(aLoggerName)
{
    TRY(SC_ConsoleLogger::SC_ConsoleLogger( const std::string &aLoggerName ));
    CATCH;
}

///////////////////////////////////////////
void SC_ConsoleLogger::trace( const std::string &aMessage )
{
    TRY(SC_ConsoleLogger::trace( const std::string &aMessage ));
        //ST_Guard<SA_Mutex> m(&mLock);
        std::cout << mName  << std::setw(SETW_SIZE) << "TRACE: " << aMessage << std::endl;
    CATCH;
}

///////////////////////////////////////////
void SC_ConsoleLogger::debug( const std::string &aMessage )
{
    TRY(SC_ConsoleLogger::debug( const std::string &aMessage ));
        //ST_Guard<SA_Mutex> m(&mLock);
        std::cout << mName  << std::setw(SETW_SIZE) << "DEBUG: " << aMessage << std::endl;
    CATCH;
}

///////////////////////////////////////////
void SC_ConsoleLogger::info( const std::string &aMessage )
{
    TRY(SC_ConsoleLogger::info( const std::string &aMessage ));
        //ST_Guard<SA_Mutex> m(&mLock);
        std::cout << mName  << std::setw(SETW_SIZE) << "INFO : " << aMessage << std::endl;
    CATCH;
}

///////////////////////////////////////////
void SC_ConsoleLogger::warn( const std::string &aMessage )
{
    TRY(SC_ConsoleLogger::warn( const std::string &aMessage ));
        //ST_Guard<SA_Mutex> m(&mLock);
        std::cout << mName  << std::setw(SETW_SIZE) << "WARN : " << aMessage << std::endl;
    CATCH;
}

///////////////////////////////////////////
void SC_ConsoleLogger::error( const std::string &aMessage )
{
    TRY(SC_ConsoleLogger::error( const std::string &aMessage ));
        //ST_Guard<SA_Mutex> m(&mLock);
        std::cout << mName  << std::setw(SETW_SIZE) << "ERROR: " << aMessage << std::endl;
    CATCH;
}

///////////////////////////////////////////
void SC_ConsoleLogger::fatal( const std::string &aMessage )
{
    TRY(SC_ConsoleLogger::fatal( const std::string &aMessage ));
        //ST_Guard<SA_Mutex> m(&mLock);
        std::cout << mName  << std::setw(SETW_SIZE) << "FATAL: " << aMessage << std::endl;
    CATCH;
}

///////////////////////////////////////////
