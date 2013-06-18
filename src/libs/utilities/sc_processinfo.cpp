//#############################
#include "sc_processinfo.hpp"
#include "si_utilitieslog.hpp"
#include <fcntl.h>
#include <logging/si_logging.hpp>
#include <iomanip>
#include <utilities/st_conversion.hpp>
#include <utilities/ss_ini.hpp>
#include <utilities/sc_datetime.hpp>

// thes era used to define where in the stat file the stats reside
#define STAT_FIELDS 39
#define FIELD_SIZE  32
#define PROCNAME 1
#define UTIME 13
#define STIME 14
#define RSS   23

using rpms::SC_ProcessInfo;
///////////////////////////////////////////
SC_ProcessInfo::SC_ProcessInfo( const int aPid)
    : mPid(aPid)
{
    load(true); // to make sure we always have some level of data in the object
}

///////////////////////////////////////////
SC_ProcessInfo::SC_ProcessInfo()
    : mPid(0)
{
}

///////////////////////////////////////////
SC_ProcessInfo::~SC_ProcessInfo()
{
}

///////////////////////////////////////////
void SC_ProcessInfo::load( const bool aBaseLine )
{
    using rpms::convert;
    // get the underlying data
    int ret = 0;
    int numerr = 0;
    if( aBaseLine )
    {
        RPMS_TRACE( UTL_PCI_LOG, "Loading baseline resource usage with [" + convert<std::string>(mPid) + "]");
        if( -1 == get_proc_stats( mPid, &mBaselineData))
        {
            RPMS_ERROR( UTL_PCI_LOG, "Failed to load up process statistics" );
        }
        // set the latest to the baseline
        mLatestData = mBaselineData;
    }
    else
    {
        RPMS_TRACE( UTL_PCI_LOG, "Loading latest resource usage with [" + convert<std::string>(mPid) + "]");
        if( -1 == get_proc_stats( mPid, &mLatestData ))
        {
            RPMS_ERROR( UTL_PCI_LOG, "Failed to load up process statistics" );
        }
    }
}

///////////////////////////////////////////
double SC_ProcessInfo::getCpuAvgPerMin() const
{
    using rpms::convert;
    if( mLatestData.mSuccess && mBaselineData.mSuccess )
    {
        long t = getTimeStampDiff();
        if( t < 60 )
        {
            return getCpuTimeDiff();
        }
        else
        {
            return ((double)getCpuTimeDiff()) / (((double)t/(double)60));
        }
    }
    return 0;
}

///////////////////////////////////////////
double SC_ProcessInfo::getCpuAvgPerSec() const
{
    using rpms::convert;
    if( mLatestData.mSuccess && mBaselineData.mSuccess )
    {
        return ((double)getCpuTimeDiff()) / ((double)getTimeStampDiff());
    }
    return 0;
}

///////////////////////////////////////////
double SC_ProcessInfo::getRssAvg() const 
{
    if( mLatestData.mSuccess && mBaselineData.mSuccess )
    {
        return ((float)getRssDiff()) / ((float)getTimeStampDiff());
    }
    return 0;
}

