#ifndef __ss_loggerfactory_hpp
#define __ss_loggerfactory_hpp

#include <base/st_singleton.hpp>
#include <base/st_spointer.hpp>
#include <logging/sa_logger.hpp>
#include <tools/sc_recursivemutex.hpp>

#include <string>
#include <map>

namespace rpms
{

    /** factory class to manage the creation of the correct types of logger */
    class SS_LoggerFactory : public ST_Singleton<SS_LoggerFactory>
    {
        friend class ST_Singleton<SS_LoggerFactory>;
        public:
            /// virtual dtor
            virtual ~SS_LoggerFactory();
    
            /// main funtion of the factory .. to create a 
            ///      logger of the correct type
            ST_SPointer<SA_Logger> getLogger( const std::string &aLoggerName );
        protected:
            // def ctor
            SS_LoggerFactory();
            
        private:
            SS_LoggerFactory( const SS_LoggerFactory &aRhs );
            SS_LoggerFactory& operator=( const SS_LoggerFactory &aRhs );
            /// typedef to make life easier
            typedef std::map< std::string, ST_SPointer<SA_Logger> > logMap;
            /// typedef to make life easier
            typedef std::pair< std::string, ST_SPointer<SA_Logger> > logPair;
            /// underlying map of loggers
            logMap * mLoggers;
            /// add a new logger to the map
            void add( const std::string &aName, const ST_SPointer<SA_Logger> &aLogger );
            /// mutex to lock for thread safety
            mutable SC_RecursiveMutex mLock;
            /// string for the logger type (from ini)
            std::string mLoggerType;
            
    };

} //namespace

#endif

