//#############################
#include "sc_bullyelection.hpp"
#include "si_electionlog.hpp"
#include "sx_election.hpp"

#include <si_macros.hpp>

#include <logging/si_logging.hpp>
#include <sockets/sx_socket.hpp>
#include <sockets/sc_tcpsocket.hpp>
#include <utilities/ss_ini.hpp>
#include <xmlutil/sc_xmlnodeconfig.hpp>

#include <deque>

using rpms::SC_BullyElection;
using rpms::ST_SPointer;

///////////////////////////////////////////
SC_BullyElection::~SC_BullyElection()
{
    TRY(SC_BullyElection::~SC_BullyElection());
    CATCH;
}

///////////////////////////////////////////
SC_BullyElection::SC_BullyElection( const std::string &aXmlConfigFile )
    : SA_ActiveElection(aXmlConfigFile)
{
    TRY(SC_BullyElection::SC_BullyElection( const std::string &aXmlConfigFile ));
    CATCH;
}

///////////////////////////////////////////
bool SC_BullyElection::shouldILead()
{
    TRY(SC_StaticElection::shouldILead());
        RPMS_DEBUG(ELC_BUL_LOG, "Checking whether I should lead");
        // if the leaderhost has been set use it
        if( !getLeaderHost().empty() )
        {
            if( getLeaderHost() == SC_InetAddress::getLocalHostName() )
            {
                return true;
            }
            return false;
        }

        // if no leader host set then we have to find it out
        std::string highestNode = findHighestActiveNodeAboveMe();
        setLeaderHost(highestNode);
        if( highestNode == SC_InetAddress::getLocalHostName() )
        {
            return true;
        }
        return false;
    CATCH;
}

///////////////////////////////////////////
bool SC_BullyElection::allowMeToLead()
{
    TRY(SC_BullyElection::allowMeToLead());
        RPMS_DEBUG(ELC_BUL_LOG, "Making sure I am the only leader running");
        tellLesserNodesToStopDirector();
        return false;
    CATCH;
}

///////////////////////////////////////////
std::string SC_BullyElection::findHighestActiveNodeAboveMe()
{
    TRY(SC_BullyElection::findHighestActiveNodeAboveMe());
        RPMS_INFO(ELC_ACT_LOG, "Finding the highest active ranked node in the system");
        // ******** PART 1 get all the higher ranking nodes
        // we only need to check for superior ranking nodes
        int rank = getMyRank();
        std::deque<std::string> superiors;
        //This map is sorted by default so we can trust the reverse iterator
        std::map<int, ST_SPointer<SC_NodeConfig> >::reverse_iterator i = mCandidates.rbegin();
        while( i != mCandidates.rend() )
        {
            if( i->first > rank ) // will not include own rank
            {
                superiors.push_back(i->second->getHostName());
            }
            else
            {
                break;
            }
            ++i;
        }

        // ******* PART 2 assess their state
        // if there are no other daemons with a higher ranking then you should lead
        if( superiors.size() < 1 )
        {
            RPMS_INFO(ELC_ACT_LOG, "This node is the highest ranking of all nodes node");
            return SC_InetAddress::getLocalHostName();
        }
        // there is at least one node with a greater ranking
        else
        {
            int port = SS_Ini::instance()->getInt("election", "election.tcp.port");
            // this may need future modification to handle very large numbers of nodes
            std::deque< ST_SPointer<SC_BullyElection::SC_BullyElecDaemonCheckThread> > threads;
            // create them
            for( int i = 0; i < superiors.size(); ++i )
            {
                threads.push_back( SC_BullyElection::SC_BullyElecDaemonCheckThread::create( superiors.at(i) ) );
                threads.at(i)->start();
            }
            // wait for them all to finish
            for( int i = 0; i < superiors.size(); ++i )
            {
                threads.at(i)->join();
            }
                
            // Now we have all the threads back we can look to see if any of the are active, and if so what their rank is
            //      ultimately we need to find out who has the higherst rank
            int topRank = -1;
            int tmp = 0;
            std::map<int, ST_SPointer<SC_NodeConfig> >::reverse_iterator iter;
            // now assess if anything is running
            for( int i = 0; i < superiors.size(); ++i )
            {
                if( threads.at(i)->isDestAlive() )
                {
                    // find the rank of the hostname
                    iter = mCandidates.rbegin();
                    while( iter != mCandidates.rend() )
                    {
                        if( threads.at(i)->getDest() == iter->second->getHostName() )
                        {
                            tmp = iter->first;
                            if( tmp > topRank )
                            {
                                topRank = tmp;
                            }
                            break;
                        }
                        ++iter;
                    }
                }
            }
                
            if( topRank > -1 )
            {
                std::string ret = mCandidates.find(topRank)->second->getHostName();
                RPMS_INFO(ELC_ACT_LOG, "Host [" + ret + "] is the highest active ranking host in the system");
                return ret;
            }
            RPMS_INFO(ELC_ACT_LOG, "All other higher ranking nodes are inactive .. this nodes is highest");
            return SC_InetAddress::getLocalHostName();
        }
    CATCH;
}

