//#############################
#include "ss_streamreaderfactory.hpp"
#include "sa_streamreader.hpp"
#include "sx_streamreader.hpp"
#include "sc_streamtofilereader.hpp"
#include "sc_streamtonullreader.hpp"
#include "sc_streamtostreamreader.hpp"
#include "si_streamreaderlog.hpp"

#include <si_macros.hpp>
#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <utilities/sc_filesystemhelper.hpp>

using rpms::SS_StreamReaderFactory;
using rpms::ST_SPointer;
using rpms::SA_StreamReader;

///////////////////////////////////////////
SS_StreamReaderFactory::~SS_StreamReaderFactory()
{
    TRY(SS_StreamReaderFactory::~SS_StreamReaderFactory());
    CATCH;
}

///////////////////////////////////////////
SS_StreamReaderFactory::SS_StreamReaderFactory()
{
    TRY(SS_StreamReaderFactory::SS_StreamReaderFactory());
    CATCH;
}

///////////////////////////////////////////
void SS_StreamReaderFactory::createSpaceForLogFile( const std::string &aLogFileName )
{
    TRY(SS_StreamReaderFactory::createSpaceForLogFile( const std::string &aLogFileName ));
        std::string logDir = SS_Ini::instance()->getString("directories", "log");
        RPMS_TRACE(SRD_FAC_LOG, "Creating logfile [" + logDir +"/" + aLogFileName + "]");

        if( !SC_FileSystemHelper::createDirectory( logDir ) )
        {
            throw SX_StreamReader("Failed to create directory [" + logDir + "]");
        }
        
        if( !SC_FileSystemHelper::ageLogFile(logDir, aLogFileName) )
        {
            throw SX_StreamReader("Failed to age logfile [" + logDir + "/" + aLogFileName + "]");
        }

    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SA_StreamReader> SS_StreamReaderFactory::getReader(const int aPipe, const std::string aReaderType, const EStreamType aStreamType, const std::string &aProcessLogName, const std::string &aProcessInstance )
{
    TRY(SS_StreamReaderFactory::getReader(const int aPipe, const std::string aReaderType, const EStreamType aStreamType, const std::string &aProcessLogName, const std::string &aProcessInstance ));
        RPMS_TRACE(SRD_FAC_LOG, "Building a [" + aReaderType + "] stream reader");
        ST_SPointer<SA_StreamReader> ret;
        if( "file" == aReaderType )
        {
            std::string logDir = SS_Ini::instance()->getString("directories", "log");
            std::string logFile( aProcessLogName + "_" +  aProcessInstance);
            if( aStreamType == _STDOUT)
            {
                return new SC_StreamToFileReader("FileRead_" + aProcessLogName + "_out", aPipe, "log", logDir + "/" + logFile);
            }
            else
            {
                return new SC_StreamToFileReader("FileRead_" + aProcessLogName + "_err", aPipe, "err", logDir + "/" + logFile);
            }
        }
        else if( "off" == aReaderType )
        {
            return new SC_StreamToNullReader("NullRead_" + aProcessLogName, aPipe);
        }
        else if( "out" == aReaderType )
        {
            if( aStreamType == _STDOUT )
            {
                return new SC_StreamToStreamReader("StreamRead_" + aProcessLogName + "_out", aPipe, true);
            }
            else
            {
                return new SC_StreamToStreamReader("StreamRead_" + aProcessLogName + "_err", aPipe, false);
            }
        }
        else
        {
            throw SX_StreamReader("Unknown stream reader type");
        }
        return ret;
    CATCH;
}

