#ifndef __sc_consolelogger_hpp
#define __sc_consolelogger_hpp

#include <logging/sa_logger.hpp>

namespace rpms
{

    /** ostream implementation of a logger */
    class SC_ConsoleLogger : public SA_Logger
    {
        public:
            SC_ConsoleLogger( const std::string &aLoggerName );
            virtual ~SC_ConsoleLogger();
    
            virtual void trace( const std::string &aMessage );
            virtual void debug( const std::string &aMessage );
            virtual void info( const std::string &aMessage );
            virtual void warn( const std::string &aMessage );
            virtual void error( const std::string &aMessage );
            virtual void fatal( const std::string &aMessage );
    
        protected:
    
    };

} // namespace

#endif