///////////////////////////////////////////
void SC_BullyElection::processElectionMsg( const std::string &aMsg )
{
    TRY(SC_BullyElection::processElectionMsg( const std::string &aMsg ));
        std::string test = aMsg.substr(0,10);
        if( ":BLY-PROBE" == test )
        {
            RPMS_DEBUG( ELC_BUL_LOG, "Probe message received [" + aMsg + "]");
        }
        else if( ":BLY-LEADR" == test )
        {
            RPMS_DEBUG( ELC_BUL_LOG, "Leader message received [" + aMsg + "]");
            suggestLeader( getLeaderFromMsg(aMsg) );
        }
        else
        {
            RPMS_DEBUG( ELC_BUL_LOG, "Unknown message received [" + aMsg + "] ignoring it");
        }
    CATCH;
}

///////////////////////////////////////////
std::string SC_BullyElection::getLeaderFromMsg( const std::string &aMsg )
{
    TRY(SC_BullyElection::getLeaderFromMsg( const std::string &aMsg ));
        RPMS_DEBUG( ELC_BUL_LOG, "Getting leader from [" + aMsg + "]" );
        int pos = aMsg.find(':', 0);// first one
        pos = aMsg.find(':', ++pos );
        ++pos; // skip ':'
        RPMS_DEBUG( ELC_BUL_LOG, "Using the following leader [" + aMsg.substr( pos, (aMsg.length()-pos)-1 ) + "]" );
        return aMsg.substr( pos, (aMsg.length()-pos)-1 );
    CATCH;
}

///////////////////////////////////////////
void SC_BullyElection::tellLesserNodesToStopDirector()
{
    TRY(SC_BullyElection::tellLesserNodesToStopDirector());
        RPMS_DEBUG(ELC_BUL_LOG, "Telling lesser nodes to stop leader activities");
        int rank = getMyRank();
        std::deque<std::string> lessers;
        std::map<int, ST_SPointer<SC_NodeConfig> >::iterator i = mCandidates.begin();
        while( i != mCandidates.end() )
        {
            if( i->first < rank )
            {
                lessers.push_back( i->second->getHostName() );
            }
            else
            {
                break;
            }
            ++i;
        }
        
        // if there are none then we can do nothing
        if( lessers.size() < 1 )
        {
            return;
        }
        else
        {
            // now we set off a load of director killer threads .. they are threads so that we can 
            //      minimise the impact of the TCP timeouts
            std::deque< ST_SPointer<SC_BullyElection::SC_ActiveElectionDirectorKillerThread> > threads;
            int i = 0;
            for( ; i < lessers.size(); ++i )
            {
                threads.push_back( SC_BullyElection::SC_ActiveElectionDirectorKillerThread::create( lessers.at(i), getLocalHost() ) );
                threads.at(i)->start();
            }

            
            // wait for them to complete
            for( i = 0; i < threads.size(); ++i )
            {
                threads.at(i)->join();
            }

            // analysis of what we have just done
            for( i = 0; i < threads.size(); ++i)
            {
                if( threads.at(i)->isDestAlive() )
                {
                    if( threads.at(i)->didYouKill() )
                    {
                        RPMS_INFO(ELC_BUL_LOG, "Found a host [" + threads.at(i)->getDestHost() + "] with a running director and have killed it");
                    }
                    else
                    {
                        RPMS_INFO(ELC_BUL_LOG, "Found a host [" + threads.at(i)->getDestHost() + "] but it was not running a director");
                    }
                }
                else
                {
                    RPMS_INFO(ELC_BUL_LOG, "Host [" + threads.at(i)->getDestHost() + "] was not running");
                }
            }
        }
    CATCH;
}


///////////////////////////////////////////
///////////////////////////////////////////
// ****************************************
///////////////////////////////////////////

///////////////////////////////////////////
rpms::SC_BullyElection::SC_BullyElecDaemonCheckThread::~SC_BullyElecDaemonCheckThread()
{
    TRY(SC_BullyElection::SC_BullyElecDaemonCheckThread::~SC_BullyElecDaemonCheckThread());
    CATCH;
}

