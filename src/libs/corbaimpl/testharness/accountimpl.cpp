#include "idls/account.h"

class AccountImpl : virtual public Account_skel
{
    public:
        AccountImpl() { mBalance = 0; };
        void deposit( CORBA::ULong aAmt )  { mBalance += aAmt; }
        void withdraw( CORBA::ULong aAmt ) { mBalance -= aAmt; }
        CORBA::Long balance() { return mBalance; }

    private:
        CORBA::Long mBalance;
};

int main( int aArgC, char *aArgV[] )
{
    std::cout << "*******************" << std::endl;
    CORBA::ORB_var orb = CORBA::ORB_init(aArgC, aArgV, "mico-local-orb");
    CORBA::BOA_var boa = orb->BOA_init(aArgC, aArgV, "mico-local-boa");

    std::cout << "*******************" << std::endl;
    // svr
    AccountImpl *server = new AccountImpl;
    CORBA::String_var ref = orb->object_to_string(server);
    std::cout << "Server ref: " << ref << std::endl;
    
    std::cout << "*******************" << std::endl;
    //---------------------------------------------
    // clt
    CORBA::Object_var obj = orb->string_to_object(ref);
    Account_var client = Account::_narrow(obj);
    client->deposit(450);
    client->withdraw(125);
    std::cout << "Balance: " << client->balance() << std::endl;
    CORBA::release(server);
    return 0;
    
    
}
/*
 int main( int argc, char *argv[] )
 {
   // ORB initialization
   CORBA::ORB_var orb = CORBA::ORB_init(aArgC, aArgV, "mico-local_orb");
   CORBA::ORB_var orb = CORBA::ORB_init( argc, argv, "mico-local-orb" );
   CORBA::BOA_var boa = orb->BOA_init( argc, argv, "mico-local-boa" );
 
   // server side
   AccountImpl* server = new AccountImpl;
   CORBA::String_var ref = orb->object_to_string( server );
   std::cout << "Server reference: " << ref << std::endl;
   
   //----------------------------------------------------------------------
   
   // client side
   CORBA::Object_var obj = orb->string_to_object( ref );
   Account_var client = Account::_narrow( obj );
 
   client->deposit( 700 );
   client->withdraw( 250 );
   std::cout << "Balance is " << client->balance() << std::endl;
 
   // We don't need the server object any more. This code belongs
   // to the server implementation
   CORBA::release( server );
   return 0;
 }
 */
