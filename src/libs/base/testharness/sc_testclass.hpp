#ifndef __sc_testclass_hpp
#define __sc_testclass_hpp

#include <base/sa_baseobject.hpp>
#include <iostream>
namespace rpms
{

class SC_TestClass : public SA_BaseObject
{
        friend std::ostream& operator<<( std::ostream& os, SC_TestClass &aIn );
    public:
        SC_TestClass();
        SC_TestClass( const unsigned int aInt, const double aDbl );
        SC_TestClass( const SC_TestClass &aRhs );

        virtual ~SC_TestClass();

        SC_TestClass & operator=( const SC_TestClass &aRhs );

        unsigned int    getInt()    { return mTestInt; }
        double          getDouble() { return mTestDbl; }

        void setInt( const unsigned int aInt ) { mTestInt = aInt; };
        void setDouble( const double aDbl)     { mTestDbl = aDbl; };
        std::string debug() const;

    private:
        unsigned int    mTestInt;
        double          mTestDbl;
};

} // namespace


#endif
