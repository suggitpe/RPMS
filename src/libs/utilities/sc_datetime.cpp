//#############################
#include "sc_datetime.hpp"
#include <utilities/st_conversion.hpp>
#include <time.h>
#include <sys/time.h>

using rpms::SC_DateTime;

///////////////////////////////////////////
SC_DateTime::SC_DateTime( const int aYear, const int aMonth, const int aDay, const int aHour, const int aMin, const int aSec )
    : mYear(aYear), mMonth(aMonth), mDay(aDay), mHour(aHour), mMin(aMin), mSec(aSec)
{
}

///////////////////////////////////////////
SC_DateTime::~SC_DateTime()
{
}

///////////////////////////////////////////
SC_DateTime SC_DateTime::now()
{
    time_t tt; // Seconds since 1/1/1970
    struct tm *tod;
    time(&tt);
    tod = localtime(&tt);
    return SC_DateTime(tod->tm_year+1900, tod->tm_mon+1, tod->tm_mday, tod->tm_hour, tod->tm_min, tod->tm_sec);
}
///////////////////////////////////////////
std::string SC_DateTime::toString()
{
    using rpms::convert;
    std::string ret = "";
    if( mDay < 10 ){ ret+= "0"; }
    ret += convert<std::string>(mDay) + "/";
    if( mMonth < 10 ){ ret+= "0"; }
    ret += convert<std::string>(mMonth) + "/";
    ret += convert<std::string>(mYear);

    if( mHour < 10 ){ ret+= "0"; }
    ret += " " + convert<std::string>(mHour) + ":";
    if( mMin < 10 ){ ret+= "0"; }
    ret += convert<std::string>(mMin) + ":";
    if( mSec < 10 ){ ret+= "0"; }
    ret += convert<std::string>(mSec);
    return ret;
}

///////////////////////////////////////////
long SC_DateTime::getCurrentTimeInSecs()
{
    long ret = 0;
    time_t t = 0;
    ::time(&t);
    return t;
}

///////////////////////////////////////////
long long int SC_DateTime::getCurrentTimeInMillis()
{
    unsigned long long int ret = 0;
    struct timeval *pTime;
    pTime = (struct timeval *)malloc(sizeof(struct timeval));
    ::gettimeofday(pTime, 0);
    ret += ( pTime->tv_sec * 1000LL );
    ret += ( pTime->tv_usec / 1000LL );
    free(pTime);
    return ret;
}

///////////////////////////////////////////
///////////////////////////////////////////
