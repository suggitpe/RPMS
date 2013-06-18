#ifndef __sa_threadbase_hpp
#define __sa_threadbase_hpp

#include <base/sa_baseobject.hpp>
#include <tools/sc_basicmutex.hpp>

namespace rpms
{

    /**
     * Base class for all threads.
     * All thread classes should derive from this one so that they 
     *      have all the base functionality they need.
     *
     * @author Peter Suggitt (2005)
     * @note Abstract class
     * */
    class SA_ThreadBase : public SA_BaseObject
    {
        public:
            /** virtual dtor */
            virtual ~SA_ThreadBase();
            /** This should be the only interface to actually start a thread */
            void start();
            /** Simple tester to see if the underlying thread is running */
            bool isRunning() const {return mRunning;}
            /** allows you to join to the underlying thread */
            void join();

        protected:
            /** String ctor
             * @param aThreadName string name of the thread */
            SA_ThreadBase( const std::string &aThreadName );
            /** This should be implemented by the derived class.  This is called by 
             * the start method in the base (here) class */
            virtual void run() = 0;
            /** This should be implemented by the derived class.  This should
             * be used as the default way to stop the thread */
            virtual void stop() = 0;
            /** simple getter to the thread class */
            virtual int getThreadId() {return mThreadId;}
            /** Accessor to set the name of the thread 
             * @param aName The name to set the thread name with */
            void setThreadName( const std::string &aName) { mThreadName = aName;}
            /** This is the non-default manner in which the thread can be stopped.  This 
             * is not the nicest way to stop a thread as there is limited scope for cleanup */
            virtual void stopDead();
            /** Must be able to reset the thread ... to save having to create a new thread 
             * object every time */
            virtual void reset();

        // members
        private:
            /** Hidden copy ctor */
            SA_ThreadBase( const SA_ThreadBase &aRhs );
            /** Hidden assignment operator */
            SA_ThreadBase& operator=( const SA_ThreadBase & aRhs );
            /** Static member to be used as the entry point for all threads 
             * @param aThread The thread that you want o execute (calls run method on the thread) */
            static void * entry( void * aThread );
            /** called internally to free up the threadresources */
            void terminate();

        private: // members
            /** flag to denote if the thread is currently running */
            bool        mRunning;
            /** Flag to denote if the thread has previously run */
            bool        mSpent;
            /** The underlying thread */
            pthread_t * mThread;
            /** The ID of the underlying thread */
            int         mThreadId;
            /** The name of the underlying thread .. used for debug */
            std::string mThreadName;
            /** Internal mutex */
            mutable SC_BasicMutex mLock;
            /** Flag for stopdead */
            bool        mStopDead;
    };

} // namespace

#endif
