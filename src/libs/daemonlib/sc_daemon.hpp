#ifndef __sc_daemon_hpp
#define __sc_daemon_hpp

#include <base/sa_application.hpp>
#include <base/st_spointer.hpp>
#include <idllib/idls/sr_daemontask.h>
#include <tools/sc_recursivemutex.hpp>

namespace rpms
{

    // pre decl
    class SC_HeartbeatThread;
    class SC_DirectorThread;
    class SC_ProcessManager;
    class SC_StateQueue;

    /** 
     * The main daemon class running on each node in the system.
     *
     * @author Peter Suggitt (2005)
     * @note <b>Concrete Daemon class</b> 
     * */
    class SC_Daemon : public SA_Application, public POA_SR_DaemonTask
    {
        public:
            /** standard ctor */
            SC_Daemon();
            /** dtor */
            virtual ~SC_Daemon();
            /** main run method .. will continue to run while mRun is true */
            void run();
            /** shutdown method so we can actually stop the app gracefully */
            virtual void shutdown();
    
        public:// remote methods
            /** Called by the director to get the satatus of the daemon
             *      @return the daemon status in xml format */
            char * getDaemonStatus();
            /** Called through the director to get a summary of the process statistics */
            char * getProcessStats();
            /** Called by the director to insert a new state into the SC_StateQueue object 
             *      @param aNewState the new state to move to
             *      @param aArg any arguments needed (eg some xml) 
             *      @return the process statistics in xml format */
            CORBA::Boolean changeState( const char* aNewState, const char* aArg );
            /** This method is used by the elections to force a daemons to 
             *  abandon its currently elected leader and regeister with 
             *  the leader running on the defined host
             *      @param aHost the new host to register with
             *      @return the success inserting the state into the queue */
            void registerWithNewDirector( const char * aHost );
            /** This is very similar to registerWithNewDirector except that it 
             *  spawns an election process to work out the new leader */
            void deRegisterCurrentDirector();

        protected:

        private:
            /** Hidden copy ctor */
            SC_Daemon( const SC_Daemon &aRhs );
            /** Hidden assignment operator */
            SC_Daemon& operator=( const SC_Daemon &aRhs );
            /** initialisation method so we do not do some things in the ctor */
            void init();
            /** registers itself with a director process */
            bool registerDaemon();
            /** based on the info that the state has changed, we need to check what needs to be 
             *  done with it.  This method will analyse the state of the system and check 
             *  what needs to be done as a result 
             *      @return whether the registration was successfull */
            void checkNewState();

        private: //members
            /** This is the class that manages the running of many different process threads */
            ST_SPointer<SC_ProcessManager> mProcMgr;
            /** This object is the heartbeat receiver thread .. this will simply 
             *  return the received heartbeat to the sender */
            ST_SPointer<SC_HeartbeatThread> mHeartbeatThread;
            /** Director will run as a thread inside one of the daemons */
            ST_SPointer<SC_DirectorThread> mDirectorThread;
            /** flag to state whether this is a director host */
            bool mDirectorHost;
            /** This is a run flag so we can easily poll when the process should run */
            bool mRun; 
            /** System Instance ID */
            unsigned long mInstanceId;
            /** as the process will not move across hosts this can be held in the class */
            std::string mLocalHost;
            /** State changing lock */
            mutable SC_RecursiveMutex mStateLock;
            /** Collection class purely to manage state transitions.  This will also make 
             *  sure that no state changes are lost */
            ST_SPointer<SC_StateQueue> mStateQueue;
            /** Flag stating whether the system is running or not */
            bool mAppRunning;
    };

} // namespace

#endif
