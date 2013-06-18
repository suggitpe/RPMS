//#############################
#include "sa_baseobject.hpp"
#include <si_macros.hpp>

using rpms::SA_BaseObject;

///////////////////////////////////////////
SA_BaseObject::SA_BaseObject() 
    : mTypeFlag(0), mRefCount(0)
{
}

///////////////////////////////////////////
SA_BaseObject::SA_BaseObject( const SA_BaseObject &aRhs ) 
    : mTypeFlag(aRhs.mTypeFlag), mRefCount(0)
{
}

///////////////////////////////////////////
SA_BaseObject::~SA_BaseObject()
{
}

///////////////////////////////////////////
SA_BaseObject & SA_BaseObject::operator=(const SA_BaseObject& aRhs)
{
    if( this != &aRhs)
    {
        mTypeFlag = aRhs.mTypeFlag;
    }
    return *this;
}

///////////////////////////////////////////
