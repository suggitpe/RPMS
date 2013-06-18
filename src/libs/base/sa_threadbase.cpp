// #####################################
#include "sa_threadbase.hpp"
#include "si_baselogging.hpp"
#include "sx_base.hpp"

#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>

using rpms::SA_ThreadBase;
using rpms::convert;

///////////////////////////////////////////
SA_ThreadBase::SA_ThreadBase( const std::string &aThreadName )
    : mRunning(false), mThread(0), mThreadId(0), mThreadName(aThreadName), 
        mSpent(false), mLock("Thread lock"), mStopDead(false)
{
    TRY( SA_ThreadBase::SA_ThreadBase( const std::string &aThreadName ) );
    CATCH;
}

///////////////////////////////////////////
SA_ThreadBase::~SA_ThreadBase()
{
    TRY(SA_ThreadBase::~SA_ThreadBase());
        if(mThread)
        {
            ::pthread_detach(*mThread);
            delete mThread;
        }
    CATCH;
}

///////////////////////////////////////////
SA_ThreadBase::SA_ThreadBase(const SA_ThreadBase &aRhs)
    : mLock("dummy lock")
{
    TRY(SA_ThreadBase::SA_ThreadBase(const SA_ThreadBase &aRhs));
        throw SX_Base("no copy ctor allowed for thread");
    CATCH;
}

///////////////////////////////////////////
SA_ThreadBase& SA_ThreadBase::operator=( const SA_ThreadBase & aRhs )
{
    TRY(SA_ThreadBase::operator=( const SA_ThreadBase & aRhs ));
        throw SX_Base("no assignment operator allowed for threads");
        return *this;
    CATCH;
}

///////////////////////////////////////////
void SA_ThreadBase::start()
{
    TRY(SA_ThreadBase::start());
        if( mRunning || mSpent )
        {
            if( mRunning )
            {
                RPMS_TRACE( BASE_THR_LOG, "Thread [" + mThreadName + "] is currently running" );
            }
            else if(mSpent)
            {
                RPMS_TRACE( BASE_THR_LOG, "Thread [" + mThreadName + "] has already been used" );
            }
            else
            {
                // should never happen .... ever .. unless someone changes the if statement
                RPMS_TRACE( BASE_THR_LOG, "Thread [" + mThreadName + "] is in an unknown state" );
            }
            return;
        }

        mThread = new pthread_t;
        // set the state first else a context switch may end up setting the state to false before we can start it
        mLock.lock();
        // create the thread
        int th_ret = ::pthread_create( mThread, NULL, SA_ThreadBase::entry, this );
        if( 0 != th_ret )
        {
            if( th_ret == EAGAIN )
            {
                throw SX_Base("Failed to create thread due to lack of system resources");
            }
            throw SX_Base("Failed to create thread (unknown reason)");
        }
        mRunning = true;
        mLock.unlock();
        RPMS_DEBUG( BASE_THR_LOG, "Started thread [" + mThreadName 
            + "] with id [" + convert<std::string>(mThreadId) + "]" );
    CATCH;
}

///////////////////////////////////////////
void * SA_ThreadBase::entry( void * aThread )
{
    TRY(SA_ThreadBase::entry( void * aThread ));
        ((SA_ThreadBase*)aThread)->mThreadId = pthread_self();
        try
        {
            ((SA_ThreadBase*)aThread)->run();
        }
        catch( SX_Exception &ex )
        {
            RPMS_ERROR(BASE_THR_LOG, "Thread [" + ((SA_ThreadBase*)aThread)->mThreadName + "] ended with exception:\n" + ex.reason());
        }
        catch(...)
        {
            RPMS_ERROR(BASE_THR_LOG, "Thread [" + ((SA_ThreadBase*)aThread)->mThreadName + "] ended with an unknown exception type");
        }
        ((SA_ThreadBase*)aThread)->terminate();
        return NULL;
    CATCH;
}

///////////////////////////////////////////
void SA_ThreadBase::reset()
{
    TRY(SA_ThreadBase::reset());
        RPMS_DEBUG( BASE_THR_LOG, "Resetting thread base");
        mLock.lock();
        mSpent = false;
        if( mThread )
        {
            ::pthread_detach(*mThread);
            delete mThread;
            mThread = 0;
        }
        mLock.unlock();
    CATCH;
}

///////////////////////////////////////////
void SA_ThreadBase::terminate()
{
    TRY(SA_ThreadBase::terminate());
        if( !mStopDead )
        {
            //RPMS_TRACE( BASE_THR_LOG, "Thread [" + mThreadName + "] with id [" 
            //    + convert<std::string>(mThreadId) + "] has now terminated");
            mLock.lock();
            mRunning = false;
            mSpent = true;
            mThreadId = 0;
            mLock.unlock();
        }
    CATCH;
}

///////////////////////////////////////////
void SA_ThreadBase::join()
{
    TRY(SA_ThreadBase::join());
        if( ! mRunning )
        {
            RPMS_DEBUG( BASE_THR_LOG, "Thread [" + mThreadName + "] with id [" 
                + convert<std::string>(mThreadId) + "] not running for a join call" );
        }
        else
        {
            RPMS_DEBUG( BASE_THR_LOG, "Joining thread with ID [" + convert<std::string>(mThreadId) + "]" );
            ::pthread_join(mThreadId, 0);
        }
    CATCH;
}

///////////////////////////////////////////
void SA_ThreadBase::stopDead()
{
    TRY(SA_ThreadBase::stopDead());
        RPMS_INFO( BASE_THR_LOG, "Killing thread [" + mThreadName + "] with id of [" 
            + convert<std::string>(mThreadId) + "]" );
        mLock.lock();
        mStopDead = true;
        ::pthread_kill( mThreadId, 0 );
        mRunning = false;
        mLock.unlock();
    CATCH;
}

///////////////////////////////////////////
