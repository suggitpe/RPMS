//#############################
#include "sc_balanceddistribution.hpp"
#include "sc_procstatsthread.hpp"
#include "si_distributorlog.hpp"
#include "sx_distributor.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>
#include <tools/st_guard.hpp>

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>
#include <xmlutil/sc_xmlprocstats.hpp>

using rpms::SC_BalancedDistribution;
///////////////////////////////////////////

///////////////////////////////////////////
SC_BalancedDistribution::SC_BalancedDistribution( const std::string &aXmlFile )
    : SA_Distribution(aXmlFile), mStatsThread(SC_ProcStatsThread::create()), mCriteria(_UNKNOWN)
{
    TRY(SC_BalancedDistribution::SC_BalancedDistribution( const std::string &aXmlFile ));
    CATCH;
}

///////////////////////////////////////////
SC_BalancedDistribution::~SC_BalancedDistribution()
{
    TRY(SC_BalancedDistribution::~SC_BalancedDistribution());
    CATCH;
}

///////////////////////////////////////////
std::map<std::string, std::string> SC_BalancedDistribution::distributeComponents( 
        const std::vector<std::string> &aNodeList, const bool aRedistribution, const bool aForced )
{
    using rpms::convert;
    TRY(SC_BalancedDistribution::distributeComponents( const std::vector<std::string> &aNodeList, 
                const bool aRedistribution, const bool aForced ));
        
        if( mCriteria == _UNKNOWN )
        {
            std::string c = SS_Ini::instance()->getString("distributor", "distribution.criteria");
            if( c == "cpu" )
            {
                RPMS_DEBUG(DST_BAL_LOG, "Using CPU for process distribution criteria");
                mCriteria = _CPU;
            }
            else if( c == "memory" )
            {
                RPMS_DEBUG(DST_BAL_LOG, "Using MEM for process distribution criteria");
                mCriteria = _MEM;
            }
            else // to save system crashing
            {
                RPMS_WARN(DST_BAL_LOG, "Unknown distribution criteria, defaulting to CPU");
                mCriteria = _CPU;
            }
        }

        ST_Guard<SA_Mutex> m( &mLock );
        if( isDistributed() && !aRedistribution )
        {
            throw SX_Distributor(
                    "Trying to redistribute when the components have already been distributed");
        }

        if( !mStatsThread->isRunning() )
        {
            RPMS_INFO(DST_BAL_LOG, "Starting a statistics thread");
            mStatsThread->start();
        }

        // we need to let the thraed start up first before we know whether we have some stats
        //      to distribute with
        while( !mStatsThread->isInitialised() )
        {
            // stop cpu toasting
            usleep(1000);
        }

        std::map<std::string, ST_SPointer<SC_CompStat> > stats = mStatsThread->getStatistics();
        
        // when the proc stats thread starts it will load the last set of data.  But if 
        //      none exists we have to asume that either the data was scrubbed or this 
        //      is the first ever time we have done this .. thus use default distribution
        if( (!mStatsThread->isDataLoaded()) || stats.empty() )
        {
            RPMS_INFO(DST_BAL_LOG, "No statistics data loaded .. using round robin to distribute");
            return defaultDistributeComponents( aNodeList );
        }
        RPMS_INFO(DST_BAL_LOG, "Statistics data loaded .. preparing to distribute");

        mComponents->load();
        std::vector<std::string> comps = mComponents->getCompList(); // get component listing for distribution
        // special logic for redistribution .. just the preparate for the distribution
        if( aRedistribution )
        {
            prepareForRedistribution(comps, aForced);
        }
        // set up any additional config containers
        prepareNodeConfigContainers(aNodeList);
    
        int nodeNum =  aNodeList.size(); //get number of the active node list
        int compNum = comps.size(); // number of components to distribute
        if(!aRedistribution)
        {
            RPMS_TRACE( DST_BAL_LOG, "Distributing [" 
                + convert<std::string>(compNum) + "] components using \"balanced\" method across ["
                + convert<std::string>(nodeNum) +"] nodes");
        }
        else
        {
            RPMS_TRACE( DST_BAL_LOG, "Re-distributing [" 
                + convert<std::string>(compNum) + "] components using \"balanced\" method across ["
                + convert<std::string>(nodeNum) +"] nodes");
        }

        // now we can do the distribution
        // ******************************
        // FIRST: we need to identify the components that we have no data for and move them to a new map
        //      (the assumption here is that over time we will only ever have no data for new components)
        std::vector<std::string> noStats;
        std::vector<std::string>::iterator iter = comps.begin();
        while( iter != comps.end() )
        {
            if( stats.find(mComponents->getProcessConfig(*iter).getCompName()) == stats.end() ) // no stats for comp
            {
                noStats.push_back(*iter);
                comps.erase(iter);
                continue;
            }
            ++iter;
        }

        RPMS_TRACE( DST_BAL_LOG, "Of the [" + convert<std::string>(compNum) + "] components, ["
                + convert<std::string>(comps.size()) + "] have stats and ["
                + convert<std::string>(noStats.size()) + "] do not" );

        // SECOND we distribute based on the distribution type
        switch( mCriteria )
        {
            case _CPU:
                distributeForCpu(comps, aNodeList, stats);
                break;
            case _MEM:
                distributeForMemory(comps, aNodeList, stats);
                break;
            default:
                throw SX_Distributor("SC_BalancedDistribution: Unknown distribution method");
        }
        

        // THIRDLY: we need to distribute the rest of the compoents (the ones we have no data for)
        //      in a balanced fashion
        for( int i = 0; i < noStats.size(); ++i  )
        {
            RPMS_TRACE( DST_BAL_LOG, "Assigning (even)    [" + noStats.at(i) + "] to [" + aNodeList.at(i%nodeNum) + "]" );
            mCompConfig->find( aNodeList.at(i%nodeNum) )->second->addProcess( 
                        mComponents->getProcessConfig( noStats.at(i) ) );
        }

        // FINALLY: we then just have to serilise the end result
        mDistState->clear(); // clean up previous distributions
        for( int i = 0; i < nodeNum; ++i )
        {
            mDistState->insert( std::pair<std::string, std::string>(aNodeList.at(i), 
                            mCompConfig->find( aNodeList.at(i) )->second->getSerializedDoc()) );
        }

        setDistributed(true);
        RPMS_TRACE( DST_BAL_LOG, "Distribution complete for [" 
                + convert<std::string>(mDistState->size()) + "] node(s)" );
        return std::map<std::string, std::string>(*mDistState);
    CATCH;
}

