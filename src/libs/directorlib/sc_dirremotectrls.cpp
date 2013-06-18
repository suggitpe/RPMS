//#############################
#include "sc_dirremotectrls.hpp"
#include "si_directorlog.hpp"
#include "sx_director.hpp"
#include "sc_daemonctrl.hpp"

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <distributor/ss_distributor.hpp>
#include <si_macros.hpp>
#include <tools/st_guard.hpp>
#include <xmlutil/sc_xmlstatus.hpp>
#include <xmlutil/sc_xmlnodeconfig.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>
#include <xmlutil/sc_xmlprocstats.hpp>

#include <vector>
#include <deque>

using rpms::SC_DirRemoteCtrls;

///////////////////////////////////////////
SC_DirRemoteCtrls::SC_DirRemoteCtrls()
    : mLock("SC_DirRemoteCtrls"), mNodes(0), mSystemRunning(false), 
        mClientInteraction(false)
{
    TRY(SC_DirRemoteCtrls::SC_DirRemoteCtrls());
        mDaemonCtrls = new ctrlMap;
    CATCH;
}

///////////////////////////////////////////
SC_DirRemoteCtrls::~SC_DirRemoteCtrls()
{
    TRY(SC_DirRemoteCtrls::~SC_DirRemoteCtrls());
        delete mDaemonCtrls;
    CATCH;
}

///////////////////////////////////////////
SC_DirRemoteCtrls::SC_DirRemoteCtrls( const SC_DirRemoteCtrls& aRhs )
    : mLock("dummy")
{
    TRY(SC_DirRemoteCtrls::SC_DirRemoteCtrls( const SC_DirRemoteCtrls& aRhs ));
        throw SX_Director("copy ctor not allowed in SC_DirRemoteCtrls");
    CATCH;
}
   
///////////////////////////////////////////
SC_DirRemoteCtrls& SC_DirRemoteCtrls::operator=( const SC_DirRemoteCtrls& aRhs )
{
    TRY(SC_DirRemoteCtrls::operator=( const SC_DirRemoteCtrls& aRhs ));
        throw SX_Director("assignment operator not allowed in SC_DirRemoteCtrls");
        return *this;
    CATCH;
}

///////////////////////////////////////////
void SC_DirRemoteCtrls::initialise()
{
    TRY(SC_DirRemoteCtrls::initialise());
        std::string xmlFile = SS_Ini::instance()->getString("directories", "config")
            + "/" + SS_Ini::instance()->getString("director", "director.xml.appconfig");
        mNodes = new SC_XMLNodeConfig( xmlFile );
        mNodes->load();
        // this will simply make sure that the distributor has 
        //      loaded up before we start using it
        SS_Distributor::instance()->initialise();
    CATCH;
}

///////////////////////////////////////////
bool SC_DirRemoteCtrls::addDaemon( const std::string &aHostName, const bool aIsAppRunning )
{
    TRY(SC_DirRemoteCtrls::addDaemon( const std::string &aHostName, const bool aIsAppRunning ));
        ST_Guard<SA_Mutex> m( &mLock );
        cleanDeadDaemons(); // clean up old ones so that old ones can reregister if needed
        if( daemonExists(aHostName) )
        {
            RPMS_WARN( DIR_CRTL_LOG, "Attempting to add duplicate controller for hostname [" 
                    + aHostName + "]" );
            return false;
        }
        
        RPMS_DEBUG( DIR_CRTL_LOG, "Adding daemon for hostname [" + aHostName + "]" );
        mDaemonCtrls->insert( ctrlPair(aHostName, SC_DaemonCtrl::create( aHostName,
                    SS_Ini::instance()->getString("threadnames", "daemonctrl") 
                    + "_" + aHostName ) ) );
        
        ctrlMap::iterator dmn = mDaemonCtrls->find(aHostName);
        dmn->second->start();
        // first we need to check whether we need to ask the newly registered daemon
        //      to start the application
        if(mSystemRunning == true)
        {
            RPMS_DEBUG( DIR_CRTL_LOG, "STATE: sending start to registering daemon");
            // wait for this daemon to initialise
            while( ! dmn->second->isInitialised() )
            {
                usleep(100000); // just to stop the CPU being toasted
            }
            dmn->second->addToDaemonStateQueue("_START", 
                    SS_Distributor::instance()->getDistributionForNode(aHostName) );
        }
        // we now need to check if the newly registered daemon is running but we do not know about it
        //      if we are not running but the new daemon is running and we have not had any client interaction,
        //      then we are safe to presume that the system should be running
        else if( (mSystemRunning == false) && (aIsAppRunning == true) && (mClientInteraction == false) )
        {
            RPMS_DEBUG( DIR_CRTL_LOG, "STATE: sending start to all daemon");
            // wait for this daemon to initialise
            while( ! dmn->second->isInitialised() )
            {
                usleep(100000); // just to stop the CPU being toasted
            }
            startSystemComponents(); // have to issue a start across the board
        }
        else if( (mSystemRunning == false) && (aIsAppRunning == true) && (mClientInteraction == true) )
        {
            RPMS_DEBUG( DIR_CRTL_LOG, "STATE: sending stop to registering daemon");
            // wait for this daemon to initialise
            while( ! dmn->second->isInitialised() )
            {
                usleep(100000); // just to stop the CPU being toasted
            }
            dmn->second->addToDaemonStateQueue("_STOP", "stopArgument");
        }

        return true;
    CATCH;
}

