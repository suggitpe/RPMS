//#############################
#include "sa_activeelection.hpp"

#include "si_electionlog.hpp"
#include "sx_election.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <sockets/sc_tcpsocket.hpp>
#include <sockets/sx_socket.hpp>
#include <xmlutil/sc_xmlnodeconfig.hpp>

using rpms::SA_ActiveElection;

///////////////////////////////////////////
SA_ActiveElection::SA_ActiveElection(const std::string &aXmlConfigFile)
    : SA_Election(aXmlConfigFile)
{
    TRY(SA_ActiveElection::SA_ActiveElection(const std::string &aXmlConfigFile));
    CATCH;
}

///////////////////////////////////////////
SA_ActiveElection::~SA_ActiveElection()
{
    TRY(SA_ActiveElection::~SA_ActiveElection());
    CATCH;
}

///////////////////////////////////////////
//  This method assumes that the highest ranking active node is the one that is the leader
//      it therefore simply tests all the nodes in reverse order to find the leader.  As 
//      soon as it finds one it will report this to the calling function
std::string SA_ActiveElection::findLeaderHost()
{
    using rpms::convert;
    TRY(SA_ActiveElection::findLeaderHost());
        if( !getLeaderHost().empty())
        {
            RPMS_TRACE(ELC_ACT_LOG, "Using current leader [" + getLeaderHost() + "]");
            return getLeaderHost();
        }
        std::string ret;
        std::map<int, ST_SPointer<SC_NodeConfig> >::reverse_iterator i = mCandidates.rbegin();
        ST_SPointer<SA_Socket> soc = new SC_TCPSocket();
        bool found = false;
        while( i != mCandidates.rend() )
        {
            try
            {
                soc->connect( i->second->getHostName().c_str(), getElectionPort() );
                found = true;
                ret = i->second->getHostName();
                RPMS_TRACE(ELC_ACT_LOG, "Found leader node at [" + ret + "]");
                break;
            }
            catch( SX_Socket &sex )
            {
                RPMS_WARN(ELC_ACT_LOG, "Failed to connect to node [" + i->second->getHostName() 
                        + "] ranked [" + convert<std::string>(i->first) + "]");
            }
            ++i;
        }
        
        if( !found )
        {
            throw SX_Election("Could not find a leader host");
        }

        return ret;
    CATCH;
}

///////////////////////////////////////////
bool SA_ActiveElection::suggestLeader( const std::string &aHost)
{
    TRY(SA_ActiveElection::suggestLeader( const std::string &aHost));
        if( getLeaderHost().empty() )
        {
            RPMS_DEBUG(ELC_ACT_LOG, "No current leader set .. assuming leader [" + aHost + "] is fine");
            setLeaderHost(aHost);
            return true;
        }
        else if( getLeaderHost() == aHost )
        {
            RPMS_DEBUG(ELC_ACT_LOG, "Suggested Leader [" + aHost + "] the same as actual leader [" + getLeaderHost() + "] --> no change required");
            return true;
        }
        std::map<int, ST_SPointer<SC_NodeConfig> >::reverse_iterator i = mCandidates.rbegin();
        ST_SPointer<SA_Socket> soc = new SC_TCPSocket();
        // go through the election list in order and look for the current leader and the suggested
        //      one .. which ever one is higher in the list will win
        while( i != mCandidates.rend() )
        {
            // first check to see if we can connect to the host we already believe to be the 
            //  leader (done in the correct order)
            if( i->second->getHostName() == getLeaderHost() )
            {
                try
                {
                    soc->connect( i->second->getHostName().c_str(), getElectionPort() );
                    RPMS_INFO(ELC_ACT_LOG, "Even though suggested leader is [" + std::string(aHost) + 
                            "], current leader [" + getLeaderHost() + "] takes precedense");
                    return false;
                }
                catch(SX_Socket &sex)
                {
                    RPMS_INFO(ELC_ACT_LOG, "Current leader is not running .. resetting leaderhost to null");
                    clearLeaderHost();
                }
            }

            // now we test that the suggested leader is there and all is OK with it
            if( i->second->getHostName() == aHost )
            {
                try
                {
                    soc->connect( i->second->getHostName().c_str(), getElectionPort() );
                    RPMS_INFO(ELC_ACT_LOG, "Suggested leader [" + std::string(aHost) + "], takes precedense over current one [" + getLeaderHost() + "]");
                    // are we the leader at the moment incorrectly
                    RPMS_INFO(ELC_ACT_LOG, "leader = " + getLeaderHost() + ", local  = " + getLocalHost() + ", new = " + aHost );
                    if( (getLeaderHost() == getLocalHost()) && (getLocalHost() != aHost) )
                    {
                        // we need to stop runnig it ourselves
                        RPMS_INFO(ELC_ACT_LOG, "stopping local director" );
                        mDaemon->deRegisterCurrentDirector();
                    }
                    else if( getLeaderHost() != aHost )
                    {
                        RPMS_INFO(ELC_ACT_LOG, "need to re-register with new leader" );
                        mDaemon->deRegisterCurrentDirector();
                    }
                    setLeaderHost(aHost);
                    return true;
                }
                catch(SX_Socket &sex)
                {
                    RPMS_INFO(ELC_ACT_LOG, "Suggested leader is not running");
                    return false;
                }
            }
            ++i;
        }
        RPMS_INFO(ELC_ACT_LOG, "Neither the suggested or current leaders are running");
        clearLeaderHost();
        return false;
    CATCH;
}

///////////////////////////////////////////
