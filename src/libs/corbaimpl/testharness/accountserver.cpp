#include "idls/account.h"
#include <fstream>
#include <iostream>

CORBA::Boolean finished = FALSE;

class AccountImpl;
typedef std::map<PortableServer::Servant, AccountImpl *, std::less<PortableServer::Servant> > ServantMap;
ServantMap svmap;

class AccountImpl : virtual public POA_Account
{
    public:
        AccountImpl() { mBalance = 0; };
        void deposit( CORBA::ULong aAmt )  { mBalance += aAmt; }
        void withdraw( CORBA::ULong aAmt ) { mBalance -= aAmt; }
        CORBA::Long balance() { return mBalance; }

    private:
        CORBA::Long mBalance;
};

class BankImpl : virtual public POA_Bank
{
    public:
        Account_ptr create( const char *aName)
        {
            std::cout << " - BankImpl: create Creating an Account" << std::endl;
            PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId (aName);
            CORBA::Object_var obj = mPoa->create_reference_with_id ( *oid, "IDL:Account:1.0");
            Account_ptr acc = Account::_narrow (obj);
            assert (!CORBA::is_nil (acc));
            std::cout << " - BankImpl: create - returning" << std::endl;
            return acc;
        }
        
        BankImpl( PortableServer::POA_ptr aPoa )
        {
            std::cout << " - BankImpl: ctor" << std::endl;
            mPoa = PortableServer::POA::_duplicate(aPoa);
        }

        void shutdown()
        {
            std::cout << " - BankImpl: Shutdown called" << std::endl;
            finished = TRUE;
        }

    private:
        PortableServer::POA_var mPoa;
};

class AccountManager : public virtual POA_PortableServer::ServantActivator
{
    public:
        PortableServer::Servant incarnate( const PortableServer::ObjectId &aId, 
                PortableServer::POA_ptr aPortSvr )
        {
            std::cout << " - AccountManager: incarnate" << std::endl;
            CORBA::String_var name = PortableServer::ObjectId_to_string (aId);
            
            AccountImpl *acc = new AccountImpl;
            //char * a = name;
            std::ifstream in ( name );
            std::cout << " - AccountManager: incarnate 1" << std::endl;
            if (in.rdbuf()->is_open()) 
            {
                CORBA::Long amount;
                in >> amount;
                if (amount > 0)
                {
                    acc->deposit (amount);
                }
                else
                {
                    acc->withdraw (-amount);
                }
            }
            svmap[acc] = acc;
            std::cout << " - AccountManager: incarnate returning" << std::endl;
            return acc;
        }
        
        void etherealize( const PortableServer::ObjectId &aId, 
                PortableServer::POA_ptr poa,
                PortableServer::Servant serv,
                CORBA::Boolean cleanup_in_progress,
                CORBA::Boolean remaining_activations)
        {
            std::cout << " - AccountManager: etherealize" << std::endl;
            if( !remaining_activations )
            {
                ServantMap::iterator it = svmap.find(serv);
                assert( it != svmap.end() );

                CORBA::String_var name = PortableServer::ObjectId_to_string(aId);
                std::ofstream out(name);
                out << (*it).second->balance() << std::endl;
                svmap.erase(it);
                delete serv;
            }
            std::cout << " - AccountManager: etherealize end of method" << std::endl;
        }
};



int main( int aArgC, char *aArgV[] )
{
    std::cout << "***************************************" << std::endl;
    for( int i = 0; i < aArgC; ++i )
    {
        std::cout << aArgV[i] << std::endl;
    }
    std::cout << "***************************************" << std::endl;
    std::cout << "Initialising" << std::endl;
    // initialise the ORB
    int noArgs = 3;
    char *orbArgs[noArgs];
    orbArgs[0] = "dummy";
    orbArgs[1] = "-ORBIIOPAddr";
    orbArgs[2] = "inet:pgdsX00.CRANLEYGDNS:9105";
    
    CORBA::ORB_var orb = CORBA::ORB_init (noArgs, orbArgs);

    // output all the know referenecs in the ORB
    CORBA::ORB::ObjectIdList_var ids = orb->list_initial_services();
    std::cout << "INITIAL REFS LIST\n";
    for( long i = 0; i < ids->length() ; ++i )
    { 
        std::cout << "    - " << ids[i] << "\n";
    }
    std::cout.flush();
    
    // get initial refs to the RootPOA service
    CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
    
    std::cout << "Initialising the  POAManager" << std::endl;
    // cast the object to a POA_var object
    PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
    // get the POAManager from the the current POA object
    PortableServer::POAManager_var mgr = poa->the_POAManager();

/*
    std::cout << "Creating first policy list" << std::endl;
    // 
    CORBA::PolicyList pl;
    pl.length(3);
    // use the current servanmt from the poa manager
    pl[0] = poa->create_request_processing_policy (PortableServer::USE_SERVANT_MANAGER);
    // sets up objects to outlive the current process (TRANSIENT | PERSISTENT)
    pl[1] = poa->create_lifespan_policy (PortableServer::TRANSIENT);
    // associates user id with the objects to ensure objects are persistent ( SYSTEM_ID | USER_ID )
    pl[2] = poa->create_id_assignment_policy (PortableServer::SYSTEM_ID);

    std::cout << "creating now portable server" << std::endl;
    // create a new POAManager for accounts
    PortableServer::POA_var accpoa = poa->create_POA ("Accounts", PortableServer::POAManager::_nil(), pl);
    PortableServer::POAManager_var accmgr = accpoa->the_POAManager();

    std::cout << "Creating account manager and setting it up as a servant" << std::endl;
    AccountManager * am = new AccountManager;
    PortableServer::ServantManager_var amref = am->_this ();
    accpoa->set_servant_manager (amref);
    
    std::cout << "Creating second policy list" << std::endl;
    CORBA::PolicyList pl2;
    pl2.length(2);
    // (TRANSIENT | PERSISTENT)
    pl2[0] = poa->create_lifespan_policy (PortableServer::PERSISTENT);
    // ( SYSTEM_ID | USER_ID )
    pl2[1] = poa->create_id_assignment_policy (PortableServer::USER_ID);
    PortableServer::POA_var bankpoa = poa->create_POA ("Bank", mgr, pl2);
    
    std::cout << "Creating a bank" << std::endl;
    BankImpl *bnk = new BankImpl(accpoa);

    std::cout << "Activating the bank" << std::endl;
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId("Bank");
    bankpoa->activate_object_with_id( *oid, bnk);

    std::cout << "Setting up ref file for the bank" << std::endl;
    std::ofstream of ("Bank.ref");
    CORBA::Object_var ref = bankpoa->id_to_reference(oid.in());
    CORBA::String_var str = orb->object_to_string(ref.in());
    std::cout << str.in() << std::endl;
    of << str.in() << std::endl;
    of.close ();

    std::cout << "Set the server running" << std::endl;
    mgr->activate ();
    accmgr->activate();
    while( !finished )
    {
        orb->perform_work();
    }

    std::cout << "cleaning up" << std::endl;
    poa->destroy (TRUE, TRUE);
    orb->shutdown(TRUE);
    delete bnk;
    delete am;
    */

    return 0;

}

