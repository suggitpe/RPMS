#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

void print_cpu_time()
{
    struct rusage usage;
    getrusage (RUSAGE_SELF, &usage);
    printf ("CPU time: %ld.%06ld sec user, %ld.%06ld sec system\n",
        usage.ru_utime.tv_sec, usage.ru_utime.tv_usec,
        usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    std::cout << "user (" << usage.ru_utime.tv_sec << ") secs, (" << usage.ru_utime.tv_usec << ")" << std::endl;
}

void debug() // just somewhere safe to keep the info
{
     /*cout << "Description ...............................................: " << setw(20) << "Latest" << setw(20) << "Baseline" << setw(20)  << "Diff"  << endl;
    cout << "-----------------------------------------------------------: " << setw(20) << "------" << setw(20) << "--------" << setw(20)  << "----"  << endl;
    cout << "Total amount of user time used (secs) .....................: " << setw(20) << mLatestData.ru_utime.tv_sec << setw(20)  << mBaselineData.ru_utime.tv_sec << setw(20)    << (mLatestData.ru_utime.tv_sec - mBaselineData.ru_utime.tv_sec)    << endl;
    cout << "Total amount of user time used (u_secs) ...................: " << setw(20) << mLatestData.ru_utime.tv_usec << setw(20) << mBaselineData.ru_utime.tv_usec << setw(20)   << (mLatestData.ru_utime.tv_usec - mBaselineData.ru_utime.tv_usec)  << endl;
    cout << "Total amount of system time used (secs) ...................: " << setw(20) << mLatestData.ru_stime.tv_sec << setw(20)  << mBaselineData.ru_stime.tv_sec << setw(20)    << (mLatestData.ru_stime.tv_sec - mBaselineData.ru_stime.tv_sec)    << endl;
    cout << "Total amount of system time used (usecs) ..................: " << setw(20) << mLatestData.ru_stime.tv_usec << setw(20) << mBaselineData.ru_stime.tv_usec << setw(20)   << (mLatestData.ru_stime.tv_usec - mBaselineData.ru_stime.tv_usec)  << endl;
    cout << "Maximum resident set size (in kilobytes) ..................: " << setw(20) << mLatestData.ru_maxrss << setw(20)    << mBaselineData.ru_maxrss << setw(20)      << (mLatestData.ru_maxrss - mBaselineData.ru_maxrss)        << endl;*/
}

int main()
{
    print_cpu_time();
    return 0;
}
