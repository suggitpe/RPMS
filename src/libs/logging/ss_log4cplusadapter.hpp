#ifndef __ss_log4cplusadapter_hpp
#define __ss_log4cplusadapter_hpp

#include <base/st_singleton.hpp>
#include <base/st_spointer.hpp>
#include <base/sa_baseobject.hpp>
#include <tools/sc_basicmutex.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#include <string>

namespace rpms
{

    // pre decl
    class SC_Log4cplusWrapper;
    
    /** Singleton container class for Log4cplus loggers and their management */
    class SS_Log4cplusAdapter : public ST_Singleton<SS_Log4cplusAdapter>
    {
        friend class ST_Singleton<SS_Log4cplusAdapter>;
        public:
            /// virtual dtor
            virtual ~SS_Log4cplusAdapter();
        private:
            /// default ctor
            SS_Log4cplusAdapter();
            /// hidden copy ctor
            SS_Log4cplusAdapter( const SS_Log4cplusAdapter &aRhs );
            /// hiden assignment operator
            SS_Log4cplusAdapter & operator=( const SS_Log4cplusAdapter &aRhs );
    
        public:
            /// get a logger object so we can send messages
            ST_SPointer<SC_Log4cplusWrapper> getLogger( const std::string &aLoggerName );
    
        private:
            /// initialise the adapter
            void initialise();
            /// log4cplus config file
            std::string mLog4cplusCfg;
            /// log4cplus property configurator (no def ctor so use a ptr)
            log4cplus::PropertyConfigurator * mConfigurator;
            /// log4cplus hierarchy so we can get hold of the raw loggers
            log4cplus::Hierarchy * mHierarchy;
    };
    
    /** Wrapper for the Log4cplus Logger API */
    class SC_Log4cplusWrapper : public SA_BaseObject
    {
        public:
            /** Logger ctor */
            SC_Log4cplusWrapper( const log4cplus::Logger &aLogger );
            /** std dtor */
            virtual ~SC_Log4cplusWrapper();
    
        private:
            /** def ctor .. hidden */
            SC_Log4cplusWrapper();
            SC_Log4cplusWrapper( const SC_Log4cplusWrapper &aRhs );
            SC_Log4cplusWrapper& operator=( const SC_Log4cplusWrapper &aRhs );
    
        public:
            /** enums to identify the level of logging */
            enum ELogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };
            /** main logging mechanism */
            void log( ELogLevel aLevel, const std::string &aMessage );
            
        private:
            /** underlying logger */
            log4cplus::Logger * mLogger;
            /** Internal mutex */
            mutable SC_BasicMutex mLock;
            
    };

} // namespace

#endif
