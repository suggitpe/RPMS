///////////////////////////////////////////////////////////////
#include "sc_clientapp.hpp"
#include "si_clientapplog.hpp"

#include <clientlib/sc_client.hpp>
#include <logging/si_logging.hpp>
#include <corbaimpl/ss_corbaimpl.hpp>

using rpms::SC_ClientApp;
using rpms::SA_Application;
using rpms::SA_BaseApp;

SA_Application *SA_BaseApp::mApp = new SC_Client();

///////////////////////////////////////////////////////////////
// EXECUTION START
int main( int aArgC, char *aArgV[] )
{
    SC_ClientApp app;
    return app.baseMain(aArgC, aArgV);
}

///////////////////////////////////////////////////////////////
int SC_ClientApp::execute( int aArgC, char *aArgV[] )
{
        // first we have to process the inputs to the client
        int c;
        
        int opts = 0;
        bool force = false;
        SC_Client::EAction action = SC_Client::NOTHING;
        std::string param = "";
        while( 1 )
        {
            int opt_ind = 0;
            c = getopt_long_only(aArgC, aArgV, "", long_opts, &opt_ind );
            if( -1 == c ) // last arg
            {
                break;
            }

            switch(c)
            {
                case 's':
                    action = SC_Client::START;
                    ++opts;
                    break;
                case 'k':
                    action = SC_Client::KILL;
                    ++opts;
                    break;
                case 'v':
                    action = SC_Client::STATUS;
                    ++opts;
                    break;
                case 'r':
                    action = SC_Client::RECONFIGURE;
                    ++opts;
                    break;
                case 'f':
                    action = SC_Client::RESTART_FAILURE;
                    param = optarg;
                    ++opts;
                    break;
                case 'b':
                    action = SC_Client::RESTART_APP_TYPE;
                    param = optarg;
                    ++opts;
                    break;
                //case 'd': // for test cases only
                //    action = SC_Client::STOP_DIRECTOR;
                //    ++opts;
                //    break;
                case 'a':
                    param = "force";
                    force = true;
                    break;
                default:
                    usage();
                    break;
            }
        }

        // just a quick check to make sure that force is being used safely
        if( force && action != SC_Client::RECONFIGURE )
        {
            RPMS_ERROR(APP_LOG, "Bad use of arguments: force (-a) only allowed with reconfigure (-r)" );
            SC_ClientApp::usage();
        }

        if( optind < aArgC || opts != 1 )
        {
            if( aArgC > 1 )
            {
                std::string arg = "";
                for( int i = 1; i < aArgC; ++i )
                {
                    arg.append(aArgV[i]).append(" ");
                }
                RPMS_ERROR(APP_LOG, "Problems with command arguments passed [" + arg + "]" );
            }
            else
            {
                RPMS_ERROR(APP_LOG, "No arguments passed in:" );
            }
            SC_ClientApp::usage();
        }

        SS_CorbaImpl::instance()->initOrb( true, false);

        if(dynamic_cast<SC_Client*>(mApp)->performTask( action, param ))
        {
            RPMS_INFO( APP_LOG, "Successfully ran the task against the client");
        }
        else
        {
            RPMS_ERROR( APP_LOG, "Failed to run the task against the client");
        }

    return 0;
}

///////////////////////////////////////////////////////////////
void SC_ClientApp::usage()
{
    RPMS_INFO( APP_LOG, "          RpmsClient ... invalid usage         ");
    for( int i = 0; long_opts[i].name; ++i)
    {
        RPMS_INFO( APP_LOG, std::string("\t --") + long_opts[i].name );
    }
    exit(1);
}