///////////////////////////////////////////
void SC_BalancedDistribution::distributeForMemory(const std::vector<std::string> &aComps, 
        const std::vector<std::string> &aNodeList, 
        const std::map<std::string, ST_SPointer<SC_CompStat> > &aStats )
{
    TRY(SC_BalancedDistribution::distributeForMemory(const std::vector<std::string> &aComps , 
                const std::vector<std::string> &aNodeList, 
                const std::map<std::string, ST_SPointer<SC_CompStat> > &aStats));
        
        // FIRST: we need to take account of the existing distribution.  We have to assume that 
        //      we are redistributing so that the logic is consistent

        // this is assumed memory usage on the node based on stats
        std::map<std::string, long> nums; // node + maxrss
        for( int i = 0; i < aNodeList.size(); ++i )
        {
            nums.insert( std::pair<std::string, long>(aNodeList.at(i), 0) );
        }
        
        long min; // this will eventually be used as a minimum
        std::vector<std::string> cfg; // list of comps assigned to the node
        for( int i = 0; i < aNodeList.size(); ++i )
        {
            std::string n = aNodeList.at(i);
            // get the names of the already assigned process names
            cfg = mCompConfig->find(aNodeList.at(i))->second->getProcessNames();
            for( int j = 0; j < cfg.size(); ++j )
            {
                min = 0;
                // get the max rss for the process name
                std::string s = cfg.at(j);
                if( aStats.find(s) != aStats.end() )
                {
                    min = aStats.find(s)->second->getMaxRss();
                }

                // now we either create a new entry or update an existing one
                if( nums.find(n) != nums.end() ) // just in case
                {
                    nums.find(n)->second += min;
                }
            }
        }

        // SECOND: we need to distribute the compoents across the existing nodes based on the
        //      defined criteria (ie assign to the one with the least mem).
        std::string assignee = "";
        std::map<std::string, long>::iterator nodeIter = nums.begin();
        for( int i = 0; i < aComps.size(); ++i )
        {
            // find out the next in line for assigning a process
            nodeIter = nums.begin();
            assignee = nodeIter->first;
            min = nodeIter->second;
            ++nodeIter;
            while( nodeIter != nums.end())
            {
                if( nodeIter->second < min )
                {
                    assignee = nodeIter->first;
                    min = nodeIter->second;
                }
                ++nodeIter;
            }

            std::string c = aComps.at(i);
            RPMS_TRACE( DST_BAL_LOG, "Assigning (max RSS) [" + c + "] to [" + assignee + "]" );
            mCompConfig->find(assignee)->second->addProcess( mComponents->getProcessConfig(c) );
            nums.find(assignee)->second += aStats.find(mComponents->getProcessConfig(c).getCompName())->second->getMaxRss();
        }
        
    CATCH;
}

