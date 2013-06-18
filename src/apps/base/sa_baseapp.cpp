#include "sa_baseapp.hpp"
#include "si_baseapplog.hpp"

#include <sys/stat.h>
#include <signal.h>

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <base/sx_exception.hpp>

using rpms::SA_BaseApp;

static bool set = false;

///////////////////////////////////////////////////////////////
extern "C" void handler(int sig)
{
    // we only want this called once else all hell breaks loose
    //      as we are using threads we have to adapt to the Linux
    //      threading policy (no threads just cloned procesess sharing
    //      address space)
    if(!set)
    {
        set = true;
        SA_BaseApp::killApp();
    }
}

///////////////////////////////////////////////////////////////
void SA_BaseApp::killApp()
{
    mApp->shutdown();
}

///////////////////////////////////////////////////////////////
int SA_BaseApp::baseMain( int aArgC, char *aArgV[] )
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    try
    {
        struct stat stat_res;
        char * cfgFile = ::getenv("RPMS_CFG_FILE");

        if( NULL == cfgFile )
        {
            std::cerr << "\n\t*** RpmsApplication error: Environment variable \'RPMS_CFG_FILE\' " 
                << "required ***\n" << std::endl;
            exit(1);
        }

        if( 0 != ::stat( cfgFile, &stat_res ) )
        {
            std::cerr << "\n\t*** RpmsApplication: Configuration file [" << cfgFile 
                <<  "] does not exist ***\n" << std::endl;
            exit(1);
        }

        if( 0 != ::access( cfgFile, R_OK ) )
        {
            std::cerr << "\n\t*** RpmsApplication: No read permission on config file [" 
                << cfgFile <<  "] ***\n" << std::endl;
            exit(1);
        }

        SS_Ini::instance()->initialise(cfgFile);
        RPMS_INFO( BASE_APP_LOG, "********************************");
        RPMS_INFO( BASE_APP_LOG, "INI initialised with [" + std::string(cfgFile) + "]");
        RPMS_INFO( BASE_APP_LOG, "********************************");

        execute( aArgC, aArgV );

        RPMS_INFO( BASE_APP_LOG, "Application execution complete");
        RPMS_INFO( BASE_APP_LOG, "********************************");
        // now explicitly delete the app while we are in the try catch
        delete mApp;

    }
    catch( SX_Exception &x )
    {
        std::cerr << "\n\t*** RpmsApplication: Rpms Exception caught *** \n"
            << x.reason() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "\n\t*** RpmsApplication: Unknown exception caught ***\n" << std::endl;
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////
