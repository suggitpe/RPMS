//#############################
#include "sc_processmanager.hpp"
#include "sc_processwrapperthread.hpp"
#include "sx_daemon.hpp"
#include "si_daemonlog.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>
#include <logging/si_logging.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>
#include <xmlutil/sc_xmlstatus.hpp>
#include <xmlutil/sc_xmlprocstats.hpp>
#include <utilities/sc_processinfo.hpp>

#include <deque>

using rpms::SC_ProcessManager;

///////////////////////////////////////////
SC_ProcessManager::SC_ProcessManager()
    : mLock("ProcessManager lock")
{
    TRY(SC_ProcessManager::SC_ProcessManager());
        mProcs = new procMap;
    CATCH;
}

///////////////////////////////////////////
SC_ProcessManager::~SC_ProcessManager()
{
    TRY(SC_ProcessManager::~SC_ProcessManager());
        delete mProcs;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessManager::startProcesses( const std::string &aProcessConfig )
{
    TRY(SC_ProcessManager::startProcesses( const std::string &aProcessConfig ));
        mLock.lock();
        RPMS_DEBUG( DMN_PRC_MAN, "Starting processes ... parsing xml");
        SC_XMLDaemonConfig xml(aProcessConfig);
        xml.load();
        RPMS_TRACE( DMN_PRC_MAN, "Daemon config xml loaded");
        std::vector<ST_SPointer<SC_ProcessConfig> > procs = xml.getProcesses();
        for( int i = 0; i < procs.size(); ++i )
        {
            mProcs->insert( std::pair<std::string, ST_SPointer<SC_ProcessWrapperThread> >(
                        procs.at(i)->getInternalName(),
                        SC_ProcessWrapperThread::create("ProcessWrapper_" 
                            + procs.at(i)->getInternalName(), *procs.at(i)) ) );
        }
        procMap::iterator iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            RPMS_TRACE( DMN_PRC_MAN, "Starting process [" + iter->first + "]");
            iter->second->start();
            ++iter;
        }
        RPMS_INFO( DMN_PRC_MAN, "All processes started");
        mLock.unlock();
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessManager::adaptToReconfiguration ( const std::string &aProcessConfig )
{
    TRY(SC_ProcessManager::adaptToReconfiguration ( const std::string &aProcessConfig ));
        mLock.lock();
        RPMS_DEBUG( DMN_PRC_MAN, "Reconfiguring daemon procesess ... parsing xml");
        SC_XMLDaemonConfig xml(aProcessConfig);
        xml.load();
        RPMS_TRACE( DMN_PRC_MAN, "Daemon config xml loaded");
        std::stack<std::string> res;

        // FIRST: set all the proceses to unchecked .. this allows us to analyse the proceses in this process
        procMap::iterator iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            iter->second->setReconfUnChecked();
            ++iter;
        }
        
        // SECOND: go through the list of proceses in the new config
        //      - if it is a new one then we start and set to checked
        //      - if we already have one running (and there is no difference in the configuration of it), then we set to checked
        //      - if we already have one but there is a difference in the config we stop the old one and restart a new one
        std::vector<ST_SPointer<SC_ProcessConfig> > procs = xml.getProcesses();
        std::map<std::string, ST_SPointer<SC_ProcessWrapperThread> >::iterator procIter;
        for(int i = 0; i < procs.size(); ++i)
        {
            // new procs
            procIter = mProcs->find( procs.at(i)->getInternalName() );
            if( procIter == mProcs->end())
            {
                // we do not have this process in the system at the moment
                mProcs->insert( std::pair<std::string, ST_SPointer<SC_ProcessWrapperThread> >(
                            procs.at(i)->getInternalName(),
                            SC_ProcessWrapperThread::create("ProcessWrapper_"
                            + procs.at(i)->getInternalName(), *procs.at(i)) ) );
                mProcs->find( procs.at(i)->getInternalName() )->second->start();
                mProcs->find( procs.at(i)->getInternalName() )->second->setReconfChecked();
                res.push( procs.at(i)->getInternalName() + " STARTED NEW");
            }
            // no changes
            else if( procIter->second->getProcData() == procs.at(i) )
            {
                // we have the process and there is no change to the process .. so we do nothing
                procIter->second->setReconfChecked();
                res.push( procs.at(i)->getInternalName() + " NO CHANGE" );
            }
            // exists but has changed
            else if( procIter->second->getProcData() != procs.at(i) )
            {
                procIter->second->stop();
                procIter->second->join();
                procIter->second = SC_ProcessWrapperThread::create( "ProcessWrapper_"
                                + procs.at(i)->getInternalName(), 
                                *(procs.at(i)) );
                procIter->second->start();
                procIter->second->setReconfChecked();
                res.push( procs.at(i)->getInternalName() + " RESTARTED" );
            }
            // panic!!!
            else
            {
                // we should not ever get here
                throw SX_Daemon("During reconfiguration we have reached an unknown scenario ... this should be reported");
            }
        }

        // THIRD: we stop all the proceses that have a false checked status (these will be the ones that should not run)
        RPMS_DEBUG(DMN_PRC_MAN, "Looking for processes that should no longer be running on this node");
        iter = mProcs->begin();
        while( iter != mProcs->end() ) // stop loop
        {
            if( !iter->second->hasBeenReconfChecked() )
            {
                iter->second->stop();
                res.push( iter->second->getProcData()->getInternalName() + " STOPPED" );
            }
            ++iter;
        }
        
        iter = mProcs->begin();
        while( iter != mProcs->end() ) // join loop
        {
            if( (!iter->second->hasBeenReconfChecked()) && iter->second->isRunning() )
            {
                iter->second->join();
            }
            ++iter;
        }

        iter = mProcs->begin();
        while( iter != mProcs->end() ) // erase loop
        {
            if( !iter->second->hasBeenReconfChecked() )
            {
                mProcs->erase(iter);
            }
            ++iter;
        }

        RPMS_DEBUG( DMN_PRC_MAN, "Reconfiguration procesess completed ...");
        while( !res.empty() )
        {
            RPMS_DEBUG(DMN_PRC_MAN, "    - " + res.top() );
            res.pop();
        }
        mLock.unlock();
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessManager::stopAllProcesses()
{
    TRY(SC_ProcessManager::stopAllProcesses());
        mLock.lock();
        RPMS_INFO( DMN_PRC_MAN, "Stopping all running processes");
        procMap::iterator iter = mProcs->begin();
        // first we need to send a stop to all the processes
        while( iter != mProcs->end() )
        {
            if( iter->second->isRunning() )
            {
                RPMS_DEBUG( DMN_PRC_MAN, "Stopping process [" + iter->first + "]");
                iter->second->stop();
            }
            ++iter;
        }

        RPMS_DEBUG( DMN_PRC_MAN, "Waiting for all processes to complete work");
        // then we have to wait for them to actually finish what they were doing
        iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            if( iter->second->isRunning() )
            {
                iter->second->join();
            }
            ++iter;
        }
        // now we clean up after ourselves making sure that the procMap is empty
        mProcs->clear();

        mLock.unlock();
        RPMS_INFO( DMN_PRC_MAN, "All running processes stopped");
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessManager::addCompStatuses( const ST_SPointer<SC_DaemonStatus> &aStatus )
{
    TRY(SC_ProcessManager::addCompStatuses( const ST_SPointer<SC_DaemonStatus> &aStatus ));
        procMap::iterator iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            aStatus->addComponentStatus( iter->second->getCompStatus() );
            ++iter;
        }
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessManager::addProcessStatistics( const ST_SPointer<SC_ProcStats> &aStats )
{
    TRY(SC_ProcessManager::addProcessStatistics( const ST_SPointer<SC_ProcStats> &aStats ));
        procMap::iterator iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            aStats->addProcStats( iter->second->getProcStats() );
            ++iter;
        }
    CATCH;
}


