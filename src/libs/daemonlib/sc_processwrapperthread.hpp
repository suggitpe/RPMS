#ifndef __sc_processwrapperthread_hpp
#define __sc_processwrapperthread_hpp

#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>
#include <tools/sc_recursivemutex.hpp>

#include <deque>

namespace rpms
{

    class SC_ProcessConfig;
    class SA_StreamReader;
    class SC_ProcessInfo;
    class SC_ComponentStatus;
    class SC_CompStat;
    
    /** 
     * This is a wrapper class for the management of a single process .
     * This uses a SC_ProcessConfig for managing the data that it needs to run.
     *
     * @author Peter Suggitt (2005)
     * @note Thread class
     * */
    class SC_ProcessWrapperThread : public SA_ThreadBase
    {
        public:
            /** This is the entry point for the thread .. there should be no other way 
             *  to create a thread of thsi type 
             *      @param aThreadName the name of the thread 
             *      @param aProcConfig the configuration for the process*/
            static ST_SPointer<SC_ProcessWrapperThread> create( const std::string &aThreadName, 
                    const SC_ProcessConfig &aProcConfig );
            /** Virtual destructor */
            virtual ~SC_ProcessWrapperThread();
            /** Stop the thread and issue a SIGTERM to the underlying process */
            void stop();

        public: // accessors
            /** Accessor method to the underlying process data */
            ST_SPointer<SC_ProcessConfig> getProcData() const;
            /** accessor to the start time */
            std::string getProcTimeStamp() const { return mProcTimeStamp; }
            /** accessor to the count of retries */
            int getRetryCount() const { return mRetryTimes->size(); }
            /** Accessor to the underlying pid */
            int getPid() const { return mChildPID; }
            /** Accessor to the reconfiguration check flag */
            bool hasBeenReconfChecked() const { return mReconfChecked;}
            /** Accessor to the reconfiguration check flag */
            void setReconfChecked() { mReconfChecked = true;}
            /** Accessor to the reconfiguration check flag */
            void setReconfUnChecked() { mReconfChecked = false;}
            /** Build a component status object of the current process */
            ST_SPointer<SC_ComponentStatus> getCompStatus() const;
            /** Build a process statistics object for the current process */
            ST_SPointer<SC_CompStat> getProcStats() const;

        protected:
            /** This is where it all happens */
            void run();
        
        private:
            /** Hidden constructor .. this ensures no one else can create an object of this 
             *      type except through the static create method */
            SC_ProcessWrapperThread( const std::string &aThreadName, const SC_ProcessConfig &aProcConfig );
            /** Hidden copy ctor */
            SC_ProcessWrapperThread( const SC_ProcessWrapperThread &aRhs );
            /** Hidden assignment operator */
            SC_ProcessWrapperThread& operator=( const SC_ProcessWrapperThread &aRhs );
            /** Initialisation method */
            void initialise();
            /** Method to run a process and keep restarting it */
            void runProcessContinuously();
            /** Method to run a process once only */
            void runProcessOnce();
            /** Checks if the process has failed too many times */
            bool checkFailureRate();
            /** This method will set up some of the environment variables that are needed */
            void getEnvironmentVarArray();
            /** fork the actual process  */
            int forkProcess();
            /** exec the actual process  */
            int execProcess( const std::string &aCmdLine );
            /** Read process pipes */
            void readProcPipes( const std::string &aLogName );
            /** This method is used to evaluate the return value from an executed process */
            bool evaluateProcessReturn( const int aRetVal );

        private: // members
            /** The underlying process details */
            ST_SPointer<SC_ProcessConfig> mProcData;
            /** Name of the directory for all logging */
            std::string mLoggingDir;
            /** mutex to lock for thread safety */
            mutable SC_RecursiveMutex mLock;
            /** General state flag for the process wrapper to run the process or not */
            bool mRunProcess;
            /** General flag to denote if the process has failed too many times */
            bool mFailed;
            /** Deque of times that the process has been started */
            std::deque<long> *mRetryTimes;
            /** Timestamp for display purposes */
            std::string mProcTimeStamp;
            /** PID of the underlying process */
            pid_t mChildPID;
            /** stdin pipe */
            int mStdIn[2];
            /** stdout pipe */
            int mStdOut[2];
            /** stderr pipe */
            int mStdErr[2];
            /** Flag for determining if the process has been killed externally */
            bool mExternalStop;
            /** Reader thread for stdout */
            ST_SPointer<SA_StreamReader> mStdOutReader; // thread
            /** Reader thread for stderr */
            ST_SPointer<SA_StreamReader> mStdErrReader; // thread
            /** This is a simple flaf to allow us to check if the thread should 
             *      be running during a reconfiguration exercise */
            bool mReconfChecked;
            /** Process statistics object */
            ST_SPointer<SC_ProcessInfo> mProcInfo; // thread
        
    };

} // namespace

#endif
