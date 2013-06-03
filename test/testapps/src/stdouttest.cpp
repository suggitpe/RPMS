#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>

using std::cout;
using std::cerr;
using std::endl;

#define LOOP 10000000
#define STAT_FIELDS 39

#define PID         0   // %d fine
#define COMM        1   // %s fine
#define STATE       2   // %c fine
#define PPID        3   // %d fine
#define PGRP        4   // %d fine
#define SESSION     5   // %d fine
#define TTY_NR      6   // %d fine
#define TPGID       7   // %d fine
#define FLAGS       8   // %lu fine
#define MINFLT      9   // %lu fine
#define CMINFLT     10  // %lu fine
#define MAJFLT      11  // %lu fine
#define CMAJFLT     12  // %lu fine
#define UTIME       13  // %lu fine
#define STIME       14  // %lu fine
#define CUTIME      15  // %ld fine
#define CSTIME      16  // %ld fine
#define PRIORITY    17  // %ld fine
#define NICE        18  // %ld fine
#define ITREALVALUE 19  // %ld
#define RANDOM      20  // ???????
#define STARTTIME   21  // %lu fine
#define VSIZE       22  // %lu fine
#define RSS         23  // %ld fine
#define RLIM        24  // %lu
#define STARTCODE   25  // %lu
#define ENDCODE     26  // %lu
#define STARTSTACK  27  // %lu
#define KSTKESP     28  // %lu
#define KSTKEIP     29  // %lu
#define SIGNAL      30  // %lu
#define BLOCKED     31  // %lu
#define SIGIGNORE   32  // %lu
#define SIGCATCH    33  // %lu
#define WCHAN       34  // %lu
#define NSWAP       35  // %lu
#define CNSWAP      36  // %lu
#define EXIT_SIGNAL 37  // %d
#define PROCESSOR   38  // %d


////////////////////////////////////////////////
struct CpuStats
{
    unsigned long utime;
    unsigned long stime;
    long rss;
};

////////////////////////////////////////////////
void handler( int sig_num )
{
    cout << "################ caught signal [" << sig_num << "] ##############" << endl;
    exit(0);
}

int getMemorySize();
int getCPUStats( struct CpuStats *cpu );

////////////////////////////////////////////////
int main( int args, char *argv[] )
{
    for( int i = 0; i < args; ++i )
    {
        cout << "arg " << i << " [" << argv[i] << "]\n";
    }
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    //for( int i = 0; i < LOOP; ++i )
    for( int i = 0;; ++i )
    {
        if( 1 )
        {
            std::string * i = new std::string("************************************************************"); // a leak
            //cerr << "*********** error loop " << *i << endl;
        }
        struct rusage use;
        ::getrusage( RUSAGE_SELF, &use );
        struct CpuStats cpu;
        getCPUStats( &cpu );
        cout << "Test loop [" << i << "]" 
            << "************************************** " << ::getpid() << " " << getMemorySize() << " [" 
            << ( getMemorySize() * getpagesize() / 1024 ) << "] = system (" 
            << use.ru_stime.tv_sec << "." << use.ru_stime.tv_usec << ") user ("
            << use.ru_utime.tv_sec << "." << use.ru_utime.tv_usec << ")" 
            << " | user[" << cpu.stime << "] system [" << cpu.utime << "] rss [" << cpu.rss << "]" << endl;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////
int getCPUStats( struct CpuStats *cpu )
{
    char fileName[64];
    int fd;
    snprintf( fileName, sizeof(fileName), "/proc/%d/stat", (int)getpid() );
    fd = open(fileName, O_RDONLY);
    if( fd == -1 )
    {
        return -1;
    }
    char procInfo[1024];
    int rval = read( fd, procInfo, sizeof(procInfo)-1 );
    close(fd);
    if( rval <= 0 )
    {
        return -1;
    }
    procInfo[rval] = '\0';

    const char * ptr = procInfo;
    char field[32];
    int n;
    char vals[STAT_FIELDS][32];
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
    rval =  sscanf( vals[UTIME], "%lu", &cpu->utime );
    rval += sscanf( vals[STIME], "%lu", &cpu->stime );
    rval += sscanf( vals[RSS],   "%ld", &cpu->rss );
    cpu->rss = cpu->rss * getpagesize() / 1024;

    return 0;
}

/////////////////////////////////////////////////////////////////////
int getMemorySize()
{
    char fileName[64];
    int fd;

    snprintf( fileName, sizeof(fileName), "/proc/%d/statm", (int)getpid() );
    fd = open(fileName, O_RDONLY);
    if( fd == -1 )
    {
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
    int rss;
    rval = sscanf( memInfo, "%*d %d", &rss );
    if( rval != 1 )
    {
        return -1;
    }
    return rss;// * getpagesize() / 1024;
}


////////////////////////////////////////////////
