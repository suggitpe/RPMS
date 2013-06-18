#include "idls/account.h"
#include <iostream>
#include <fstream>

int main( int aArgC, char *aArgV[] )
{
    std::cout << "***************************************" << std::endl;
    std::cout << "Initialising" << std::endl;
    // initialise the ORB
    CORBA::ORB_var orb = CORBA::ORB_init( aArgC, aArgV );
    
    // create the object based on the read in string
    CORBA::Object_var obj = orb->bind("IDL:Bank:1.0", "inet:pgdsX00.CRANLEYGDNS:9105");
    if( CORBA::is_nil(obj) )
    {
        std::cerr << "No object of type [IDL:Bank:1.0] could be found on [inet:pgdsX00.CRANLEYGDNS:9105]" << std::endl;
        exit(1);
    }

    std::cout << "client: creating a bank" << std::endl;
    // now we have to down cast the object to the correct type of object
    //  will return null if the object does not exist
    Bank_var bank = Bank::_narrow (obj);
    if( CORBA::is_nil(bank) ) 
    {
        std::cout << "oops: could not locate Bank" << std::endl;
        exit (1);
    }

    std::cout << "client: creating an account" << std::endl;
    // now we can create an account from the bank object
    //   returns an interface to the accountompl on the server side
    Account_var account = bank->create ("Pete");
    if (CORBA::is_nil (account)) 
    {
        std::cout << "oops: account is nil" << std::endl;
        exit (1);
    }

    ////////////////////////////////////////////////////////////////////////////////

    std::cout << "calling deposit" << std::endl;
    // call desposit on the remote object through the interface
    account->deposit (700);

    std::cout << "calling withdraw" << std::endl;
    // call withdraw on the remote object through the interface
    account->withdraw (450);

    // we now call the balance method on the remore accountimpl object
    //      returning to us the remote value
    std::cout << "Balance is " << account->balance() << std::endl;

    // 
    std::cout << "calling bank shutdown" << std::endl;
    bank->shutdown();
    // shutdown the ORB before we destroy it
    std::cout << "calling orb shutdown" << std::endl;
    orb->shutdown(true);
    //  destroys the ORB .. thread safe
    std::cout << "calling orb destroy" << std::endl;
    orb->destroy();

    return 0;
}





/* THIS IS OLD STYLE BOA APPROACH
int main( int aArgC, char *aArgV[] )
{
    std::cout << "***************************************" << std::endl;
    std::cout << "Initialising";
    CORBA::ORB_var orb = CORBA::ORB_init( aArgC, aArgV, "mico-local-orb");
    CORBA::BOA_var boa = orb->BOA_init( aArgC, aArgV, "mico-local-boa");
    std::cout << " ... done" << std::endl;

    std::cout << "Getting server ref [";
    std::ifstream in("/tmp/account.objid");
    char ref[1000];
    in >> ref;
    in.close();
    std::cout << ref;
    std::cout << "] ... done" << std::endl;

    std::cout << "Getting object refs to bind to" ;
    CORBA::Object_var obj = orb->string_to_object(ref);
    Account_var client = Account::_narrow(obj);
    std::cout << " ... done" << std::endl;

    client->deposit(700);
    client->withdraw(250);
    std::cout << client->balance() << std::endl;

    return 0;
     
}
*/
