//#############################
#include "sa_election.hpp"
#include "si_electionlog.hpp"
#include "sx_election.hpp"
#include "sc_electioncommthread.hpp"

#include <si_macros.hpp>
#include <corbaimpl/ss_corbaimpl.hpp>
#include <corbaimpl/sx_corbaimpl.hpp>
#include <sockets/sc_tcpsocket.hpp>
#include <sockets/sx_socket.hpp>
#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <xmlutil/sc_xmlnodeconfig.hpp>

using rpms::SA_Election;

///////////////////////////////////////////
SA_Election::~SA_Election()
{
    TRY(SA_Election::~SA_Election());
    CATCH;
}

///////////////////////////////////////////
SA_Election::SA_Election(const std::string &aXmlConfigFile)
    :mXmlConfigFile(aXmlConfigFile), mRank(-99), mLeader(false), mCommThread(0), mLocalHost("unset"), mElectionPort(0)
{
    TRY(SA_Election::SA_Election(const std::string &aXmlConfigFile));
    CATCH;
}

///////////////////////////////////////////
void SA_Election::initialise()
{
    TRY(SA_Election::initialise());
        mElectionPort = SS_Ini::instance()->getInt("election", "election.tcp.port");
        mLocalHost = SC_InetAddress::getLocalHostName();
        ST_SPointer<SC_XMLNodeConfig> nodes = new SC_XMLNodeConfig( mXmlConfigFile );
        nodes->load();
        mCandidates = nodes->getElections();
        mRank = nodes->getRank( getLocalHost() );
    CATCH;
}

///////////////////////////////////////////
void SA_Election::initialiseElectable()
{
    TRY(SA_Election::initialsie());
        std::string port = SS_Ini::instance()->getString("CORBA", "corba.orb.port");
        std::string remoteName = SS_Ini::instance()->getString("daemon", "daemon.idl.remotename");
        try
        {
            CORBA::Object_var obj = SS_CorbaImpl::instance()->getRemoteObject( mLocalHost, port, remoteName );
            mDaemon = SR_DaemonTask::_narrow(obj);
        }
        catch( SX_CorbaImpl &cex )
        {
            RPMS_WARN(ELC_BAS_LOG, "Failed to connect to remote object [" + remoteName 
                    + "] on [" + mLocalHost + ":" + port + "]");
            throw;
        }
        
        if( CORBA::is_nil(mDaemon) )
        {
            throw SX_Election("Failed to get remote object [" + remoteName 
                    + "] on [" + mLocalHost + ":" + port + "]");
        }
        mCommThread = SC_ElectionCommThread::create( this );
        mCommThread->start();
    CATCH;
}

///////////////////////////////////////////
CORBA::Object_var SA_Election::getRemoteObject( const std::string &aRemoteName, const std::string &aPort )
{
    TRY(SA_Election::getRemoteObject( const std::string &aRemoteName, const std::string &aPort ));
        return SS_CorbaImpl::instance()->getRemoteObject( findLeaderHost(), aPort, aRemoteName);
    CATCH;
}

///////////////////////////////////////////
std::map<int, std::string> SA_Election::getElectionShortList()
{
    TRY(std::string> SA_Election::getElectionShortList());
        std::map<int, ST_SPointer<SC_NodeConfig> >::iterator i = mCandidates.begin();
        std::map<int, std::string> ret;
        while(i != mCandidates.end())
        {
            ret.insert( std::pair<int, std::string>(i->first, i->second->getHostName()) );
            ++i;
        }
        return ret;
    CATCH;
}
///////////////////////////////////////////
