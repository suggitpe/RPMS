//#############################
#include "sa_distribution.hpp"
#include "si_distributorlog.hpp"
#include "sx_distributor.hpp"

#include <si_macros.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>
#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>

using rpms::SA_Distribution;
///////////////////////////////////////////


///////////////////////////////////////////
SA_Distribution::SA_Distribution( const std::string &aXmlFile )
    : mDistributed(false), mLock("Distribution_Lock")
{
    TRY(SA_Distribution::SA_Distribution());
        mComponents = new SC_XMLSysConfig( aXmlFile );
        mDistState = new std::map<std::string, std::string>;
        mCompConfig = new std::map< std::string, ST_SPointer<SC_XMLDaemonConfig> >;
    CATCH;
}

///////////////////////////////////////////
SA_Distribution::~SA_Distribution()
{
    TRY(SA_Distribution::~SA_Distribution());
        delete mDistState;
        delete mCompConfig;
    CATCH;
}

///////////////////////////////////////////
void SA_Distribution::initialise()
{
    TRY(SA_Distribution::initialise());
        // nothing to be done here
    CATCH;
}

///////////////////////////////////////////
void SA_Distribution::reset()
{
    TRY(SA_Distribution::reset());
        RPMS_TRACE( DST_BAS_LOG, "Resetting components" );
        mDistState->clear();
        mCompConfig->clear();
        mDistributed = false;
    CATCH;
}

///////////////////////////////////////////
void SA_Distribution::prepareForRedistribution( std::vector<std::string> &aComps, const bool aForced )
{
    TRY(SA_Distribution::prepareForRedistribution( std::vector<std::string> &aComps ));
        RPMS_DEBUG( DST_BAS_LOG, "Preparing for a re-distribution");
        // first we need to clean out all of the distributed components that 
        //      are not moveable from the list to distribute
        //RPMS_TRACE( DST_BAS_LOG, "    - removing non moveable components from the distribution list");
        if(!aForced)
        {
            std::vector<std::string>::iterator iter = aComps.begin();
            while( iter != aComps.end() )
            {
                if( !mComponents->getProcessConfig(*iter).isMoveable() )
                {
                    // we want to remove al the processes that are not moveable
                    aComps.erase(iter);
                    continue;
                }
                ++iter;
            }
        }
        
        // then we need to clean out all the previously distributed moveable 
        //      components from the end list
        //RPMS_TRACE( DST_BAS_LOG, "    - removing moveable components from the distributed list");
        if(aForced)
        {
            mCompConfig->clear();
        }
        else
        {
            std::map< std::string, ST_SPointer<SC_XMLDaemonConfig> >::iterator i = mCompConfig->begin();
            while( i != mCompConfig->end() )
            {
                i->second->removeMoveableProcesses();
                ++i;
            }
        }
    CATCH;
}

///////////////////////////////////////////
void SA_Distribution::prepareNodeConfigContainers( const std::vector<std::string> &aNodeList )
{
    TRY(SA_Distribution::prepareNodeConfigContainers( const std::vector<std::string> &aNodeList ));
        int nodeNum =  aNodeList.size();
        for( int i =0; i < nodeNum; ++i )
        {
            if( mCompConfig->find( aNodeList.at(i) ) == mCompConfig->end() )
            {
                //RPMS_TRACE( DST_BAS_LOG, "Adding new node config for [" + aNodeList.at(i) + "]" );
                mCompConfig->insert( std::pair< std::string, ST_SPointer<SC_XMLDaemonConfig> >( 
                        aNodeList.at(i), new SC_XMLDaemonConfig() ) );
                mCompConfig->find( aNodeList.at(i) )->second->load();
            }
        }
    CATCH;
}

///////////////////////////////////////////
std::string SA_Distribution::getDistributionForNode( const std::string &aNodeName )
{
    TRY(SA_Distribution::getDistributionForNode( const std::string &aNodeName ));
        // here we are just going to search for the already created xml for the node 
        //      and return if found else we create a dummy xml
        RPMS_TRACE( DST_BAS_LOG, "Getting node distribution for node [" + aNodeName + "]" );
        std::map<std::string, std::string>::iterator i = mDistState->find(aNodeName);
        if( i == mDistState->end() ) // not found
        {
            RPMS_TRACE( DST_BAS_LOG, "Could not find a configuration for node [" + aNodeName 
                    + "], so creating a dummy node config" );
            SC_XMLDaemonConfig x;
            x.load();
            return x.createDummyDaemonConfig();
        }
        return i->second;
    CATCH;
}

///////////////////////////////////////////
std::map<std::string, std::string> SA_Distribution::defaultDistributeComponents( const std::vector<std::string> &aNodeList )
{
    using rpms::convert;
    TRY(std::string> SA_Distribution::defaultDistributeComponents( const std::vector<std::string> &aNodeList ));
        RPMS_TRACE( DST_BAS_LOG, "*********************");
        if( isDistributed() )
        {
            throw SX_Distributor(
                    "SA_Distribution: Trying to redistribute when the components have already been distributed");
        }
        
        mComponents->load();
        std::vector<std::string> comps = mComponents->getCompList();

        // create the containers for the procesess
        prepareNodeConfigContainers(aNodeList);

        int nodeNum =  aNodeList.size();
        int compNum = comps.size();
        RPMS_TRACE( DST_BAS_LOG, "Distributing [" 
                + rpms::convert<std::string>(compNum) + "] components using default method across ["
                + rpms::convert<std::string>(nodeNum) +"] nodes");


        // assign the components to the relevant node
        // In this instance we just do a simple round robin approach to the distribution
        for( int i = 0; i < compNum; ++i  )
        {
            mCompConfig->find( aNodeList.at(i%nodeNum) )->second->addProcess( 
                        mComponents->getProcessConfig( comps.at(i) ) );
        }

        //  create the end result of the distribution in xml form so that it can be sent to the daemons
        for( int i = 0; i < nodeNum; ++i )
        {
            mDistState->insert( std::pair<std::string, std::string>(aNodeList.at(i), 
                            mCompConfig->find( aNodeList.at(i) )->second->getSerializedDoc()) );
        }
        setDistributed(true);
        RPMS_TRACE( DST_BAS_LOG, "Distribution complete for [" + rpms::convert<std::string>(mDistState->size()) + "] node" );
        return std::map<std::string, std::string>(*mDistState);
    CATCH;
}

///////////////////////////////////////////