///////////////////////////////////////////
bool SC_DirRemoteCtrls::daemonExists( const std::string &aHostName )
{
    bool ret = false;
    ctrlMap::iterator i = mDaemonCtrls->find( aHostName );
    if( i != mDaemonCtrls->end() )
    {
        ret = true;
    }
    return ret;
}

///////////////////////////////////////////
std::string SC_DirRemoteCtrls::getProcessStats()
{
    TRY(SC_DirRemoteCtrls::getProcessStats());
        cleanDeadDaemons(); // else we get some weird behaviour
        std::map<std::string, std::string> stats;
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            std::string s;
            try
            {
                s = i->second->getDaemonProcStats(); // gets the xml status from the daemon
            }
            catch( SX_Director &dex )
            {
                RPMS_INFO(DIR_CRTL_LOG, "Caught eception in getProcessStats():\n" +  dex.reason() );
                ++i;
                continue;
            }
            stats.insert( std::pair<std::string, std::string>(i->first, s) );
            ++i;
        }

        // now we create a single version of the xml combining all the xml we already have received
        SC_XMLProcStats final; // blank one to start with
        final.load();

        std::map<std::string, std::string>::iterator j = stats.begin();
        while( j != stats.end() )
        {
            SC_XMLProcStats tmp(j->second);
            tmp.load();
            final.addProcData( tmp.getProcStatsNode() );
            ++j;
        }
        return final.getSerializedDoc();
    CATCH;
}

///////////////////////////////////////////
std::string SC_DirRemoteCtrls::getDaemonStatus()
{
    TRY(SC_DirRemoteCtrls::getDaemonStatus());
        cleanDeadDaemons(); // else we get some weird behaviour
        std::string ret;
        std::map<std::string, std::string> statuses;
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            std::string status;
            try
            {
                status = i->second->getStatus(); // gets the xml status from the daemon
            }
            catch( SX_Director &dex )
            {
                RPMS_DEBUG(DIR_CRTL_LOG, "Caught eception in getDaemonStatus():\n" +  dex.reason() );
                ++i;
                continue;
            }
            statuses.insert(std::pair<std::string, std::string>(i->first, status));
            ++i;
        }

        // now we create a single version of the xml combining all the xml we already have received
        SC_XMLStatus final; // blank one to start with
        final.load();

        std::map<std::string, std::string>::iterator j = statuses.begin();
        while( j != statuses.end() )
        {
            SC_XMLStatus tmp(j->second);
            tmp.load();
            final.addDaemonData( tmp.getStatusNode() );
            ++j;
        }
        
        // now we have to add in the nodes that are not registered
        std::deque<std::string> nodes = mNodes->getNodeHosts();
        for( int i = 0; i < nodes.size(); ++i )
        {
            if( statuses.find(nodes.at(i)) == statuses.end() )
            {
                RPMS_DEBUG(DIR_CRTL_LOG, "Building dummy node for [" + nodes.at(i) + "]");
                ST_SPointer<SC_DaemonStatus> tmp = new SC_DaemonStatus( nodes.at(i), false, false, false );
                SC_XMLStatus dummy( tmp );
                dummy.load();
                final.addDaemonData( dummy.getStatusNode() );
            }
        }
        // now we add some system data to a new node
        final.addSystemData();
        return final.getSerializedDoc();
    CATCH;
}

///////////////////////////////////////////
void SC_DirRemoteCtrls::stopAll()
{
    TRY( SC_DirRemoteCtrls::stopAll() );
        SS_Distributor::instance()->stopActiveProcesses();
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            i->second->stop();
            ++i;
        }

        // only return when they have actually stopped
        i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            i->second->join();
            i->second = 0;
            ++i;
        }
        // now clean up
        cleanDeadDaemons();
    CATCH;
}

///////////////////////////////////////////
void SC_DirRemoteCtrls::checkDaemons()
{
    TRY(SC_DirRemoteCtrls::checkDaemons());
        cleanDeadDaemons(true);
    CATCH;
}

///////////////////////////////////////////
void SC_DirRemoteCtrls::cleanDeadDaemons( const bool aRunDistribution )
{
    TRY(SC_DirRemoteCtrls::cleanDeadDaemons(const bool aRunDistribution));
        ST_Guard<SA_Mutex> m( &mLock );
        // iterate through the map looking for any dead ones .. if one is 
        //      found then we remove it from the map
        RPMS_DEBUG( DIR_CRTL_LOG, "Cleaning up all the dead daemon controllers");
        bool cleaned = false;
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            if( i->second.empty() || !i->second->isRunning() )
            {
                RPMS_DEBUG( DIR_CRTL_LOG, "Found a dead daemon ctrl for hostname [" + i->first + "]");
                mDaemonCtrls->erase(i->first);
                cleaned = true;
            }
            ++i;
        }

        // now we have to run a load balance if we have had to clean up any daemons
        if( aRunDistribution && cleaned )
        {
            // run the load balancer
            reconfigureSystem(false);
        }
        
    CATCH;
}

