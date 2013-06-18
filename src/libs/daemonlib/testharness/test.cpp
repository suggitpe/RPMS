//#############################
#include <testharness/si_testharness.hpp>

#include <base/st_spointer.hpp>
#include <daemonlib/sx_process.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>
#include <daemonlib/sc_processwrapperthread.hpp>
#include <daemonlib/sc_statequeue.hpp>

TEST_BASE;

using rpms::SC_ProcessConfig;
using rpms::SC_ProcessWrapperThread;
using rpms::SX_Unknown;
using rpms::SC_StateQueue;
using rpms::SC_StateQueueObject;

void testProcWrapper();
void testStateQueue();

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    TRY(execute());
    RPMS_INFO( LOGNAME, "START ***************************");
    testProcWrapper();
    //testStateQueue();
    RPMS_INFO( LOGNAME, "END   ***************************");
    CATCH;
}

void testStateQueue()
{
    RPMS_INFO( LOGNAME, "Adding to the queue");
    SC_StateQueue s;
    s.push( SC_StateQueueObject::_START, "start" );
    s.push( SC_StateQueueObject::_CONFIGURE, "config" );
    s.push( SC_StateQueueObject::_RESTART, "restart" );
    s.push( SC_StateQueueObject::_STOP, "stop" );
    RPMS_INFO( LOGNAME, "Getting from the queue");
    size_t q = s.size();
    for( int i = 0; i < q; ++i)
    {
        SC_StateQueueObject o = s.front();
        RPMS_INFO( LOGNAME, "    - val = [" + o.getArg() + "]");
        s.pop();
    }
}

//////////////////////////////////////////////////
void testProcWrapper()
{
    // set up the args
    //std::string compName = "testscript1.pl";
    std::string compName = "stdouttest";
    int instance = 0;
    bool runnable = true;
    bool moveable = true;
    int maxretries = 2;
    long retrywindow = 100;
    std::string runType = "continuous";
    std::string out = "file";
    std::string err = "off";
    std::string args = "-g jhgjhg -h -a -j -g kjgjh";
    std::string logname = "stdout_test";

    int i = 0;
    for( ; i < 1; ++i )
    {
        // no run it
        SC_ProcessConfig x( compName, instance, runnable, moveable, maxretries, retrywindow, runType, out, err, args,  logname);
        ST_SPointer<SC_ProcessWrapperThread> prc = SC_ProcessWrapperThread::create(std::string("ProcessWrapper_TestThread"), x);
        std::cout << "-----------------------------------------------" << std::endl;
        prc->start();
        //usleep(200000000);
        //prc->stop();
        prc->join();
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << ">>>>>> " << i << " ****************************************" << std::endl;
    }
}

