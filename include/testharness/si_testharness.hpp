#ifndef __si_testharness_hpp
#define __si_testharness_hpp

/** 
 * @author  Peter Suggitt (2005) 
 * @note <b>Testharness include for logging</b> \n
 * This is only used by the testharness macro
 * */


#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <base/sx_exception.hpp>

#define LOGNAME "testharness"

#define TEST_BASE                                                                                                   \
                                                                                                                    \
using rpms::SS_Ini;                                                                                                 \
using rpms::SX_Exception;                                                                                           \
using rpms::SX_Unknown;                                                                                             \
using rpms::SS_LoggerFactory;                                                                                       \
using rpms::ST_SPointer;                                                                                            \
                                                                                                                    \
void execute(int aArgC, char *aArgV[]);                                                                             \
                                                                                                                    \
int main(int aArgC, char *aArgV[])                                                                                  \
{                                                                                                                   \
    try                                                                                                             \
    {                                                                                                               \
        SS_Ini::instance()->initialise("/home/suggitpe/test/RPMS/" + std::string(::getenv("RPMS_VER")) + "/config/rpms.ini");                          \
        execute(aArgC, aArgV);                                                                                      \
    }                                                                                                               \
    catch( SX_Exception &x )                                                                                        \
    {                                                                                                               \
        std::cerr << x.reason() << std::endl;                                                                       \
    }                                                                                                               \
    return 0;                                                                                                       \
}                                                                                                                   \

#endif

