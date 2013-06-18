#ifndef __sc_processmanager_hpp
#define __sc_processmanager_hpp

#include <base/sa_baseobject.hpp>
#include <base/st_spointer.hpp>
#include <tools/sc_recursivemutex.hpp>

#include <map>

namespace rpms
{

    class SC_ProcessWrapperThread;
    class SC_DaemonStatus;
    class SC_ProcStats;

    /** Manages processes */
    class SC_ProcessManager : public SA_BaseObject
    {
        public:
            /** Constructor */
            SC_ProcessManager();
            /** Main destructor */
            virtual ~SC_ProcessManager();
            /** Takes in an xml config and will start processes based on this data 
             *      @param aProcessConfig the xml with which we can start the processes */
            void startProcesses( const std::string &aProcessConfig );
            /** Stops all processes that it is running */
            void stopAllProcesses();
            /** Add the component statuses from the process wrappers
             *      @param aStatus the status object with wwhich we can add the process data to */
            void addCompStatuses( const ST_SPointer<SC_DaemonStatus> &aStatus );
            /** This will restart a process with a given name.  Optional argument is 
             *  there to differentiate whether to do a soft retart (only if it is 
             *  not currently runnig) or a forced restart (restart even if it is running) 
             *      @param aProcessName name of the process to restart (can also pass in 'all') 
             *      @param aHardRestart boolean value to denote if we should only restart if it has failed */
            void restartProcess( const std::string &aProcessName, const bool aHardRestart = false );
            /** This method is used to manage the reconfiguration of the system from the daemon side,
             *  a new config XML is passed in and based on that info this will start or stop processes
             *  as necessary
             *      @param aProcessConfig the xml configuration that we want to change to */
            void adaptToReconfiguration ( const std::string &aProcessConfig );
            /** Add component statistic from the process wrappers (similar concept to addCompStatuses)
             *      @param aStats statistic object that we need to add to */
            void addProcessStatistics( const ST_SPointer<SC_ProcStats> &aStats );

        protected:
        private:
            /** Hidden copy ctor */
            SC_ProcessManager( const SC_ProcessManager &aRhs );
            /** Hidden ass operator */
            SC_ProcessManager& operator=( const SC_ProcessManager &aRhs );

        private: // members
            /** typedef to reduce the amount of typeing */
            typedef std::map<std::string, ST_SPointer<SC_ProcessWrapperThread> > procMap;
            /** typedef to reduce the amount of typeing */
            typedef std::pair<std::string, ST_SPointer<SC_ProcessWrapperThread> > procPair;
            /** This is a map of the underlying process threads */
            procMap * mProcs;
            /** This is a simple lock sop that we do not have any more than one thread
             *  working on this object at anyone time */
            mutable SC_RecursiveMutex mLock;
            
    };

}// namepsace

#endif
