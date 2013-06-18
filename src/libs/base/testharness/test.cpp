//#############################

#include "sc_testclass.hpp"
#include <base/st_spointer.hpp>
#include <base/sa_baseobject.hpp>
#include <testharness/si_testharness.hpp>

TEST_BASE;

using rpms::SC_TestClass;

////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    
    RPMS_DEBUG( LOGNAME, "This is an application to test objects");
    RPMS_INFO( LOGNAME, "START ***************************");
    // random shit
    RPMS_DEBUG( LOGNAME, "***************************");
    {
        SC_TestClass obj1(1, 1.2);
        RPMS_INFO( LOGNAME, "\tobj1: " + obj1.debug() );
    }
    RPMS_DEBUG( LOGNAME, "***************************");
    {
        SC_TestClass obj2;
        RPMS_INFO( LOGNAME, "\tobj2" + obj2.debug());
    }
    RPMS_DEBUG( LOGNAME, "***************************");
    {
        ST_SPointer<SC_TestClass> pObj (new SC_TestClass(23, 12.2));
        RPMS_INFO(LOGNAME, "pObj" + pObj->debug());
    }
    RPMS_DEBUG( LOGNAME, "***************************");
    {
        ST_SPointer<SC_TestClass> pObj = new SC_TestClass(23, 12.2);
        RPMS_INFO( LOGNAME, "pObj" + pObj->debug());
    }
    
    RPMS_INFO( LOGNAME, "END ***************************" );
}

