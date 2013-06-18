#ifndef __sc_heartbeatthread_hpp
#define __sc_heartbeatthread_hpp

#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>

namespace rpms
{
    // pre decl
    class SA_Socket;

    /** 
     * Heartbeater thread just replies to any incoming heartbeats
     *
     * @author Peter Suggitt (2005)
     * @note <b>Concrete Daemon class</b> 
     * */
    class SC_HeartbeatThread : public SA_ThreadBase
    {
        public:
            /** Static methid to create the heartbeat thread (this is the only access to a new one) */
            static ST_SPointer<SC_HeartbeatThread> create();
            /** This is used to stop the thread when it is running */
            virtual void stop();
            /** virtual dtor */
            virtual ~SC_HeartbeatThread();
            /** Reset the thread and all the object contents ready for it to be restarted */
            void reset();

        protected:
            /** Main ctor
             *      @param aName The name of the thread*/
            SC_HeartbeatThread( const std::string &aName );
            /** The main runnable method for the thread */
            virtual void run();

        private:
            /** Hidden copy ctor */
            SC_HeartbeatThread( const SC_HeartbeatThread &aRhs );
            /** Hidden assignment operator */
            SC_HeartbeatThread& operator=( const SC_HeartbeatThread &aRhs );
            /** Initialisation method */
            void init();

        private:// members
            /** Flag denoting whether the thread should run */
            bool mShouldRun;
            /** The UDP port to listen for incoming heartbeats */
            int mHeartbeatPort;
            /** The underlying socket object */
            ST_SPointer<SA_Socket> mSocket;
            /** Maximum socket stimeouts allowed before we presume the director is not running */
            int mMaxFailedAttempts;
    };

} // namespace

#endif
