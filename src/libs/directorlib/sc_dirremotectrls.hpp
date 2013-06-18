#ifndef __sc_dirremdummyotectrls_hpp
#define __sc_dirremotectrls_hpp

#include <base/sa_baseobject.hpp>
#include <base/st_spointer.hpp>
#include <tools/sc_recursivemutex.hpp>

#include <map>

namespace rpms
{

    // pre decl
    class SC_DaemonCtrl;
    class SC_XMLNodeConfig;
    
    /** 
     * Main Controller of the daemons.
     * A member of the manager class that is responsible for remote daemon management.
     *
     * @author Peter Suggitt (2005)
     * @note Concrete class
     * */
    class SC_DirRemoteCtrls : public SA_BaseObject
    {
        public:
            /** Default constructor */
            SC_DirRemoteCtrls();
            /** Virtual destructor */
            virtual ~SC_DirRemoteCtrls();
            /** Add a daemon to the registered daemon container */
            bool addDaemon( const std::string &aHostName, const bool aIsAppRunning );
            /** Determines whether a daemon exists for a given hostname */
            bool daemonExists( const std::string &aHostName );
            /** Stops all the daemon controller threads.  This method does not 
             *      stop the system, just the running threads for the director. */
            void stopAll();
            /** Returns the collective status of the daemons */
            std::string getDaemonStatus();
            /** Returns the collective process stats from the daemons */
            std::string getProcessStats();
            /** Change the running state of the system as a whole */
            bool startStopSystemComponents( const bool aIsToRun );
            /** Initialise the remote controller */
            void initialise();
            /** Getter method for the mSystemRunning member */
            bool isSystemRunning() const {return mSystemRunning;}
            /** Reconfigure the system after re-load balancing */
            bool reconfigureSystem( const bool aForce );
            /** Generic method to restart applications */
            bool restartSystemComponent( const std::string &aCompName, const bool aForceIt );
            /** Method used by the Director thread to check that all the Daemons are running correctly */
            void checkDaemons();
            
        protected:
    
        private:
            /**  */
            SC_DirRemoteCtrls( const SC_DirRemoteCtrls& aRhs );
            /**  */
            SC_DirRemoteCtrls& operator=( const SC_DirRemoteCtrls& aRhs );
            /** iterates through the map looking for any DaemonCtrl Threads that are 
             * inactive and removes them */
            void cleanDeadDaemons( const bool aRunDistribution = false );
            /** Start the applications after working out what they should run */
            bool startSystemComponents();
            /** Simply stops the application across all the daemons */
            bool stopSystemComponents();
    
        private: // members
            /**  */
            typedef std::map< std::string, ST_SPointer<SC_DaemonCtrl> > ctrlMap;
            /**  */
            typedef std::pair< std::string, ST_SPointer<SC_DaemonCtrl> > ctrlPair;
            /**  */
            ctrlMap * mDaemonCtrls;
            /** mutex to lock for thread safety */
            mutable SC_RecursiveMutex mLock;
            /**  */
            ST_SPointer<SC_XMLNodeConfig> mNodes;
            /** Flag showing whether the system is actually running */
            bool mSystemRunning;
            /** This flag lets us know if the client has issued any starts or stops */
            bool mClientInteraction;
    };

} // namespace


#endif
