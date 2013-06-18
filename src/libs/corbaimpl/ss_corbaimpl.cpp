//#############################
#include "ss_corbaimpl.hpp"
#include "si_corbaimpl.hpp"
#include "sx_corbaimpl.hpp"

#include <logging/si_logging.hpp>
#include <sockets/sc_inetaddress.hpp>
#include <utilities/ss_ini.hpp>
#include <si_macros.hpp>

using rpms::SS_CorbaImpl;

///////////////////////////////////////////
SS_CorbaImpl::SS_CorbaImpl()
    : mOrbInitialised(false), mCanSend(false), mCanRecv(false), SA_ThreadBase("CorbaImpl")
{
    TRY(SS_CorbaImpl::SS_CorbaImpl());
    CATCH;
}

///////////////////////////////////////////
SS_CorbaImpl::~SS_CorbaImpl()
{
    TRY(SS_CorbaImpl::~SS_CorbaImpl());
        mOrb->shutdown(true);
    CATCH;
}
///////////////////////////////////////////
void SS_CorbaImpl::run()
{
    TRY(SS_CorbaImpl::run());
        runOrb();
    CATCH;
}

///////////////////////////////////////////
void SS_CorbaImpl::stop()
{
    TRY(SS_CorbaImpl::stop());
        RPMS_DEBUG(CBA_SS_LOG, "Shutting down ORB");
        mOrb->shutdown(true);
    CATCH;
}

///////////////////////////////////////////
void SS_CorbaImpl::initOrb( const bool aCanSend, const bool aCanRecv )
{
    TRY(SS_CorbaImpl::initOrb( const bool aCanSend, const bool aCanRecv ));
        bool orbDebug = SS_Ini::instance()->getBool("CORBA", "corba.orb.debug");
        // work out how many args we need to deal with
        int numArgs = 1;
        if(aCanSend)
        {
            mCanSend = true;
        }
        else
        {
            numArgs += 1; // -ORBNoIIOPProxy
        }

        if(aCanRecv)
        {
            mCanRecv = true;
            numArgs += 2; // -ORBIIOPAddr + binding
        }
        else
        {
            numArgs += 1; // -ORBNoIIOPServer
        }
    
        if( orbDebug )
        {
            numArgs += 4; // ORBDebug
        }

        char *orbArgs[numArgs];
        orbArgs[0] = "Dummy";
        int i = 1;

        std::string debuglevel = SS_Ini::instance()->getString("CORBA", "corba.orb.debuglevel");
        if( orbDebug )
        {
            orbArgs[i++] = "-ORBDebug";
            orbArgs[i++] = "All";
            orbArgs[i++] = "-ORBDebugLevel";
            orbArgs[i++] = const_cast<char *>(debuglevel.c_str());
        }

        if( !aCanSend )
        {
            orbArgs[i++] = "-ORBNoIIOPProxy";
        }

        // must decl out of if as the c_str val will be reassigned my mem mgr
        std::string binding;
        if(aCanRecv)
        {
            orbArgs[i++] = "-ORBIIOPAddr";
            binding = std::string("inet:") + SC_InetAddress::getLocalHostName() + ":" 
                + SS_Ini::instance()->getString("CORBA", "corba.orb.port");
            orbArgs[i++] = const_cast<char *>(binding.c_str());
        }
        else
        {
            orbArgs[i++] = "-ORBNoIIOPServer";
        }
    
        // debug the ORB init params
        std::string orbCfg = "";
        for( int j = 1; j < numArgs; ++j )
        {
            if(j > 1) orbCfg += " ";
            orbCfg += orbArgs[j];
        }

        RPMS_DEBUG( CBA_SS_LOG, "Initialising CORBA orb with [" + orbCfg + "]" );
        try
        {
            mOrb = CORBA::ORB_init( numArgs, orbArgs );
        }
        catch( CORBA::Exception &ex  )
        {
            ex._print(std::cout);
            throw SX_CorbaImpl("Whilst initialising the orb a CORBA exception was thrown");
        }
        RPMS_TRACE( CBA_SS_LOG, "CORBA orb initialised" );
        mOrbInitialised = true;
    CATCH;
}

