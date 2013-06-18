//#############################
#include "sc_systeminfo.hpp"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

using rpms::SC_SystemInfo;
///////////////////////////////////////////
SC_SystemInfo::SC_SystemInfo()
{
    sysinfo(&mSysInfo);
    uname(&mUName);
}

///////////////////////////////////////////
SC_SystemInfo::~SC_SystemInfo()
{
}

///////////////////////////////////////////
float SC_SystemInfo::getCpuClockSpeed()
{
    FILE *fp;
    char buf[1024];
    size_t bytes;
    char *match;
    float ret;

    fp = fopen("/proc/cpuinfo", "r");
    bytes = fread(buf, 1, sizeof(buf), fp);
    fclose(fp);

    if( bytes == 0 || bytes == sizeof(buf) )
    {
        return 0;
    }
    buf[bytes] = '\0';
    match = strstr(buf, "cpu MHz" );
    if( match == NULL )
    {
        return 0;
    }
    sscanf(match, "cpu MHz : %f", &ret );
    return ret;
}

///////////////////////////////////////////
