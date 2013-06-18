///////////////////////////////////////////////////////////////
#include "sc_daemonapp.hpp"
#include "si_daemonapplog.hpp"

#include <daemonlib/sc_daemon.hpp>
#include <logging/si_logging.hpp>
#include <corbaimpl/ss_corbaimpl.hpp>

using rpms::SC_DaemonApp;
using rpms::SA_Application;
using rpms::SA_BaseApp;

SA_Application *SA_BaseApp::mApp = new SC_Daemon();

///////////////////////////////////////////////////////////////
// EXECUTION START
int main( int aArgC, char *aArgV[] )
{
    SC_DaemonApp app;
    return app.baseMain(aArgC, aArgV);
}

///////////////////////////////////////////////////////////////
int SC_DaemonApp::execute( int aArgC, char *aArgV[] )
{
    SS_CorbaImpl::instance()->initOrb( true, true);
    RPMS_DEBUG( APP_LOG, "Starting SC_Daemon ....");
    dynamic_cast<SC_Daemon*>(mApp)->run();
    return 0;
}

///////////////////////////////////////////////////////////////