///////////////////////////////////////////
bool SC_DirRemoteCtrls::startStopSystemComponents( const bool aIsToRun )
{
    TRY(SC_DirRemoteCtrls::startStopSystemComponents( const bool aIsToRun ));
        ST_Guard<SA_Mutex> m( &mLock ); // we don't want two calls clashing
        try
        {
            // first do a quick cleanup
            cleanDeadDaemons();

            mClientInteraction = true; //we have a definitive answer to the state
        
            return (aIsToRun?startSystemComponents():stopSystemComponents());
        }
        catch( SX_Exception &ex )
        {
            RPMS_WARN( DIR_CRTL_LOG, "Exception caught when trying to execute start/stop:" + ex.reason() );
        }
        return false;
    CATCH;
}

///////////////////////////////////////////
bool SC_DirRemoteCtrls::startSystemComponents()
{
    TRY(SC_DirRemoteCtrls::startSystemComponents());
        // we only do this for the start command .. else we may end up in trouble!
        if( mSystemRunning )
        {
            RPMS_INFO( DIR_CRTL_LOG, "Ignoring start command .. the system is already running" );
            return false;
        }
        // get a list of the registered nodes
        std::vector<std::string> registeredNodes;
        ctrlMap::iterator iter = mDaemonCtrls->begin();
        while( iter != mDaemonCtrls->end() )
        {
            registeredNodes.push_back(iter->first);
            ++iter;
        }
        // distribute all the components across the nodes
        std::map<std::string, std::string> distComps = SS_Distributor::instance()->distribute( registeredNodes, false );

        // now send the xml with the distributed config
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            i->second->addToDaemonStateQueue("_START", distComps.find(i->first)->second);
            ++i;
        }
        mSystemRunning = true;
        RPMS_INFO( DIR_CRTL_LOG, "Systen running state changed to [true]");
        return true;
    CATCH;
}


///////////////////////////////////////////
bool SC_DirRemoteCtrls::stopSystemComponents()
{
    TRY(SC_DirRemoteCtrls::stoptSystemComponents());
        SS_Distributor::instance()->stopActiveProcesses();
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            i->second->addToDaemonStateQueue("_STOP", "stopArgument"); // dummy arg
            ++i;
        }
        mSystemRunning = false;
        RPMS_INFO( DIR_CRTL_LOG, "Systen running state changed to [false]");
        SS_Distributor::instance()->resetDistribution();
        return true;
    CATCH;
}

///////////////////////////////////////////
bool SC_DirRemoteCtrls::reconfigureSystem(const bool aForce)
{
    TRY(SC_DirRemoteCtrls::reconfigureSystem( const bool aForce ));
        if( !mSystemRunning )
        {
            RPMS_INFO( DIR_CRTL_LOG, "Ignoring start command .. the system is not running .. so unable to reconfigure" );
            return false;
        }
        cleanDeadDaemons(); // else we get some weird behaviour
        // this is very similar to the start function except that it will 
        // get a list of the registered nodes
        std::vector<std::string> registeredNodes;
        ctrlMap::iterator iter = mDaemonCtrls->begin();
        while( iter != mDaemonCtrls->end() )
        {
            registeredNodes.push_back(iter->first);
            ++iter;
        }
        // distribute all the components across the nodes telling that this is a reconfigure
        std::map<std::string, std::string> distComps = SS_Distributor::instance()->distribute( registeredNodes, true, aForce );
        
        ctrlMap::iterator i = mDaemonCtrls->begin();
        while( i != mDaemonCtrls->end() )
        {
            i->second->addToDaemonStateQueue("_CONFIGURE", distComps.find(i->first)->second );
            ++i;
        }
        RPMS_INFO( DIR_CRTL_LOG, "New configuration details sent to the daemons");
        return true;
    CATCH;
}

///////////////////////////////////////////
bool SC_DirRemoteCtrls::restartSystemComponent( const std::string &aCompName, const bool aForceIt )
{
    TRY(SC_DirRemoteCtrls::restartSystemComponent( const std::string &aCompName, const bool aForceIt ));
        RPMS_INFO( DIR_CRTL_LOG, "Restarting [" + aCompName + "] " + (aForceIt?"with":"without") + " force");
        ctrlMap::iterator i = mDaemonCtrls->begin();
        std::string state = (aForceIt?"_RESTART":"_FAILSTART");
        while( i != mDaemonCtrls->end() )
        {
            i->second->addToDaemonStateQueue(state, aCompName);
            ++i;
        }
        return true;
    CATCH;
}


///////////////////////////////////////////