///////////////////////////////////////////
int SC_ProcessInfo::get_proc_stats( const int aPid,  struct rpms::proc_data *aData )
{
    if( aPid == 0 )
    {
        RPMS_WARN( UTL_PCI_LOG, "Invalid pid [0] used for get_proc_stats()");
        aData->mSuccess = false;
        aData->mProcName = "N/A";
        aData->mTimeStamp = 0;
        aData->mUserTime = 0;
        aData->mSysTime = 0;
        aData->mRss = 0;
        return -1;
    }
    
    char fileName[64];
    int fd;
    snprintf( fileName, sizeof(fileName), "/proc/%d/stat", (int)aPid );
    fd = open(fileName, O_RDONLY);
    if( fd == -1 )
    {
        RPMS_WARN( UTL_PCI_LOG, "Could not open /proc/" + convert<std::string>(aPid) + "/stat");
        aData->mSuccess = false;
        aData->mProcName = "N/A";
        aData->mTimeStamp = 0;
        aData->mUserTime = 0;
        aData->mSysTime = 0;
        aData->mRss = 0;
        return -1;
    }
    char procInfo[1024];
    int rval = read( fd, procInfo, sizeof(procInfo)-1 );
    close(fd);
    if( rval <= 0 )
    {
        aData->mSuccess = false;
        aData->mProcName = "N/A";
        aData->mTimeStamp = 0;
        aData->mUserTime = 0;
        aData->mSysTime = 0;
        aData->mRss = 0;
        return -1;
    }
    procInfo[rval] = '\0';
    //std::cout << procInfo;
    //std::cout.flush();

    const char * ptr = procInfo;
    char field[32];
    int n;
    char vals[STAT_FIELDS][FIELD_SIZE];
    int c = 0;
    while( sscanf( ptr, "%31[^ ]%n", field, &n) == 1 )
    {
        strncpy( vals[c], field, sizeof(field) );
        ptr += n;
        if( *ptr != ' ' )
        {
            break;
        }
        ++ptr;
        ++c;
    }
    // now we can populate the values
    rval =  sscanf( vals[UTIME],    "%lu", &aData->mUserTime );
    rval += sscanf( vals[STIME],    "%lu", &aData->mSysTime );
    rval += sscanf( vals[RSS],      "%ld", &aData->mRss );
    aData->mRss = aData->mRss * ::getpagesize() / 1024;
    aData->mTimeStamp = SC_DateTime::getCurrentTimeInSecs();
    // for the procname we need to strip off the ()s
    aData->mProcName = std::string(vals[PROCNAME]).substr(1, strlen(vals[PROCNAME])-2);
    aData->mSuccess = true;

    return 0;
}

///////////////////////////////////////////
long SC_ProcessInfo::getRssForPid( const int aPid )
{
    using rpms::convert;
    char fileName[64];
    int fd;
    int pid;
    if( aPid == 0)
        pid = ::getpid();
    else
        pid = aPid;

    snprintf( fileName, sizeof(fileName), "/proc/%d/statm", (int)pid );
    fd = open(fileName, O_RDONLY);
    if( fd == -1 )
    {
        RPMS_WARN( UTL_PCI_LOG, "Could not open /proc/" + convert<std::string>(pid) + "/statm");
        return -1;
    }

    char memInfo[128];
    int rval = read( fd, memInfo, sizeof(memInfo)-1 );
    close(fd);
    if( rval <= 0 )
    {
        return -1;
    }
    memInfo[rval] = '\0';
    long rss;
    rval = sscanf( memInfo, "%*d %d", &rss );
    if( rval != 1 )
    {
        return -1;
    }
    return rss * ::getpagesize() / 1024;
}

///////////////////////////////////////////
void SC_ProcessInfo::debug() const
{
    if(SS_Ini::instance()->getBool("utilities", "processinfo.debug"))
    {
        using std::setw;
        using std::setfill;
        using std::cout;
        using std::endl;
        cout << "Description of " << setw(31) << mLatestData.mProcName << ": " << setw(20) << "Latest" << setw(20) << "Baseline" << setw(20)  << "Diff"  << endl;
        cout << "----------------------------------------------: " << setw(20) << "------" << setw(20) << "--------" << setw(20)  << "----"  << endl;
        cout << "Valid check ..................................: " << setw(20) << mLatestData.mSuccess << setw(20)  << mBaselineData.mSuccess << setw(20) << "N/A" << endl;
        cout << "Time since baseline check ....................: " << setw(20) << mLatestData.mTimeStamp << setw(20)  << mBaselineData.mTimeStamp << setw(20) << (mLatestData.mTimeStamp - mBaselineData.mTimeStamp) << endl;
        cout << "Total amount of user time used (jiffies) .....: " << setw(20) << mLatestData.mUserTime << setw(20)   << mBaselineData.mUserTime  << setw(20) << (mLatestData.mUserTime  - mBaselineData.mUserTime)  << endl;
        cout << "Total amount of system time used (jiffies) ...: " << setw(20) << mLatestData.mSysTime << setw(20)    << mBaselineData.mSysTime   << setw(20) << (mLatestData.mSysTime   - mBaselineData.mSysTime)   << endl;
        cout << "Resident set size (Kb) .......................: " << setw(20) << mLatestData.mRss << setw(20)        << mBaselineData.mRss       << setw(20) << (mLatestData.mRss       - mBaselineData.mRss)       << endl;
    }
}

///////////////////////////////////////////
