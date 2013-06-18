//#############################

#include <base/st_spointer.hpp>
#include <testharness/si_testharness.hpp>

using rpms::SA_Logger;

//////////////////////////////////////////////////
// funct decl
void testConsoleLogger();
void testLog4cplusLogger();
void testLogging();
void testMacros();

TEST_BASE;

//////////////////////////////////////////////////
void execute( int aArgC, char *aArgV[] )
{
    //testConsoleLogger();
    //testLog4cplusLogger();
    testLogging();
    testMacros();
}

/////////////////////////////////////////////////
void testMacros()
{
}

/////////////////////////////////////////////////
void testLogging()
{
    ST_SPointer<SA_Logger> c = SS_LoggerFactory::instance()->getLogger(LOGNAME);
    c->trace("This is an application to test generic logging");
    c->trace("START ***************************");
    c->trace("Trace message");
    c->debug("Debug message");
    c->info("Info message");
    c->warn("Warn message");
    c->error("Error message");
    c->fatal("Fatal message");
    c->trace("END ***************************");
}
/*
/////////////////////////////////////////////////
void testLog4cplusLogger()
{
    ST_SPointer<SA_Logger> c = new SC_Log4cplusLogger(LOGNAME);
    c->trace("This is an application to test log4cplus logging");
    c->trace("START ***************************");
    c->trace("Trace message");
    c->debug("Debug message");
    c->info("Info message");
    c->warn("Warn message");
    c->error("Error message");
    c->fatal("Fatal message");
    c->trace("END ***************************");
}

/////////////////////////////////////////////////
void testConsoleLogger()
{
    ST_SPointer<SA_Logger> c = new SC_ConsoleLogger (LOGNAME);
    c->trace("This is an application to test console logging");
    c->trace("START ***************************");
    c->trace("Trace message");
    c->debug("Debug message");
    c->info("Info message");
    c->warn("Warn message");
    c->error("Error message");
    c->fatal("Fatal message");
    c->trace("END ***************************");
}

/////////////////////////////////////////////////
*/
