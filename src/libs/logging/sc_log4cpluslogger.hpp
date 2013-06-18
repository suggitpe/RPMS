#ifndef __sc_log4cpluslogger_hpp
#define __sc_log4cpluslogger_hpp

#include <base/st_spointer.hpp>
#include <logging/sa_logger.hpp>

namespace rpms
{

    class SC_Log4cplusWrapper;
    
    /** Log4cplus logger implementation */
    class SC_Log4cplusLogger : public SA_Logger
    {
        public:
            SC_Log4cplusLogger( const std::string &aLoggerName );
            virtual ~SC_Log4cplusLogger();
    
            virtual void trace( const std::string &aMessage );
            virtual void debug( const std::string &aMessage );
            virtual void info( const std::string &aMessage );
            virtual void warn( const std::string &aMessage );
            virtual void error( const std::string &aMessage );
            virtual void fatal( const std::string &aMessage );
    
        protected:
    
        private:
            /// logger instance
            ST_SPointer<SC_Log4cplusWrapper> mLogger;
    };

} // namespace

#endif
