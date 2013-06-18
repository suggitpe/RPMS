//#############################
#include "sc_roundrobindistribution.hpp"
#include "si_distributorlog.hpp"
#include "sx_distributor.hpp"

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>

#include <si_macros.hpp>
#include <tools/st_guard.hpp>
#include <utilities/st_conversion.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>

using rpms::SC_RoundRobinDistribution;
///////////////////////////////////////////
SC_RoundRobinDistribution::SC_RoundRobinDistribution( const std::string &aXmlFile )
    : SA_Distribution(aXmlFile)
{
    TRY( SC_RoundRobinDistribution::SC_RoundRobinDistribution( const std::string &aXmlFile ));
    CATCH;
}

///////////////////////////////////////////
SC_RoundRobinDistribution::~SC_RoundRobinDistribution()
{
    TRY(SC_RoundRobinDistribution::~SC_RoundRobinDistribution());
    CATCH;
}

///////////////////////////////////////////
std::map<std::string, std::string> SC_RoundRobinDistribution::distributeComponents( 
        const std::vector<std::string> &aNodeList, const bool aRedistribution, const bool aForced )
{
    using rpms::convert;
    TRY(SC_RoundRobinDistribution::distributeComponents( const std::vector<std::string> &aNodeList, 
                const bool aRedistribution, const bool aForced ));
        ST_Guard<SA_Mutex> m( &mLock );
        if( isDistributed() && !aRedistribution )
        {
            throw SX_Distributor(
                    "Trying to redistribute when the components have already been distributed");
        }

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
            RPMS_TRACE( DST_RRB_LOG, "Distributing [" 
                + convert<std::string>(compNum) + "] components using \"round robin\" method across ["
                + convert<std::string>(nodeNum) +"] nodes");
        }
        else
        {
            RPMS_TRACE( DST_RRB_LOG, "Re-distributing [" 
                + convert<std::string>(compNum) + "] components using \"round robin\" method across ["
                + convert<std::string>(nodeNum) +"] nodes");
        }

        // here we do the actual distribution
        // ******************************
        // FIRST: get the number of allocated objects to each node and build up an allocation structure 
        //      so we know where we have allocated
        std::map<std::string, int> nums;
        int num = 0;
        int min = 0;
        for( int i = 0; i < nodeNum; ++i )
        {
            num = mCompConfig->find(aNodeList.at(i))->second->getNumProcs();
            if( num < min )
                min = num;
            nums.insert( std::pair<std::string, int>(aNodeList.at(i), 
                        mCompConfig->find(aNodeList.at(i))->second->getNumProcs()) );
        }

        // SECOND: then we assign the components to the relevant node based on how many they already have
        std::map<std::string, int>::iterator iter = nums.begin();
        for( int i = 0; i < compNum; )
        {
            while( iter != nums.end() )
            {
                if( iter->second == min )
                {
                    //RPMS_TRACE( DST_RRB_LOG, "allocating [" + comps.at(i) + "] to [" + iter->first + "]");
                    mCompConfig->find( iter->first )->second->addProcess( mComponents->getProcessConfig( comps.at(i) ) );
                    iter->second += 1;
                    ++i;
                }
                ++iter;
                if( i >= compNum )
                {
                    break;
                }
            }
            iter = nums.begin();
            ++min;
        }

        // ******************************
        // create the end result of the distribution in xml form so that it can be sent to the daemons
        mDistState->clear(); // this will clean up anything that we have already distributed
        for( int i = 0; i < nodeNum; ++i )
        {
            mDistState->insert( std::pair<std::string, std::string>(aNodeList.at(i), 
                            mCompConfig->find( aNodeList.at(i) )->second->getSerializedDoc()) );
        }


        // now we just have to send the data
        setDistributed(true);
        RPMS_TRACE( DST_RRB_LOG, "Distribution complete for [" 
                + convert<std::string>(mDistState->size()) + "] node(s)" );
        return std::map<std::string, std::string>(*mDistState);
        
    CATCH;
}

///////////////////////////////////////////
