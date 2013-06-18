//#############################
#include "sc_ringelection.hpp"
#include "si_electionlog.hpp"
#include "sx_election.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <sockets/sc_tcpsocket.hpp>
#include <sockets/sx_socket.hpp>

using rpms::SC_RingElection;

///////////////////////////////////////////
SC_RingElection::SC_RingElection( const std::string &aXmlConfigFile )
    : SA_ActiveElection(aXmlConfigFile), mElecting(false), mShortList(0)
{
    TRY(SC_RingElection::SC_RingElection( const std::string &aXmlConfigFile ));
    CATCH;
}

///////////////////////////////////////////
SC_RingElection::~SC_RingElection()
{
    TRY(SC_RingElection::~SC_RingElection());
        if( mShortList != 0 )
        {
            delete mShortList;
        }
    CATCH;
}

///////////////////////////////////////////
bool SC_RingElection::shouldILead()
{
    TRY(SC_RingElection::shouldILead());
        RPMS_DEBUG(ELC_RNG_LOG, "Checking whether I should lead");
        if( !getLeaderHost().empty() )
        {
            if( getLeaderHost() == getLocalHost() )
            {
                return true;
            }
            return false;
        }
        // now we initiate the election itself
        initiateRingElection();
        while( isElecting() )
        {
            usleep(250000);
            RPMS_TRACE(ELC_RNG_LOG,"Waiting for election results...");
        }
        
        if( !getLeaderHost().empty() )
        {
            if( getLeaderHost() == getLocalHost() )
            {
                return true;
            }
            return false;
        }
        // problem occured in the election
        RPMS_WARN(ELC_RNG_LOG, "Weird results from ring election ... could not find a leader, yet I know it is not me");

        return false;
    CATCH;
}

///////////////////////////////////////////
void SC_RingElection::initiateRingElection()
{
    TRY(SC_RingElection::initiateRingElection());
        RPMS_INFO(ELC_RNG_LOG,"Initiating ring election");
        mElecting = true;
        std::string msg = createInitialMsg("ELECT");
        if( sendMsgToNextNode(msg) )
        {
            return;
        }
        // if we end up here then it means that we are the only node running
        //      and therefore we mst lead
        suggestLeader(getLocalHost());
        mElecting = false;
    CATCH;
}


///////////////////////////////////////////
bool SC_RingElection::allowMeToLead()
{
    TRY(SC_RingElection::allowMeToLead());
        RPMS_DEBUG(ELC_RNG_LOG, "Allow me to lead is not implemented");
        return true;
    CATCH;
}

///////////////////////////////////////////
void SC_RingElection::processElectionMsg( const std::string &aMsg )
{
    TRY(SC_RingElection::processElectionMsg( const std::string &aMsg ));
        std::string me = getLocalHost();
        std::string test = aMsg.substr(0,10);
        if( ":RNG-ELECT" == test )
        {
            if( me != getOriginatingHostFromElecMsg(aMsg) )
            {
                // just pass it on adding in our own details to the message
                RPMS_DEBUG( ELC_RNG_LOG, "We have someone elses msg [" + aMsg + "] passing it on ...");
                sendMsgToNextNode((aMsg + createSelfForElecMsg()) );
            }
            else
            {
                // This is our own message back again
                RPMS_DEBUG( ELC_RNG_LOG, "We have our own msg back [" + aMsg + "]");
                initiateCoOrdMsg(aMsg);
            }
        }
        else if( ":RNG-COORD" == test )
        {
            std::string leader = getElectedLeaderFromCoordMsg(aMsg);
            if( me != getOriginatingHostFromElecMsg(aMsg) )
            {
                RPMS_DEBUG( ELC_RNG_LOG, "We have someone elses coordmsg [" + aMsg + "], leader is [" + leader + "]");
                suggestLeader( leader );
                sendMsgToNextNode(aMsg);
            }
            else
            {
                RPMS_DEBUG( ELC_RNG_LOG, "We have our own msg back [" + aMsg + "], leader is [" + leader + "]");
                suggestLeader(leader);
                mElecting = false;
            }
        }
        else
        {
            RPMS_DEBUG( ELC_RNG_LOG, "Unknown message received [" + aMsg + "] ignoring it");
        }
    CATCH;
}

///////////////////////////////////////////
std::string SC_RingElection::findNextRingNode( const std::string &aHost )
{
    TRY(SC_RingElection::findNextRingNode( const std::string &aHost ));
        if( mShortList == 0 )
        {
            mShortList = new std::map<int, std::string>(getElectionShortList());
        }
        std::map<int, std::string>::iterator i = mShortList->begin();
        while(i != mShortList->end())
        {
            if( i->second == aHost )
            {
                ++i;
                if( i != mShortList->end() )
                {
                    return i->second;
                }
                else
                {
                    return mShortList->begin()->second;
                }
            }
            ++i;
        }
        throw SX_Election("Reached end of findNextRingNode with host [" + aHost + "] without finding a next node");
    CATCH;
}

