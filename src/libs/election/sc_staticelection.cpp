//#############################
#include "sc_staticelection.hpp"
#include "sx_election.hpp"
#include "si_electionlog.hpp"

#include <logging/si_logging.hpp>
#include <sockets/sc_inetaddress.hpp>
#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>
#include <xmlutil/sc_xmlnodeconfig.hpp>

using rpms::SC_StaticElection;

///////////////////////////////////////////
SC_StaticElection::~SC_StaticElection()
{
    TRY(SC_StaticElection::~SC_StaticElection());
    CATCH;
}

///////////////////////////////////////////
SC_StaticElection::SC_StaticElection( const std::string &aXmlConfigFile )
    : SA_Election(aXmlConfigFile)
{
    TRY(SC_StaticElection::SC_StaticElection( const std::string &aXmlConfigFile ));
    CATCH;
}

///////////////////////////////////////////
bool SC_StaticElection::shouldILead()
{
    TRY(SC_StaticElection::shouldILead());
        /// just takes the highest configured node and compares with this host
        if ( SC_InetAddress::getLocalHostName() == findLeaderHost() )
        {
            setLeader();
            return true;
        }
        return false;
    CATCH;
}

///////////////////////////////////////////
bool SC_StaticElection::allowMeToLead()
{
    return true;
}

///////////////////////////////////////////
std::string SC_StaticElection::findLeaderHost()
{
    TRY(SC_StaticElection::getLeaderHost());
        if( getLeaderHost().empty() )
        {
            std::map<int, ST_SPointer<SC_NodeConfig> >::reverse_iterator i = mCandidates.rbegin();
            setLeaderHost(i->second->getHostName());
        }
        return getLeaderHost();
    CATCH;
}

///////////////////////////////////////////
bool SC_StaticElection::suggestLeader( const std::string &aHost)
{
    TRY(SC_StaticElection::suggestLeader( const std::string &aHost));
        setLeaderHost(aHost);
        return true;
    CATCH;
}

///////////////////////////////////////////
void SC_StaticElection::processElectionMsg( const std::string &aMsg )
{
    TRY(SC_StaticElection::processElectionMsg( const std::string &aMsg ));
        RPMS_DEBUG( ELC_STC_LOG, "Election message received [" + aMsg + "]" );
    CATCH;
}

///////////////////////////////////////////
