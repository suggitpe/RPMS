//#############################

//#include <ostream>
#include <base/st_spointer.hpp>
#include <testharness/si_testharness.hpp>
#include <utilities/st_conversion.hpp>
#include <pthread.h>
#include <queue>
#include <dlfcn.h>
#include <utilities/sc_datetime.hpp>
#include <utilities/sc_systeminfo.hpp>
#include <utilities/sc_processinfo.hpp>
#include <utilities/sc_filesystemhelper.hpp>

//////////////////////////////////////////////////
// funct decl
void testSec();
void testIni();
void testSecPtr();
void testThreading();
void testTime();
void testSysInfo();
void testForExe();
void testProcessInfo( int aArgC, char *aArgV[] );
void testProcessSelf();
void testProcessPid( const int aPid);

TEST_BASE;

using rpms::SC_DateTime;
using rpms::SC_SectionMap;
using rpms::SC_SystemInfo;
using rpms::SC_ProcessInfo;
using rpms::SC_FileSystemHelper;
using rpms::convert;

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    RPMS_DEBUG( LOGNAME, "This is an application to test objects");
    RPMS_DEBUG( LOGNAME, "START ***************************");
    //testIni();
    //testSec();
    //testSecPtr();
    //testThreading();
    //testTime();
    testSysInfo();
    //testForExe();
    
    //testProcessInfo( aArgC, aArgV );
    RPMS_DEBUG( LOGNAME, "END   ***************************");
}

#define MAX 1000

//////////////////////////////////////////////////
void testProcessInfo( int aArgC, char *aArgV[] )
{
    if( aArgC < 2 )
    {
        std::cerr << "You need to pass in an argument for the pid" << std::endl;
        exit(1);
    }

    RPMS_DEBUG( LOGNAME, "Creating a process info object");
    int pid = atoi(aArgV[1]);
    if(pid == 0)
    {
        testProcessSelf();
    }
    else
    {
        testProcessPid( pid );
    }
}

//////////////////////////////////////////////////
void testProcessSelf()
{
    RPMS_DEBUG( LOGNAME, "Testing myself");
    SC_ProcessInfo info(::getpid());
    for( int i = 0; i < 1000; ++i )
    {
        std::queue<std::string> * q = new std::queue<std::string>;
        q->push("***************************************************************");
        void * lib = ::dlopen("/home/suggitpe/codebase/RPMS/trunk/build/install/lib/libclientlib.so", RTLD_LAZY);
        if( !lib )
        {
            RPMS_DEBUG( LOGNAME, "Failed to open lib");
        }
        dlclose(lib);
        ::getpid();
        ::getgid();
    }
    sleep(1);
    info.load();
    info.debug();
}

//////////////////////////////////////////////////
void testProcessPid( const int aPid)
{
    RPMS_DEBUG( LOGNAME, "Testing external pid");
    SC_ProcessInfo info(aPid);
    while(1)
    {
        info.load();
        if(!info.isLatestValid())
        {
            break;
        }
        info.debug();
        sleep(1);
    }
}

//////////////////////////////////////////////////
void testForExe()
{
    std::queue<std::string> tests;
    tests.push("stdouttest");
    tests.push("stdouttest1");
    tests.push("stdouttest2");


    while(!tests.empty())
    {
        RPMS_DEBUG( LOGNAME, "Testing for exe [" + tests.front() + "]");
        if( SC_FileSystemHelper::testForExecutable(tests.front()) )
        {
            RPMS_DEBUG( LOGNAME, "Found           [" + tests.front() + "]");
        }
        else
        {
            RPMS_DEBUG( LOGNAME, "Did not find    [" + tests.front() + "]");
        }
        tests.pop();
    }
}

