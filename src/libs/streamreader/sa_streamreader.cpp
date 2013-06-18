//#############################
#include "sa_streamreader.hpp"
#include <si_macros.hpp>

using rpms::SA_StreamReader;

///////////////////////////////////////////
SA_StreamReader::SA_StreamReader( const std::string &aThreadName, const int aPipe )
    : SA_ThreadBase(aThreadName), mPipe(aPipe), mShouldRun(true)
{
    TRY(SA_StreamReader::SA_StreamReader( const std::string &aThreadName, const int aPipe ));
    CATCH;
}

///////////////////////////////////////////
SA_StreamReader::~SA_StreamReader()
{
    TRY(SA_StreamReader::~SA_StreamReader());
    CATCH;
}

///////////////////////////////////////////
void SA_StreamReader::stop()
{
    TRY(SA_StreamReader::stop());
        mShouldRun = false;
        stopDead();
    CATCH;
}

///////////////////////////////////////////
void SA_StreamReader::run()
{
    TRY(SA_StreamReader::run());
        execute();
    CATCH;
}

///////////////////////////////////////////