///////////////////////////////////////////
void SC_ProcessManager::restartProcess( const std::string &aProcessName, const bool aHardRestart )
{
    using rpms::convert;
    TRY(SC_ProcessManager::restartProcess( const std::string &aProcessName, const bool aHardRestart ));
        RPMS_TRACE( DMN_PRC_MAN, "Looking for processes called [" + aProcessName + "] for a [" + (aHardRestart?"hard":"soft") + "] restart");
        // This bit is ripe for popping into a conversion function but it is likely to cause some level of pain with linking
        std::string upper;
        std::string::const_iterator i = aProcessName.begin();
        while(i != aProcessName.end())
        {
            upper += std::toupper(*i);
            ++i;
        }

        // first stop all the ones we are interested in
        int count = 0;
        procMap::iterator iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            if( upper == "ALL" || aProcessName == iter->second->getProcData()->getCompName() )
            {
                if( aHardRestart && iter->second->isRunning() )
                {
                    iter->second->stop();
                    ++count;
                }
            }
            ++iter;
        }
        RPMS_TRACE( DMN_PRC_MAN, "Stopped [" + convert<std::string>(count) + "] [" + aProcessName + "] procesess");
        
        // then wait for them to dies correctly (join them)
        iter = mProcs->begin();
        count = 0;
        while( iter != mProcs->end() )
        {
            if( upper == "ALL" || aProcessName == iter->second->getProcData()->getCompName() )
            {
                if( aHardRestart && iter->second->isRunning() )
                {
                    iter->second->join();
                    ++count;
                }
            }
            ++iter;
        }
        RPMS_TRACE( DMN_PRC_MAN, "Joined on [" + convert<std::string>(count) + "] [" + aProcessName + "] procesess");

        // now we know we can safely remove them and restart them again
        iter = mProcs->begin();
        count = 0;
        while( iter != mProcs->end() )
        {
            if( upper == "ALL" || aProcessName == iter->second->getProcData()->getCompName() )
            {
                // at this point we have done all the killing spree so we just want to start 
                //      any processes that are not running
                if( ! iter->second->isRunning() )
                {
                    // replace the old object
                    iter->second = SC_ProcessWrapperThread::create( "ProcessWrapper_"
                                    + iter->second->getProcData()->getInternalName(), 
                                    *(iter->second->getProcData()));
                    iter->second->start();
                    ++count;
                }
            }
            ++iter;
        }
        
        RPMS_INFO( DMN_PRC_MAN, "Re-Started [" + convert<std::string>(count) + "] [" + aProcessName + "] process(es)");
    CATCH;
}


// ###################### HIDDEN
///////////////////////////////////////////
SC_ProcessManager::SC_ProcessManager( const SC_ProcessManager &aRhs )
    : mLock("dummy lock name")
{
    TRY(SC_ProcessManager::SC_ProcessManager( const SC_ProcessManager &aRhs ));
        throw SX_Daemon("copy ctor not allowed in SC_ProcessManager");
    CATCH;
}

///////////////////////////////////////////
SC_ProcessManager& SC_ProcessManager::operator=( const SC_ProcessManager &aRhs )
{
    TRY(SC_ProcessManager::operator=( const SC_ProcessManager &aRhs ));
        throw SX_Daemon("assignment operator not allowed in SC_ProcessManager");
        return *this;
    CATCH;
}

///////////////////////////////////////////