///////////////////////////////////////////
rpms::SC_BullyElection::SC_BullyElecDaemonCheckThread::SC_BullyElecDaemonCheckThread(const std::string &aThreadName, const std::string &aDestHost)
    : SA_ThreadBase(aThreadName), mDestHost(aDestHost), mDestAlive(false)
{
    TRY(SC_BullyElection::SC_BullyElecDaemonCheckThread::SC_BullyElecDaemonCheckThread(const std::string &aThreadName, const std::string &aDestHost));
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_BullyElection::SC_BullyElecDaemonCheckThread> rpms::SC_BullyElection::SC_BullyElecDaemonCheckThread::create(const std::string &aDestHost)
{
    TRY(SC_BullyElection::SC_BullyElecDaemonCheckThread::create(const std::string &aDestHost));
        return new SC_BullyElection::SC_BullyElecDaemonCheckThread( "ActiveElectionDaemonCheckThread", aDestHost );
    CATCH;
}

///////////////////////////////////////////
void rpms::SC_BullyElection::SC_BullyElecDaemonCheckThread::stop()
{
    TRY(SC_BullyElection::SC_BullyElecDaemonCheckThread::stop());
        throw SX_Election("SC_BullyElection::SC_BullyElecDaemonCheckThread::stop() is not relevant");
    CATCH;
}

///////////////////////////////////////////
void rpms::SC_BullyElection::SC_BullyElecDaemonCheckThread::reset()
{
    TRY(SC_BullyElection::SC_BullyElecDaemonCheckThread::reset());
        throw SX_Election("SC_BullyElection::SC_BullyElecDaemonCheckThread::reset() is not relevant");
    CATCH;
}
///////////////////////////////////////////
void rpms::SC_BullyElection::SC_BullyElecDaemonCheckThread::run()
{
    TRY(SC_BullyElection::SC_BullyElecDaemonCheckThread::run());
        ST_SPointer<SA_Socket> soc = new SC_TCPSocket();
        int port = SS_Ini::instance()->getInt("election", "election.tcp.port");
        try
        {
            soc->connect( mDestHost.c_str(), port);
            soc->send(":BLY-PROBE:"); // nothing more is needed than this
            mDestAlive = true;
        }
        catch( SX_Socket &sex )
        {
            mDestAlive = false;
        }
    CATCH;
}

///////////////////////////////////////////
// ****************************************
///////////////////////////////////////////

///////////////////////////////////////////
rpms::SC_BullyElection::SC_ActiveElectionDirectorKillerThread::~SC_ActiveElectionDirectorKillerThread()
{
    TRY(SC_BullyElection::SC_ActiveElectionDirectorKillerThread::~SC_ActiveElectionDirectorKillerThread());
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_BullyElection::SC_ActiveElectionDirectorKillerThread> rpms::SC_BullyElection::SC_ActiveElectionDirectorKillerThread::create(const std::string &aDestHost, const std::string &aLocalHost)
{
    TRY(SC_BullyElection::SC_ActiveElectionDirectorKillerThread::create(const std::string &aDestHost, const std::string &aLocalHost));
        return new SC_BullyElection::SC_ActiveElectionDirectorKillerThread( "ActiveElectionThread", aDestHost, aLocalHost );
    CATCH;
}


///////////////////////////////////////////
rpms::SC_BullyElection::SC_ActiveElectionDirectorKillerThread::SC_ActiveElectionDirectorKillerThread(const std::string &aThreadName, const std::string &aDestHost, const std::string &aLocalHost)
    : SA_ThreadBase(aThreadName), mDestHost(aDestHost), mKilled(false), mDestAlive(false), mLocalHost(aLocalHost)
{
    TRY(SC_BullyElection::SC_ActiveElectionDirectorKillerThread::SC_ActiveElectionDirectorKillerThread(const std::string &aThreadName, const std::string &aDestHost, const std::string &aLocalHost));
    CATCH;
}

///////////////////////////////////////////
void rpms::SC_BullyElection::SC_ActiveElectionDirectorKillerThread::stop()
{
    TRY(SC_BullyElection::SC_ActiveElectionDirectorKillerThread::stop());
        throw SX_Election("SC_BullyElection::SC_ActiveElectionDirectorKillerThread::stop() is not relevant");
    CATCH;
}

///////////////////////////////////////////
void rpms::SC_BullyElection::SC_ActiveElectionDirectorKillerThread::reset()
{
    TRY(SC_BullyElection::SC_ActiveElectionDirectorKillerThread::reset());
        throw SX_Election("SC_BullyElection::SC_ActiveElectionDirectorKillerThread::reset() is not relevant");
    CATCH;
}

///////////////////////////////////////////
void rpms::SC_BullyElection::SC_ActiveElectionDirectorKillerThread::run()
{
    TRY(SC_BullyElection::SC_ActiveElectionDirectorKillerThread::run());
        std::string msg = ":BLY-LEADR:" + mLocalHost + ":";
        SC_TCPSocket s;
        try
        {
            s.connect( SC_InetAddress(mDestHost.c_str(), SS_Ini::instance()->getInt("election", "election.tcp.port") ) );
            mDestAlive = true;
            s.send(msg);
            RPMS_DEBUG( ELC_RNG_LOG, "Sent [" + msg + "] to [" + mDestHost + "] fine" );
            mKilled = true;
        }
        catch( SX_Socket &sex )
        {
            mDestAlive = false;
            mKilled = false;
        }
    CATCH;
}



