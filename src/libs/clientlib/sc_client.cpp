//#############################
#include "sc_client.hpp"
#include "sx_client.hpp"
#include "si_clientlog.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <election/ss_electionfactory.hpp>
#include <election/sx_election.hpp>
#include <election/sa_election.hpp>
#include <corbaimpl/sx_corbaimpl.hpp>
#include <idllib/idls/sr_directortask.h>
#include <xmlutil/sc_xmltransformer.hpp>
#include <si_macros.hpp>

using rpms::SC_Client;

///////////////////////////////////////////
SC_Client::SC_Client()
{
    TRY(SC_Client::SC_Client());
    CATCH;
}

///////////////////////////////////////////
SC_Client::~SC_Client()
{
    TRY(SC_Client::~SC_Client());
    CATCH;
}

///////////////////////////////////////////
void SC_Client::shutdown()
{
    TRY(SC_Client::shutdown());
        RPMS_WARN(CLIENT_LOG, "***** Shutting down client .. this method has not been implemented");
    CATCH;
}

///////////////////////////////////////////
bool SC_Client::transformXml( const std::string &aXmlStatus )
{
    TRY(SC_Client::transformXml( const std::string &aXmlStatus ));
        RPMS_TRACE( CLIENT_LOG, "Transforming XML:\n" + aXmlStatus );
        std::string xsl = SS_Ini::instance()->getString("directories", "xsl") + "/" 
            + SS_Ini::instance()->getString("client", "client.xsl.template");
        SC_XMLTransformer t(aXmlStatus);
        t.load();
        return t.transform(xsl);
    CATCH;
}

///////////////////////////////////////////
bool SC_Client::performTask( const EAction &aAction, const std::string &aParam)
{
    TRY(SC_Client::performTask( const EAction &aAction, const std::string &aParam));
        bool ret = false;
        try
        {
            std::string idlName = SS_Ini::instance()->getString("director", "director.idl.remotename");
            std::string port = SS_Ini::instance()->getString("CORBA", "corba.orb.port");
            CORBA::Object_var obj;
            try
            {
                obj = SS_ElectionFactory::instance()->getElection(false)->getRemoteObject( idlName, port );
            }
            catch( SX_Election &sex )
            {
                RPMS_ERROR(CLIENT_LOG, "Could not find a leader host to bind against:\n" + sex.reason());
                return false;
            }
            catch( SX_CorbaImpl &cex )
            {
                RPMS_ERROR(CLIENT_LOG, "Could not bind against assumed leader host .. perhaps it is in the middle of being elected leader:\n" +  cex.reason());
                return false;
            }

            RPMS_TRACE( CLIENT_LOG, "Creating local interface to remote object" );
            SR_DirectorTask_var task = SR_DirectorTask::_narrow(obj);
            if( CORBA::is_nil(task) )
            {
                throw SX_Client("Failed to locate SR_DirectorTask object");
            }

            std::string retVal = "";
            switch( aAction )
            {
                case START:
                    RPMS_DEBUG( CLIENT_LOG, "Performing start task against remote server");
                    ret = task->clientStart();
                        break;
                case KILL:
                    RPMS_DEBUG( CLIENT_LOG, "Performing kill task against remote server");
                    ret = task->clientStop();
                    break;
                case STATUS:
                    RPMS_DEBUG( CLIENT_LOG, "Performing getStatus task against remote server");
                    retVal = task->clientGetStatus();
                    ret = transformXml( retVal );
                    break;
                case RECONFIGURE:
                    RPMS_DEBUG( CLIENT_LOG, "Performing reconfigure task against remote server");
                    ret = task->clientReconfigure( aParam.c_str() );
                    break;
                case RESTART_FAILURE:
                    RPMS_DEBUG( CLIENT_LOG, "Performing restart failed app task against remote server for [" + aParam + "]");
                    ret = task->clientRestartFailedApp( aParam.c_str() );
                    break;
                case RESTART_APP_TYPE:
                    RPMS_DEBUG( CLIENT_LOG, "Performing restart app task against remote server for [" + aParam + "]");
                    ret = task->clientRestartApp( aParam.c_str() );
                    break;
                case STOP_DIRECTOR:
                    RPMS_DEBUG( CLIENT_LOG, "Performing stop director task against remote server");
                    task->clientStopDirector();
                    ret = true;
                    break;
                default:
                    return false;
                    break;
            }
        }
        catch( CORBA::Exception &ex )
        {
            ex._print(std::cout);
            throw SX_Client("Exception caught of type [CORBA::Exception]");
        }
        return ret;
    CATCH;
}