///////////////////////////////////////////
void SC_BalancedDistribution::distributeForCpu(const std::vector<std::string> &aComps, 
        const std::vector<std::string> &aNodeList, 
        const std::map<std::string, ST_SPointer<SC_CompStat> > &aStats )
{
    TRY(SC_BalancedDistribution::distributeForCpu(const std::vector<std::string> &aComps, 
                const std::vector<std::string> &aNodeList, 
                const std::map<std::string, ST_SPointer<SC_CompStat> > &aStats ));
        // FIRST: we need to take account of the existing distribution.  We have to assume that 
        //      we are redistributing so that the logic is consistent

        // this is assumed memory usage on the node based on stats
        std::map<std::string, double> nums; // node + cpuavg
        for( int i = 0; i < aNodeList.size(); ++i )
        {
            nums.insert( std::pair<std::string, double>(aNodeList.at(i), 0) );
        }
        
        double min; // this will eventually be used as a minimum
        std::vector<std::string> cfg; // list of comps assigned to the node
        for( int i = 0; i < aNodeList.size(); ++i )
        {
            std::string n = aNodeList.at(i);
            // get the names of the already assigned process names
            cfg = mCompConfig->find(aNodeList.at(i))->second->getProcessNames();
            for( int j = 0; j < cfg.size(); ++j )
            {
                min = 0;
                // get the cpu avg for the process name
                std::string s = cfg.at(j);
                if( aStats.find(s) != aStats.end() )
                {
                    min = aStats.find(s)->second->getCpuAvg();
                }

                // now we either create a new entry or update an existing one
                if( nums.find(n) != nums.end() ) // just in case
                {
                    nums.find(n)->second += min;
                }
            }
        }

        // SECOND: we need to distribute the compoents across the existing nodes based on the
        //      defined criteria (ie assign to the one with the least mem).
        std::string assignee = "";
        std::map<std::string, double>::iterator nodeIter = nums.begin();
        for( int i = 0; i < aComps.size(); ++i )
        {
            // find out the next in line for assigning a process
            nodeIter = nums.begin();
            assignee = nodeIter->first;
            min = nodeIter->second;
            ++nodeIter;
            while( nodeIter != nums.end())
            {
                if( nodeIter->second < min )
                {
                    assignee = nodeIter->first;
                    min = nodeIter->second;
                }
                ++nodeIter;
            }

            std::string c = aComps.at(i);
            RPMS_TRACE( DST_BAL_LOG, "Assigning (avg CPU) [" + c + "] to [" + assignee + "]" );
            mCompConfig->find(assignee)->second->addProcess( mComponents->getProcessConfig(c) );
            nums.find(assignee)->second += aStats.find(mComponents->getProcessConfig(c).getCompName())->second->getCpuAvg();
        }
        
    CATCH;
}

///////////////////////////////////////////
void SC_BalancedDistribution::stop()
{
    TRY(SC_BalancedDistribution::stop());
        RPMS_INFO(DST_BAL_LOG, "Stopping statistics thread");
        mStatsThread->stop();
        mStatsThread->join();
    CATCH;
}

///////////////////////////////////////////