///////////////////////////////////////////
std::string SC_RingElection::createInitialMsg( const std::string & aPrefix )
{
    using rpms::convert;
    TRY(SC_RingElection::createInitialMsg( const std::string & aPrefix ));
        return ":" + getMessagePrefix() + "-" + aPrefix 
            + ":" + getLocalHost()
            + "-" + convert<std::string>(getMyRank()) + ":";
    CATCH;
}


///////////////////////////////////////////
bool SC_RingElection::sendMsgToNextNode( const std::string &aMsg )
{
    TRY(SC_RingElection::sendMsgToNextNode( const std::string &aMsg ));
        std::string me = getLocalHost();
        std::string next = findNextRingNode(me);
        SC_TCPSocket s;
        while( next != me )
        {
            try
            {
                s.connect(SC_InetAddress(next.c_str(), getElectionPort()));
                s.send(aMsg);
                RPMS_DEBUG( ELC_RNG_LOG, "Sent [" + aMsg + "] to [" + next + "] fine" );
                return true;
            }
            catch( SX_Socket &ex )
            {
                RPMS_DEBUG(ELC_RNG_LOG, "Next host [" + next + "] not alive, skipping");
            }

            // did we just fail to the originator of the message?
            if( next == getOriginatingHostFromElecMsg(aMsg) )
            {
                RPMS_WARN(ELC_RNG_LOG, "Message not being passed on due to originator no longer running");
                // here we protect against the originator not actually running (stop the message spining throughout the system)
                return false;
            }
            next = findNextRingNode(next);
        }
        return false;
    CATCH;
}

///////////////////////////////////////////
void SC_RingElection::initiateCoOrdMsg( const std::string &aMsg )
{
    TRY(SC_RingElection::initiateCoOrdMsg( const std::string &aMsg ));
        std::string c_msg = createInitialMsg("COORD") + getElectedLeaderFromElecMsg(aMsg) + ":";
        if( sendMsgToNextNode(c_msg) )
        {
            return;
        }
        mElecting = false;
        RPMS_WARN(ELC_RNG_LOG, "No one to send the coordinator message to");
    CATCH;
}


///////////////////////////////////////////
std::string SC_RingElection::createSelfForElecMsg()
{
    using rpms::convert;
    TRY(SC_RingElection::createSelfForElecMsg(););
        return getLocalHost() + "-" + convert<std::string>(getMyRank()) + ":";
    CATCH;
}

///////////////////////////////////////////
std::string SC_RingElection::getElectedLeaderFromCoordMsg(const std::string &aMsg )
{
    TRY(SC_RingElection::getElectedLeaderFromCoordMsg(const std::string &aMsg ));
        // using msg .. eg :RNG-COORD:pgdsX00-4:pgdsX00:
        int pos = aMsg.find(':', 0); // first one
        for( int i = 0; i < 2; ++i )
        {
            pos = aMsg.find(':',++pos);
        }
        ++pos;
        return aMsg.substr(pos, (aMsg.length()-pos)-1);
    CATCH;
}

///////////////////////////////////////////
std::string SC_RingElection::getElectedLeaderFromElecMsg(const std::string &aMsg )
{
    TRY(SC_RingElection::getElectedLeaderFromElecMsg(const std::string &aMsg ));
        // using msg .. eg :RNG-ELECT:pgdsX00-4:pgdsX02-1:pgdsX01-3:
        //std::deque<std::string> x;
        int pos = aMsg.find(':', 1);
        int end = 0;
        std::string eval = "";
        std::string ret = "";
        int tick = -1;
        int tmp = -1;
        int topRank = -1;
            
        while( pos != std::string::npos )
        {
            ++pos;
            end = aMsg.find(':', pos);
            if( end != std::string::npos )
            {
                eval = aMsg.substr(pos, end-pos);
                tick = eval.find('-',0);
                tmp = atoi( eval.substr(tick+1, eval.length()-(tick+1)).c_str() );
                if( tmp > topRank )
                {
                    topRank = tmp;
                    ret = eval.substr(0,tick);
                }
            }
            pos = end;
        }

        RPMS_TRACE( ELC_RNG_LOG, "Top rank from [" + aMsg + "] is [" + ret + "]" );
        return ret;
    CATCH;
}

///////////////////////////////////////////
std::string SC_RingElection::getOriginatingHostFromElecMsg( const std::string &aMsg )
{
    TRY(SC_RingElection::getOriginatingHostFromElecMsg( const std::string &aMsg ));
        int start = aMsg.find(':', 1)+1;
        int end = aMsg.find('-', start);
        return aMsg.substr(start, end-start);
    CATCH;
}

///////////////////////////////////////////
