//#############################
#include "ss_electionfactory.hpp"
#include "sc_staticelection.hpp"
#include "sc_bullyelection.hpp"
#include "sc_ringelection.hpp"
#include "sx_election.hpp"
#include "si_electionlog.hpp"
#include "sa_election.hpp"

#include <utilities/ss_ini.hpp>
#include <utilities/st_conversion.hpp>
#include <logging/si_logging.hpp>

using rpms::SS_ElectionFactory;
using rpms::ST_SPointer;
using rpms::SA_Election;

///////////////////////////////////////////
SS_ElectionFactory::SS_ElectionFactory()
    : mLock("SS_ElectionFactory"), mData(0), mElectable(true)
{
    TRY(SS_ElectionFactory::SS_ElectionFactory());
    CATCH;
}

///////////////////////////////////////////
SS_ElectionFactory::~SS_ElectionFactory()
{
    TRY(SS_ElectionFactory::~SS_ElectionFactory());
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SA_Election> SS_ElectionFactory::getElection( const bool aElectable )
{
    using rpms::convert;
    TRY(SS_ElectionFactory::getElection());
        if( !mData.empty() )
        {
            if( (!mElectable) && aElectable )
            {
                throw SX_Election("Trying to get an election with a different election criteria than previously defined current [" + convert<std::string>(mElectable) 
                        + "] requested [" + convert<std::string>(aElectable) + "]");
            }
            else
            {
                return mData;
            }
        }

        init(aElectable);
        return mData;
    CATCH;
}

///////////////////////////////////////////
void SS_ElectionFactory::init( const bool aElectable )
{
    TRY(SS_ElectionFactory::init( const bool aElectable ));
        mElectionType = SS_Ini::instance()->getString( "election", "election.type" );
        std::string xmlFile = SS_Ini::instance()->getString("directories", "config")
            + "/" + SS_Ini::instance()->getString("director", "director.xml.appconfig");
        RPMS_INFO(ELC_FAC_LOG, "Using election type [" + mElectionType + "]");
        if( "static" == mElectionType )
        {
            mData = new SC_StaticElection(xmlFile);
        }
        else if( "bully" == mElectionType ) 
        {
            mData = new SC_BullyElection(xmlFile);
        }
        else if( "ring" == mElectionType ) 
        {
            mData = new SC_RingElection(xmlFile);
        }
        else if( "auto" == mElectionType ) 
        {
            throw SX_Election("Automatic elections are not yet configured [" + mElectionType + "]");
        }
        else
        {
            throw SX_Election("Trying to create unknown election of type [" + mElectionType + "]");
        }

        // now we must initialise it
        mData->initialise();
        if(aElectable)
        {
            mData->initialiseElectable();
        }
        else
        {
            mElectable = false;
        }

    CATCH;
}

///////////////////////////////////////////
