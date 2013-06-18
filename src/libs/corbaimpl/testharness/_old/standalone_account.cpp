#include <iostream>

class Account
{
    public:
        Account();
        void deposit( unsigned long aAmt );
        void withdraw( unsigned long aAmt );
        long balance() { return mBalance; }
        
    private:
        long mBalance;
};

Account::Account()
    : mBalance(0)
{
}

void Account::deposit( unsigned long aAmt )
{
    mBalance += aAmt;
}

void Account::withdraw( unsigned long aAmt )
{
    mBalance -= aAmt;
}

int main( int aArgC, char *aArgV[] )
{
    Account acc;
    acc.deposit(700);
    acc.withdraw(250);
    std::cout << "Balance is [" << acc.balance() << "]" << std::endl;

    return 0;
}
