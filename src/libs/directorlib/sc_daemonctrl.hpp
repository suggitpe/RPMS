#ifndef __sc_daemonctrl_hpp
#define __sc_daemonctrl_hpp

#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>
#include <idllib/idls/sr_daemontask.h>

namespace rpms
{

    // pre decl
    class SA_Socket;
    
    /** 
     * Class used to control a single daemon.
     * This class is the delegate for managing a single daemon.
     *
     * @author Peter Suggitt (2005)
     * @note Concrete class
     * */
    class SC_DaemonCtrl : public SA_ThreadBase
    {
        public:
            /** Create a new object of type SC_DaemonCtrl */
            static ST_SPointer<SC_DaemonCtrl> create( const std::string &aDaemonHost, 
                    const std::string &aThreadName );
            /** Stop the thread */
            void stop();
            /** Virtual destructor */
            virtual ~SC_DaemonCtrl();
            /** Reset the thread */
            void reset();
            /** Get the remote daemon status*/
            std::string getStatus();
            /** Get the remote daemon process stats */
            std::string getDaemonProcStats();
            /** Getter method for the initialised flag */
            bool isInitialised() const { return mInitialised; }
            /** Go to the underlying daemon and add to the state stack */
            void addToDaemonStateQueue( const std::string &aState, const std::string &aArg );
            
        protected:
            /** Main constructor. Called from the create method */
            SC_DaemonCtrl( const std::string &aDaemonHost, const std::string &aThreadName );
            /** The main runnable context for the thread */
            void run();
            
        private:
            /** Hidden copy constructor */
            SC_DaemonCtrl( const SC_DaemonCtrl &aRhs );
            /** Hidden assignment operator */
            SC_DaemonCtrl& operator=( const SC_DaemonCtrl &aRhs );
            /** Internal initialisation */
            void init();
    
        private: // members
            /** Flag to see whether this thread should run */
            bool mShouldRun;
            /** Port number for the heartbeats on the daemon host */
            int mHeartbeatPort;
            /** Socket for sending out heartbeats to the daemons */
            ST_SPointer<SA_Socket> mSocket;
            /** Name of the remote daemon host */
            std::string mDaemonHost;
            /** Remote object pointer (CORBA object) */
            SR_DaemonTask_var mRemote;
            /** flag to show if initialised */
            bool mInitialised;
    };

} // namespace

#endif
