//#############################
#include "ss_distributor.hpp"
#include "sx_distributor.hpp"
#include "si_distributorlog.hpp"
#include "sc_roundrobindistribution.hpp"
#include "sc_balanceddistribution.hpp"

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>

using rpms::SS_Distributor;

///////////////////////////////////////////
SS_Distributor::SS_Distributor()
    : mInitialised(false), mDist(0)
{
    TRY(SS_Distributor::SS_Distributor());
    CATCH;
}

///////////////////////////////////////////
SS_Distributor::~SS_Distributor()
{
    TRY(SS_Distributor::~SS_Distributor());
    CATCH;
}

///////////////////////////////////////////
void SS_Distributor::initialise()
{
    TRY(SS_Distributor::initialise());
        if( mInitialised )
        {
            mDist = 0;
        }
        // load up the system configuration
        std::string xmlFile = SS_Ini::instance()->getString("directories", "config")
            + "/" + SS_Ini::instance()->getString("director", "director.xml.appconfig");
        // now load up the distribution method
        std::string method = SS_Ini::instance()->getString("distributor", "distribution.method");
        if( "roundrobin" == method )
        {
            RPMS_TRACE( DST_LOG, "Using Round-Robin distribution pattern");
            mDist = new SC_RoundRobinDistribution( xmlFile );
        }
        else if( "balanced" == method )
        {
            RPMS_TRACE( DST_LOG, "Using Balanced distribution pattern");
            mDist = new SC_BalancedDistribution( xmlFile );
        }
        else
        {
            throw SX_Distributor("Unknown distribution method configured [" + method + "]");
        }
        mDist->initialise();
        mInitialised = true;
    CATCH;
}

///////////////////////////////////////////
std::map<std::string, std::string> SS_Distributor::distribute( const std::vector<std::string> &aNodeList, const bool aRedistribution, const bool aForced )
{
    TRY(SS_Distributor::distribute( const std::vector<std::string> &aNodeList, const bool aRedistribution, const bool aForced ));
        return mDist->distributeComponents( aNodeList, aRedistribution, aForced );
    CATCH;
}

///////////////////////////////////////////
std::string SS_Distributor::getDistributionForNode( const std::string &aNodeName )
{
    TRY(SS_Distributor::getDistributionForNode( const std::string &aNodeName ));
        return mDist->getDistributionForNode(aNodeName);
    CATCH;
}

///////////////////////////////////////////
void SS_Distributor::resetDistribution()
{
    TRY(SS_Distributor::resetDistribution());
        mDist->reset();
    CATCH;
}

///////////////////////////////////////////
void SS_Distributor::stopActiveProcesses()
{
    TRY(SS_Distributor::stopActiveProcesses());
        if( ! mDist.empty() )
        {
            mDist->stop();
        }
    CATCH;
}

// *************************************************************************
// DEAD METHODS .. HIDEN FROM VIEW
///////////////////////////////////////////
SS_Distributor::SS_Distributor( const SS_Distributor &aRhs )
{
    TRY(SS_Distributor::SS_Distributor( const SS_Distributor &aRhs ));
        throw SX_Distributor("copy constructor has been disabled for this class");
    CATCH;
}

///////////////////////////////////////////
SS_Distributor& SS_Distributor::operator=( const SS_Distributor &aRhs )
{
    TRY(SS_Distributor::operator=( const SS_Distributor &aRhs ));
        throw SX_Distributor("assignment operator has been disabled for this class");
    CATCH;
}


///////////////////////////////////////////
