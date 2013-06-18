#ifndef __sc_directorthread_hpp
#define __sc_directorthread_hpp

#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>
#include <idllib/idls/sr_directortask.h>
#include <tools/sc_recursivemutex.hpp>

namespace rpms
{

    // pre decl
    class SC_DirRemoteCtrls;
    class SC_RecursiveMutex;

    /**
     * Main director thread class.
     * This class is to be used by a daemon application to start a director thread, this 
     *      is where all the controlling of the system happens.
     * One way to look at this class is to think of it as being one great big communication 
     *      wrapper class that delegates all the real functionality to the SC_DirRemoteCtrls 
     *      class
     *
     * @author Peter Suggitt (2005)
     * @note Concrete class
     * */
    class SC_DirectorThread : public SA_ThreadBase, public POA_SR_DirectorTask
    {
        public:
            /** 
             * This is the entry point for creating the class.  This has been hidden away 
             *      so that No one actually creates any threads without some sort 
             *      of configurable checking  */
            static ST_SPointer<SC_DirectorThread> create();
            /** Virtual destructor  */
            virtual ~SC_DirectorThread();
            /** Stop the Thread ruinning */
            void stop();
            /** Reset the thread to where it was before executed */
            void reset();
            /** Getter to test whether the director is ready to use */
            bool isInitialised() {return mInitialised;}
            
        public: // Derived from POA object
            /** Start the system.  Command borne from the client application. */
            CORBA::Boolean clientStart();
            /** Stop the systemn.  Command borne from the client application. */
            CORBA::Boolean clientStop();
            /** Reconfigure the system.  Command borne from the client application. */
            CORBA::Boolean clientReconfigure( const char * aForce );
            /** Get the system status in XML.  Command borne from the client application. */
            char * clientGetStatus();
            /** Restart any given system components that have failed and exceeded 
             *      their retry limit.  Command borne from the client application. */
            CORBA::Boolean clientRestartFailedApp( const char * aName );
            /** Restart any given system components (bounce them!).  Command borne from the client 
             *      application. */
            CORBA::Boolean clientRestartApp( const char * aName );
            /** Stop the director service. Command borne from the client application. */
            void clientStopDirector();
            /** Register a new Daemon with the Director Service.  Command borne from the daemon 
             *      application. */
            CORBA::Boolean registerDaemon( const char * aHostName, const unsigned long aInstanceId, CORBA::Boolean aIsAppRunning );
            /** Methid to collect all of the process statistics from the daemons */
            char * distGetStats();
            
            
        protected:
            /** Protected constructor */
            SC_DirectorThread( const std::string &aName );
            /** Start the thread running */
            void run();
    
        private:
            /** Initialisation method.  No exceptions in the ctor pls! */
            void init();
            /** Hidden copy constructor so we don't ever have two threads running 
             *      as a result of a copy */
            SC_DirectorThread( const SC_DirectorThread &aRhs ); //  hidden
            /** Assignment hidden for same reaspn as copy constructor */
            SC_DirectorThread& operator=( const SC_DirectorThread &aRhs ); // hidden
    
        private: // members
            /** Flag to show whether the thread should be ticking over */
            bool mShouldRun;
            /** Flag to show that the thread is initialised */
            bool mInitialised;
            /** Instance ID of the system */
            unsigned long mInstanceId;
            /** Mutex Lock object .. to be used with a ST_Guard object */
            mutable SC_RecursiveMutex mLock;
            /** Collection of controllers to the the daemons.  All the functionality lives in 
             *      this class */
            ST_SPointer<SC_DirRemoteCtrls> mControllers;
    };

} // namespace

#endif
