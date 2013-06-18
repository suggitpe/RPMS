//#############################

#include <testharness/si_testharness.hpp>
#include <base/st_spointer.hpp>

#include <xmlutil/sc_xmlnodeconfig.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>

#define MAX_RUN1 1
#define MAX_RUN2 0

TEST_BASE;

using rpms::SA_XMLDocument;
using rpms::SC_XMLNodeConfig;
using rpms::SC_XMLSysConfig;
using rpms::SC_ProcessConfig;

///////////////////////////////////////////
void testBasicXML();
void testProcCfg();
void memCheckForReload();

///////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    RPMS_TRACE( LOGNAME, "This is an application to test objects");
    RPMS_INFO( LOGNAME, "START ***************************");
    //testBasicXML();
    //testProcCfg();
    memCheckForReload();
    RPMS_INFO( LOGNAME, "END   ***************************");
}

///////////////////////////////////////////
void memCheckForReload()
{
    SC_XMLSysConfig cfg( "/home/suggitpe/test/RPMS/" + std::string(::getenv("RPMS_VER")) + "/config/rpms_config.xml" );
    for( int i = 0; i < 1000; ++i )
    {
        cfg.load();
        cfg.debug();
        usleep(1000000);
    }
}

///////////////////////////////////////////
void testProcCfg()
{
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
    SC_ProcessConfig x( compName, instance, runnable, moveable, maxretries, retrywindow, runType, out, err, args,  logname);

    std::string compName1 = "stdouttest";
    int instance1 = 0;
    bool runnable1 = true;
    bool moveable1 = true;
    int maxretries1 = 2;
    long retrywindow1 = 100;
    std::string runType1 = "continuous";
    std::string out1 = "file";
    std::string err1 = "off";
    std::string args1 = "-g jhgjhg -h -a -j -g kjgjh";
    std::string logname1 = "stdout_test";
    SC_ProcessConfig x1( compName1, instance1, runnable1, moveable1, maxretries1, retrywindow1, runType1, out1, err1, args1,  logname1);

    SC_ProcessConfig x2 = x1;
    SC_ProcessConfig x3(x);

    // ***********************************
    // ...............    
    if( x == x )
        RPMS_TRACE( LOGNAME, "x  == x  is true");
    else
        RPMS_TRACE( LOGNAME, "x  == x  is false");
    // ...............    
    if( x == x2 )
        RPMS_TRACE( LOGNAME, "x  == x2 is true");
    else
        RPMS_TRACE( LOGNAME, "x  == x2 is false");
    // ...............    
    if( x == x3 )
        RPMS_TRACE( LOGNAME, "x  == x3 is true");
    else
        RPMS_TRACE( LOGNAME, "x  == x3 is false");
    // ...............    
    if( x1 == x2 )
        RPMS_TRACE( LOGNAME, "x1 == x2 is true");
    else
        RPMS_TRACE( LOGNAME, "x1 == x2 is false");
    // ...............    
    if( x2 == x3 )
        RPMS_TRACE( LOGNAME, "x2 == x3 is true");
    else
        RPMS_TRACE( LOGNAME, "x2 == x3 is false");
    // ...............    

    
}

///////////////////////////////////////////
void testBasicXML()
{
    RPMS_TRACE( LOGNAME, "This is an application to test objects");
    RPMS_INFO( LOGNAME, "START ***************************");

    // we do this a few times so that we can test for any memory leaks
    std::string s = SS_Ini::instance()->getString("directories", "config")
        + "/" + SS_Ini::instance()->getString("director", "director.xml.appconfig");
    ////////////////////////////////////////////////////
    RPMS_INFO( LOGNAME, "TEST 1 ***************************");
    {
        ST_SPointer<SA_XMLDocument> d = new SC_XMLNodeConfig(s);
        for( int i = 0; i < MAX_RUN1; ++i )
        {
            d->load();
            d->debug();
        }
    }

    ////////////////////////////////////////////////////
    RPMS_INFO( LOGNAME, "TEST 2 ***************************");
    {
        ST_SPointer<SA_XMLDocument> d = new SC_XMLSysConfig(s);
        for( int i = 0; i < MAX_RUN2; ++i )
        {
            d->load();
            d->debug();
        }
    }
}
