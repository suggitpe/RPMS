//#############################
#include "sc_statequeue.hpp"

#include <si_macros.hpp>

using rpms::SC_StateQueue;
using rpms::SC_StateQueueObject;

///////////////////////////////////////////
SC_StateQueue::SC_StateQueue()
{
    TRY(SC_StateQueue::SC_StateQueue());
        mQueue = new std::queue<SC_StateQueueObject>;
    CATCH;
}

///////////////////////////////////////////
SC_StateQueue::~SC_StateQueue()
{
    TRY(SC_StateQueue::~SC_StateQueue());
        delete mQueue;
    CATCH;
}

///////////////////////////////////////////
void SC_StateQueue::push( const SC_StateQueueObject::EState aState, const std::string &aArg )
{
    TRY(SC_StateQueue::push( const SC_StateQueueObject::EState aState, const std::string &aArg ));
        mQueue->push( SC_StateQueueObject(aState, aArg) );
    CATCH;
}

///////////////////////////////////////////
SC_StateQueueObject SC_StateQueue::front() const
{
    TRY(SC_StateQueue::front());
        return mQueue->front();
    CATCH;
}

///////////////////////////////////////////
void SC_StateQueue::pop()
{
    TRY(SC_StateQueueObject::pop());
        if( mQueue != 0 )
        {
            mQueue->pop();
        }
    CATCH;
}

///////////////////////////////////////////
size_t SC_StateQueue::size() const
{
    if( mQueue != 0 )
    {
        return mQueue->size();
    }
    return 0;
}

///////////////////////////////////////////
bool SC_StateQueue::empty() const
{
    if( mQueue != 0)
    {
        return mQueue->empty();
    }
    return true;
}


//////////////////////////////////////////////////////
// ################################################ //
// ################################################ //
//////////////////////////////////////////////////////

///////////////////////////////////////////
SC_StateQueueObject::SC_StateQueueObject( const EState aState, const std::string &aArg )
    : mState(aState), mArg(aArg)
{
    TRY(SC_StateQueueObject::SC_StateQueueObject( const EState aState, const std::string &aArg ));
    CATCH;
}

///////////////////////////////////////////
SC_StateQueueObject::~SC_StateQueueObject()
{
    TRY(SC_StateQueueObject::~SC_StateQueueObject());
    CATCH;
}

///////////////////////////////////////////
SC_StateQueueObject::SC_StateQueueObject(const SC_StateQueueObject &aRhs )
{
    TRY(SC_StateQueueObject::SC_StateQueueObject(const SC_StateQueueObject &aRhs ));
        // copy members
        mState = aRhs.mState;
        mArg = aRhs.mArg;
    CATCH;
}

///////////////////////////////////////////
SC_StateQueueObject& SC_StateQueueObject::operator=(const SC_StateQueueObject &aRhs )
{
    TRY(SC_StateQueueObject& SC_StateQueueObject::operator=(const SC_StateQueueObject &aRhs ));
        if( this != &aRhs )
        {
            // copy members
            mState = aRhs.mState;
            mArg = aRhs.mArg;
        }
        return *this;
    CATCH;
}

///////////////////////////////////////////
