//#############################

//#include <log4cplus/logger.h>
//#include <log4cplus/configurator.h>

#include <utilities/ss_ini.hpp>
#include <base/sx_exception.hpp>

#include <logging/si_logging.hpp>

#include <iostream>

//using rpms::SA_Logger;
using rpms::SS_Ini;
using rpms::SS_LoggerFactory;
using rpms::SX_Exception;

//////////////////////////////////////////////////
// funct decl
void testLoad();
void testMacros();

//////////////////////////////////////////////////
int main()
{
    try
    {
        SS_Ini::instance()->initialise("/home/suggitpe/codebase/RPMS/build/libs/logging/testharness/logging.ini");
        //testLoad();
        testMacros();
    }
    catch( SX_Exception &aEx )
    {
        std::cerr << aEx.reason() << std::endl;
    }
    return 0;
}

/////////////////////////////////////////////////
void testMacros()
{
    RPMS_TRACE("logging", "test trace message");
    RPMS_DEBUG("logging", "test debug message");
    RPMS_INFO("logging", "test info message");
    RPMS_WARN("logging", "test warn message");
    RPMS_ERROR("logging", "test error message");
    RPMS_FATAL("logging", "test fatal message");
}

/////////////////////////////////////////////////
void testLoad()
{
    //log4cplus::PropertyConfigurator::doConfigure(SS_Ini::instance()->getString("logging", "log4cplus.cfg.file"));
    //log4cplus::Logger logger = log4cplus::Logger::getInstance("LOGGING");
    //LOG4CPLUS_INFO(logger, "Initialized");
}

/////////////////////////////////////////////////