//////////////////////////////////////////////////
void testSysInfo()
{
    RPMS_DEBUG( LOGNAME,  "Machine ***********");
    RPMS_DEBUG( LOGNAME,  "   CPU speed ------- [" + rpms::convert<std::string>(SC_SystemInfo::getCpuClockSpeed()) + "]");
    RPMS_DEBUG( LOGNAME,  "   This process size [" + rpms::convert<std::string>(SC_ProcessInfo::getRssForPid(getpid())) + "]");
    SC_SystemInfo info;
    RPMS_DEBUG( LOGNAME,  "SysInfo ***********");
    RPMS_DEBUG( LOGNAME,  "   Load 0 ---------- [" + rpms::convert<std::string>(info.getLoad0()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Load 1 ---------- [" + rpms::convert<std::string>(info.getLoad1()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Load 2 ---------- [" + rpms::convert<std::string>(info.getLoad2()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Total RAM ------- [" + rpms::convert<std::string>(info.getTotalRAM()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Free RAM -------- [" + rpms::convert<std::string>(info.getFreeRAM()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Shared RAM ------ [" + rpms::convert<std::string>(info.getSharedRAM()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Buffered RAM ---- [" + rpms::convert<std::string>(info.getBufferedRAM()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Total Swap ------ [" + rpms::convert<std::string>(info.getTotalSwap()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Free Swap ------- [" + rpms::convert<std::string>(info.getFreeSwap()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Num Procs ------- [" + rpms::convert<std::string>(info.getNumProcs()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Pad ------------- [" + rpms::convert<std::string>(info.getPad()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Total High ------ [" + rpms::convert<std::string>(info.getTotalHigh()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Free High ------- [" + rpms::convert<std::string>(info.getFreeHigh()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Mem Unit -------- [" + rpms::convert<std::string>(info.getMemUnit()) + "]");
    RPMS_DEBUG( LOGNAME,  "UName ***********");
    RPMS_DEBUG( LOGNAME,  "   System Name ----- [" + std::string(info.getSysName()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Node Name ------- [" + std::string(info.getNodeName()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Release --------- [" + std::string(info.getRelease()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Version --------- [" + std::string(info.getVersion()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Machine --------- [" + std::string(info.getMachine()) + "]");
    RPMS_DEBUG( LOGNAME,  "   Domain Name ----- [" + std::string(info.getDomainName()) + "]");

    RPMS_DEBUG( LOGNAME,  "Leak test ***********");
    for(int i = 0; i < 10; ++i)
    {
        SC_SystemInfo *leak = new SC_SystemInfo();
        RPMS_DEBUG( LOGNAME,  "   This process size [" + rpms::convert<std::string>(SC_ProcessInfo::getRssForPid(getpid())) + "]");
        //delete leak;
        //usleep(500000);
    }
}

//////////////////////////////////////////////////
void testTime()
{
    std::cout << " ------- " << SC_DateTime::now().toString() << std::endl;
    std::cout << " ------- " << SC_DateTime::getCurrentTimeInSecs() << std::endl;
    std::cout << " ------- " << SC_DateTime::getCurrentTimeInMillis() << std::endl;
}

//////////////////////////////////////////////////
#define NUM_THREADS 25

void *debug(void * args)
{
    for( int i=0; i< NUM_THREADS; ++i )
    {
        RPMS_DEBUG(LOGNAME, SS_Ini::instance()->getString("general", "system.name"));
        RPMS_DEBUG(LOGNAME, SS_Ini::instance()->getString("daemon", "daemon.kjjh.kjkjh"));
        RPMS_DEBUG(LOGNAME, SS_Ini::instance()->getString("logging", "type"));
        usleep(5000);
    }
    return 0;
}

void testThreading()
{
    pthread_t p[NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; i++)
    {
		pthread_create (&(p[i]), NULL, debug, NULL );
    }
    for(int i = 0; i < NUM_THREADS; i++)
    {
		pthread_join (p[i], NULL );
    }
}



//////////////////////////////////////////////////
void testIni()
{
    RPMS_DEBUG( LOGNAME, "-------------------------------------------------------------------");
    try
    {
        SS_Ini::instance()->initialise("./test.ini");
        //SS_Ini::instance()->debug();

        RPMS_DEBUG( LOGNAME, "Getting string from section \"section1\", name \"key1\"");
        RPMS_DEBUG( LOGNAME, "\tstring = [" + SS_Ini::instance()->getString("section1", "key1") + "]");

        RPMS_DEBUG( LOGNAME, "Getting int from section \"section2\", name \"key2\"");
        RPMS_DEBUG( LOGNAME, "\tint = [" + rpms::convert<std::string>(SS_Ini::instance()->getInt("section2", "key2")) + "]");

        RPMS_DEBUG( LOGNAME, "Getting bool from section \"section2\", name \"key3\"");
        RPMS_DEBUG( LOGNAME, "\tbool = [" + rpms::convert<std::string>(SS_Ini::instance()->getBool("section2", "key3")) + "]");

        RPMS_DEBUG( LOGNAME, "Getting bad int from section \"section2\", name \"key1\"");
        RPMS_DEBUG( LOGNAME, "\tbad int = [" + rpms::convert<std::string>(SS_Ini::instance()->getInt("section2", "key1")) + "]");
    }
    catch( SX_Exception &iniEx )
    {
        RPMS_ERROR( LOGNAME, iniEx.reason());
    }
    RPMS_DEBUG( LOGNAME, "-------------------------------------------------------------------");
}


//////////////////////////////////////////////////
void testSecPtr()
{
    RPMS_DEBUG( LOGNAME, "-------------------------------------------------------------------");
    RPMS_DEBUG( LOGNAME, "Testing smart pointer with SC_SectionMap");
    RPMS_DEBUG( LOGNAME, "new map");
    std::map<std::string, ST_SPointer<SC_SectionMap> > * myMap = new std::map<std::string, ST_SPointer<SC_SectionMap> >;
    {
        RPMS_DEBUG( LOGNAME, "define sptr with ptr");
        ST_SPointer<SC_SectionMap> sec1 = new SC_SectionMap("testsection");
        RPMS_DEBUG( LOGNAME, rpms::convert<std::string>(sec1.locks()));

        RPMS_DEBUG( LOGNAME, "insert");
        myMap->insert( std::pair<std::string, ST_SPointer<SC_SectionMap> >(std::string("section1"), sec1 ) );
        RPMS_DEBUG( LOGNAME, rpms::convert<std::string>(sec1.locks()));
        RPMS_DEBUG( LOGNAME, "add some data");
        sec1->add("key11", "data11");
        sec1->add("key12", "data12");
        RPMS_DEBUG( LOGNAME, rpms::convert<std::string>(sec1.locks()));
        RPMS_DEBUG( LOGNAME, "replace");
        sec1 = new SC_SectionMap("atest");
        RPMS_DEBUG( LOGNAME, "..............");
    }
    RPMS_DEBUG( LOGNAME, "..............");


    RPMS_DEBUG( LOGNAME, "define another sptr with ptr");
    ST_SPointer<SC_SectionMap> sec2 = new SC_SectionMap("test2");
    RPMS_DEBUG( LOGNAME, "add key");
    sec2->add("key21", "data21");
    RPMS_DEBUG( LOGNAME, "insert second one");
    myMap->insert( std::pair<std::string, ST_SPointer<SC_SectionMap> >(std::string("section2"), sec2 ) );
    RPMS_DEBUG( LOGNAME, "done");


    // iterates through the map dumping the contents
    std::map<std::string, ST_SPointer<SC_SectionMap> >::iterator iter = myMap->begin();
    while( iter != myMap->end() )
    {
        RPMS_DEBUG( LOGNAME, "iteration ..............................");
        RPMS_DEBUG( LOGNAME, iter->first + ": ");
        iter->second->debug();
        ++iter;
    }

    delete myMap;
    
     RPMS_DEBUG( LOGNAME, "-------------------------------------------------------------------");
}

//////////////////////////////////////////////////
void testSec()
{
    RPMS_DEBUG( LOGNAME, "-------------------------------------------------------------------");
    RPMS_DEBUG( LOGNAME, "Building ini file sections ..");
    RPMS_DEBUG( LOGNAME, "####################");
    {
        try
        {
            RPMS_DEBUG( LOGNAME, " - creating a local map0");
            SC_SectionMap m0("m0");
            m0.debug();
            RPMS_DEBUG( LOGNAME, "..........");

            RPMS_DEBUG( LOGNAME, " - adding some elements");
            m0.add("name0", "value0");
            m0.add("name1", "value1");
            m0.add("name2", "value2");
            m0.add("name3", "value3");
            m0.debug();
            RPMS_DEBUG( LOGNAME, "..........");

            RPMS_DEBUG( LOGNAME, " - taking a copy of m0 with copy ctor");
            SC_SectionMap m1(m0);
            m1.debug();
            RPMS_DEBUG( LOGNAME, "..........");

            RPMS_DEBUG( LOGNAME, " - taking a copy of m0 with assignment operator");
            SC_SectionMap m2("m2");
            m2 = m0;
            m2.debug();
            RPMS_DEBUG( LOGNAME, "..........");

            RPMS_DEBUG( LOGNAME, "####################");
            RPMS_DEBUG( LOGNAME, " - adding to m0");
            m0.add("add_key0", "add_value0");
            m0.debug();
            RPMS_DEBUG( LOGNAME, "..........");
            m1.debug();
            RPMS_DEBUG( LOGNAME, "..........");
            m2.debug();
            RPMS_DEBUG( LOGNAME, "..........");
        
            RPMS_DEBUG( LOGNAME, "####################");
            RPMS_DEBUG( LOGNAME, " - adding to m1");
            m1.add("add_key1", "add_value1");
            m0.debug();
            RPMS_DEBUG( LOGNAME, "..........");
            m1.debug();
            RPMS_DEBUG( LOGNAME, "..........");
            m2.debug();
            RPMS_DEBUG( LOGNAME, "..........");

            RPMS_DEBUG( LOGNAME, "####################");
            RPMS_DEBUG( LOGNAME, " - creating a local map3 with string string ctor");
            SC_SectionMap m3("nm1", "val1");
            m3.debug();
            RPMS_DEBUG( LOGNAME, "..........");

            RPMS_DEBUG( LOGNAME, "test of assop");
            SC_SectionMap m4("testsection");
            m4 = m3;

            RPMS_DEBUG( LOGNAME, "####################");
            RPMS_DEBUG( LOGNAME, " - adding the same strings to the map");
            m3.add("nm1", "val1");
            m3.debug();
            RPMS_DEBUG( LOGNAME, "..........");
        }
        catch( SX_Exception &ex )
        {
            RPMS_ERROR( LOGNAME, ex.reason() );
            
        }
    RPMS_DEBUG( LOGNAME, "-------------------------------------------------------------------");
    }
}

