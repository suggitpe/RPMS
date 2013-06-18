//#############################
#include "sc_basicmutex.hpp"
#include "sx_mutex.hpp"

using rpms::SC_BasicMutex;

///////////////////////////////////////////
SC_BasicMutex::SC_BasicMutex( const std::string &aName )
    : mName(aName)
{
    ::pthread_mutex_init( &mLock, 0 );
}

///////////////////////////////////////////
SC_BasicMutex::~SC_BasicMutex()
{
    ::pthread_mutex_destroy( &mLock );
}


///////////////////////////////////////////
void SC_BasicMutex::lock()
{
    if( 0 != ::pthread_mutex_lock( &mLock ))
    {
        throw SX_Mutex("Failed to lock basic mutex [" + mName + "]");
    }
}

///////////////////////////////////////////
void SC_BasicMutex::unlock()
{
    if( 0 != ::pthread_mutex_unlock( &mLock ) )
    {
        throw SX_Mutex("Failed to unlock basic mutex [" + mName + "]");
    }
}


///////////////////////////////////////////
