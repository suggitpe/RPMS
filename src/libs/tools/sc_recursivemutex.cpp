//#############################
#include "sc_recursivemutex.hpp"
#include "sx_mutex.hpp"

using rpms::SC_RecursiveMutex;

///////////////////////////////////////////
SC_RecursiveMutex::SC_RecursiveMutex( const std::string &aName )
    : mCounter(0), mThreadId(0), mName(aName)
{
    // create mutex with default attributes
    // also using PTHREAD_MUTEX_RECURSIVE_NP
    ::pthread_mutexattr_t attr;
    ::pthread_mutexattr_init( &attr );
    ::pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );
    ::pthread_mutex_init( &mLock, &attr );
    ::pthread_mutexattr_destroy( &attr );
}

///////////////////////////////////////////
SC_RecursiveMutex::~SC_RecursiveMutex()
{
    ::pthread_mutex_destroy( &mLock );
}

///////////////////////////////////////////
void SC_RecursiveMutex::lock()
{
    if( mThreadId != pthread_self() || mCounter == 0  )
    {
        if( 0 != ::pthread_mutex_lock( &mLock ) )
        {
            throw SX_Mutex("Failed to lock recursive mutex [" + mName + "]");
        }
        mThreadId = pthread_self();
    }
    ++mCounter;
}

///////////////////////////////////////////
void SC_RecursiveMutex::unlock()
{
    if( --mCounter == 0 )
    {
        mThreadId = 0;
        if( 0 != ::pthread_mutex_unlock( &mLock ) )
        {
            throw SX_Mutex("Failed to unlock recursive mutex [" + mName + "]");
        }
    }
}

///////////////////////////////////////////
