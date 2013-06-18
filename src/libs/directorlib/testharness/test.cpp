//#############################
#include <testharness/si_testharness.hpp>

TEST_BASE;

void testDistribution();

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    TRY(execute());
    RPMS_INFO( LOGNAME, "START ***************************");
    testDistribution();
    RPMS_INFO( LOGNAME, "END   ***************************");
    CATCH;
}

//////////////////////////////////////////////////
void testDistribution()
{
}

