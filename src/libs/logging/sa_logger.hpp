#ifndef __sa_logger_hpp
#define __sa_logger_hpp

#include <string>
#include <base/sa_baseobject.hpp>
#include <tools/sc_recursivemutex.hpp>

namespace rpms
{

    /** Base class for all loggers */
    class SA_Logger : public SA_BaseObject
    {
        public:
            SA_Logger( const std::string &aLoggerName );
            virtual ~SA_Logger();
    
            virtual void trace( const std::string &aLoggerName ) = 0;
            virtual void debug( const std::string &aMessage ) = 0;
            virtual void info( const std::string &aMessage ) = 0;
            virtual void warn( const std::string &aMessage ) = 0;
            virtual void error( const std::string &aMessage ) = 0;
            virtual void fatal( const std::string &aMessage ) = 0;
    
        protected:
            /** logger name */
            std::string mName;
            /** mutex to lock for thread safety */
            mutable SC_RecursiveMutex mLock;
    
        private:
    
    };

} // namespace


#endif
