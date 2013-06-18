//#############################
#include "sx_exception.hpp"

using rpms::SX_Exception;

///////////////////////////////////////////
SX_Exception::SX_Exception( const std::string &aReason ) 
    : mReason(aReason), mStack( new std::stack<std::string>)
{
}

///////////////////////////////////////////
SX_Exception::~SX_Exception()
{
    delete mStack;
}

///////////////////////////////////////////
SX_Exception::SX_Exception( const SX_Exception &aRhs )
{
    mReason = aRhs.mReason;
    mStack = new std::stack<std::string>(*aRhs.mStack);
}

///////////////////////////////////////////
SX_Exception& SX_Exception::operator=( const SX_Exception &aRhs )
{
    if( this != &aRhs)
    {
        mReason = aRhs.mReason;
        mStack = new std::stack<std::string>(*aRhs.mStack);
    }
    return *this;
}

///////////////////////////////////////////
const std::string SX_Exception::reason() const
{
    return "\n***********************************\nException caught of type " 
        + type() + ":\n\t" + mReason
        + "\n***********************************\n"
        + stack() 
        + "***********************************\n";
}

///////////////////////////////////////////
const std::string SX_Exception::type() const
{
    return "SX_Exception";
}

///////////////////////////////////////////
const std::string SX_Exception::stack() const
{
    std::string ret = "Stack trace:\n";
    while( !mStack->empty() )
    {
        ret += "\t" + mStack->top() + "\n";
        mStack->pop();
    }
    return ret;
}
///////////////////////////////////////////
void SX_Exception::push( const std::string &aMeth)
{
    mStack->push(aMeth);
}
