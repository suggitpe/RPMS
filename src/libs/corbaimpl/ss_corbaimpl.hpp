#ifndef __ss_corbaimpl_hpp
#define __ss_corbaimpl_hpp

#include <base/st_singleton.hpp>
#include <base/sa_threadbase.hpp>
#include <CORBA.h>

namespace rpms
{

    /** 
     * Corba singleton class to manage the interface between the app and the CORBA ORB.  This is 
     *      set up as a singleton to avoid issues in the call stack.
     *
     *  @author Peter Suggitt (2005)
     *  @note <b>Concrete class</b>
     * */
    class SS_CorbaImpl : public ST_Singleton<SS_CorbaImpl>, public SA_ThreadBase
    {
        friend class ST_Singleton<SS_CorbaImpl>;
        public:
            /** dtor */
            virtual ~SS_CorbaImpl();
            /** initialise the orb */
            void initOrb( const bool aCanSend = true, const bool aCanRecv = true );
            /** Initialise the poa with a servant object (eg the diretor will initialise with itself) */
            void initPoa( const PortableServer::Servant &aServant );
            /** This is used to deregister an object with the poa mgr */
            void deRegisterPoa( const PortableServer::Servant &aServant );
            /** This is a wrapper for the main bind function and gives us the flexibility to connect to remote objects with a few strings and a port number */
            CORBA::Object_var SS_CorbaImpl::getRemoteObject( const std::string &aRemoteHost, const std::string &aRemotePort, const std::string &aRemoteName );

            /** Accessor to the cansend flag */
            bool canSend() {return mCanSend;}
            /** Accessor to the can recv flag */
            bool canRecv() {return mCanRecv;}
            /** Stop running the ORB */
            void stop();

        protected:
            /** We can only create one of these objects through the singleton static method */
            SS_CorbaImpl();
            /** runs the ORB itself */
            void runOrb();
            /** Run the underlying orb */
            void run();
        private:
            /** Hidden opy ctor */
            SS_CorbaImpl( const SS_CorbaImpl &aRhs );
            /** Hidden assignment operator */
            SS_CorbaImpl & operator=( const SS_CorbaImpl &aRhs );

            /** This is the main crux of the CORBA communications */
            CORBA::ORB_var mOrb;
            /** Portable server that alll corbe enabled objects can bind to */
            PortableServer::POA_var mPoa;
            /** Used to initialise the ORB initially */
            bool mCanSend;
            /** Used to initialise the ORB initially */
            bool mCanRecv;
            /** Simple flag denoting whether the ORB is initialised yet */
            bool mOrbInitialised;
    };

} // namespace

#endif
