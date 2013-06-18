//#############################
#include "sc_testclass.hpp"
#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>

using rpms::SC_TestClass;

// std namespace

///////////////////////////////////////////
// friend impl for ostream
std::ostream& operator<<( std::ostream &os, SC_TestClass &aIn )
{
    os << "mTestInt[" << aIn.getInt() << "], mTestDbl[" << aIn.getDouble() << "]";
    return os;
}

///////////////////////////////////////////
SC_TestClass::SC_TestClass() :
    mTestInt(0), mTestDbl(0.0)
{
}

///////////////////////////////////////////
SC_TestClass::SC_TestClass( const unsigned int aInt, const double aDbl):
    mTestInt(aInt), mTestDbl(aDbl)
{
}

///////////////////////////////////////////
SC_TestClass::SC_TestClass( const SC_TestClass &aRhs ) :
    mTestInt(aRhs.mTestInt), mTestDbl(aRhs.mTestDbl)
{
}

///////////////////////////////////////////
SC_TestClass::~SC_TestClass()
{
    RPMS_DEBUG( "sc_testclass", "SC_TestClass dtor" );
}

///////////////////////////////////////////
SC_TestClass & SC_TestClass::operator=( const SC_TestClass &aRhs )
{
    if( this != &aRhs)
    {
        mTestInt = aRhs.mTestInt;
        mTestDbl = aRhs.mTestDbl;
    }
    return *this;
}

///////////////////////////////////////////
std::string SC_TestClass::debug() const
{
    return "mTestInt[" + rpms::convert<std::string>(mTestInt) + "], mTestDbl[" + rpms::convert<std::string>(mTestDbl) + "]";
}
