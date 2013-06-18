#ifndef __si_logging_hpp
#define __si_logging_hpp

#include <logging/ss_loggerfactory.hpp>

#define RPMS_TRACE( aLogType, aMsg )  \
SS_LoggerFactory::instance()->getLogger( (aLogType) )->trace( (aMsg) )

#define RPMS_DEBUG( aLogType, aMsg )  \
SS_LoggerFactory::instance()->getLogger( (aLogType) )->debug( (aMsg) )

#define RPMS_INFO( aLogType, aMsg )  \
SS_LoggerFactory::instance()->getLogger( (aLogType) )->info( (aMsg) )

#define RPMS_WARN( aLogType, aMsg )  \
SS_LoggerFactory::instance()->getLogger( (aLogType) )->warn( (aMsg) )

#define RPMS_ERROR( aLogType, aMsg )  \
SS_LoggerFactory::instance()->getLogger( (aLogType) )->error( (aMsg) )

#define RPMS_FATAL( aLogType, aMsg )  \
SS_LoggerFactory::instance()->getLogger( (aLogType) )->fatal( (aMsg) )


#endif
