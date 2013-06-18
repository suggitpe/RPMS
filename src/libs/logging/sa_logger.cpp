//#############################
#include "sa_logger.hpp"
#include <si_macros.hpp>

using rpms::SA_Logger;

///////////////////////////////////////////
SA_Logger::SA_Logger(  const std::string &aLoggerName  )
    : mName(aLoggerName), mLock("SA_Logger")
{
    TRY(SA_Logger::SA_Logger(  const std::string &aLoggerName  ));
    CATCH;
}

///////////////////////////////////////////
SA_Logger::~SA_Logger()
{
    TRY(SA_Logger::~SA_Logger());
    CATCH;
}

///////////////////////////////////////////