///////////////////////////////////////////
void SS_CorbaImpl::initPoa( const PortableServer::Servant &aServant )
{
    TRY(SS_CorbaImpl::initPoa( const PortableServer::Servant &aServant ));
        RPMS_DEBUG( CBA_SS_LOG, "Initialising POA Manager");
        mPoa = PortableServer::POA::_narrow( mOrb->resolve_initial_references("RootPOA") );
        PortableServer::POAManager_var poaMgr = mPoa->the_POAManager();
        PortableServer::ObjectId_var oid = mPoa->activate_object(aServant);
        poaMgr->activate();
        RPMS_TRACE( CBA_SS_LOG, "POA manager initialised with servant" );
    CATCH;
}

///////////////////////////////////////////
void SS_CorbaImpl::deRegisterPoa( const PortableServer::Servant &aServant )
{
    TRY(SS_CorbaImpl::deRegisterPoa( const PortableServer::Servant &aServant ));
        try
        {
            PortableServer::ObjectId_var id = mPoa->servant_to_id( aServant );
            mPoa->deactivate_object(*id);
        }
        catch( PortableServer::POA::WrongPolicy_catch &wp )
        {
            RPMS_ERROR(CBA_SS_LOG, "Caught POA::WrongPolicy exception when calling deactivate_object");
            throw SX_CorbaImpl( "Caught POA::WrongPolicy exception when calling deactivate_object" );
        }
        catch( PortableServer::POA::ObjectNotActive_catch &ona )
        {
            RPMS_ERROR(CBA_SS_LOG, "Caught POA::ObjectNotActive exception when calling deactivate_object");
            throw SX_CorbaImpl( "Caught POA::ObjectNotActive exception when calling deactivate_object" );
        }
    CATCH;
}

///////////////////////////////////////////
void SS_CorbaImpl::runOrb()
{
    TRY(SS_CorbaImpl::runOrb());
        RPMS_DEBUG( CBA_SS_LOG, "Starting CORBA ORB" );
        return mOrb->run();
    CATCH;
}

///////////////////////////////////////////
CORBA::Object_var SS_CorbaImpl::getRemoteObject( const std::string &aRemoteHost, const std::string &aRemotePort, const std::string &aRemoteName )
{
    TRY(SS_CorbaImpl::getRemoteObject( const std::string &aRemoteHost, const std::string &aRemotePort, const std::string &aRemoteName ));
        RPMS_DEBUG( CBA_SS_LOG, "Binding to remote object [" + aRemoteName + "] on [" 
            + aRemoteHost + ":" + aRemotePort + "]" );
        if( !mCanSend )
        {
            throw SX_CorbaImpl("ORB has not been initialsied to connect to remote objects");
        }

        std::string binding = (std::string("inet:") + aRemoteHost + ":" + aRemotePort);
        CORBA::Object_var ret;
        try
        {
            ret = mOrb->bind( aRemoteName.c_str(), binding.c_str() );
        }
        catch( CORBA::SystemException &ex )
        {
            RPMS_ERROR( CBA_SS_LOG, "caught corba exception" );
            throw SX_CorbaImpl("Failed to bind and caught a CORBA::SystemException exception");
        }
        catch( CORBA::UserException &x )
        {
            RPMS_ERROR( CBA_SS_LOG, "caught corba exception" );
            throw SX_CorbaImpl("Failed to bind and caught a CORBA::UserException exception");
        }
        
        if( CORBA::is_nil(ret) )
        {
            throw SX_CorbaImpl("Failed to bind against remote object with [\"" 
                + aRemoteName + "\", \"" + binding + "\"]");
        }
        RPMS_TRACE(CBA_SS_LOG, "Successfully bound tp remote object [" + aRemoteName + "]");
        return ret;
    CATCH;
}


// ******************** HIDDEN ************
///////////////////////////////////////////
SS_CorbaImpl::SS_CorbaImpl( const SS_CorbaImpl &aRhs )
        : SA_ThreadBase("Copied thread") 
{
    TRY(SS_CorbaImpl::SS_CorbaImpl( const SS_CorbaImpl &aRhs ));
        throw SX_CorbaImpl("Copy ctor not allowed");
    CATCH;
}

///////////////////////////////////////////
SS_CorbaImpl & SS_CorbaImpl::operator=( const SS_CorbaImpl &aRhs )
{
    TRY(SS_CorbaImpl::operator=( const SS_CorbaImpl &aRhs ));
        throw SX_CorbaImpl("operator= not allowed");
    CATCH;
}

///////////////////////////////////////////
