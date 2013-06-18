//#############################
#include "sc_xmlerrhandler.hpp"
#include "si_xmlutillog.hpp"
//#include "sa_xmldocument.hpp"
#include "sx_xmlutil.hpp"
#include "ss_xmlutil.hpp"

#include <utilities/ss_ini.hpp>
#include <utilities/st_conversion.hpp>
#include <logging/si_logging.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <si_macros.hpp>

using rpms::SC_XMLErrHandler;
using rpms::convert;

///////////////////////////////////////////
SC_XMLErrHandler::SC_XMLErrHandler()
    : mErrsFound(false)
{
    TRY(SC_XMLErrHandler::SC_XMLErrHandler());
        mFailOnErr = SS_Ini::instance()->getBool("xmlutil", "parser.failOnError");
    CATCH;
}

///////////////////////////////////////////
SC_XMLErrHandler::~SC_XMLErrHandler()
{
    TRY(SC_XMLErrHandler::~SC_XMLErrHandler());
    CATCH;
}

///////////////////////////////////////////
void SC_XMLErrHandler::warning(const XERCES_CPP_NAMESPACE::SAXParseException& ex )
{
    TRY(SC_XMLErrHandler::warning(const XERCES_CPP_NAMESPACE::SAXParseException& ex ));
        std::string err = SS_XMLUtil::convertToString( ex.getMessage() );
        RPMS_WARN( XML_ERR_LOG, "Warning message thrown [" + err + "], at line [" + convert<std::string>(ex.getLineNumber()-1) + "]" );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLErrHandler::error(const XERCES_CPP_NAMESPACE::SAXParseException& ex )
{
    TRY(SC_XMLErrHandler::error(const XERCES_CPP_NAMESPACE::SAXParseException& ex ));
        mErrsFound = true;
        std::string err = "Fatal parse exception thrown [";
        err += SS_XMLUtil::convertToString( ex.getMessage() );
        err += "] \n\t\tat line number [";
        err += convert<std::string>(ex.getLineNumber()-1);
        err += "] in [";
        err += SS_XMLUtil::convertToString(ex.getSystemId());
        err += "]";

        if(mFailOnErr)
        {
            RPMS_ERROR( XML_ERR_LOG, err + " ... exiting" );
            exit(1);
        }
        else
        {
            RPMS_ERROR( XML_ERR_LOG, err + " ... continuing" );
        }
    CATCH;
}

///////////////////////////////////////////
void SC_XMLErrHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& ex)
{
    TRY(SC_XMLErrHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& ex));
        mErrsFound = true;
        std::string err = "Fatal parse exception thrown [";
        err += SS_XMLUtil::convertToString( ex.getMessage() );
        err += "] \n\t\tat line number [";
        err += convert<std::string>(ex.getLineNumber()-1);
        err += "] in [";
        err += SS_XMLUtil::convertToString(ex.getSystemId());
        err += "]";

        if(mFailOnErr)
        {
            RPMS_FATAL( XML_ERR_LOG, err + " ... exiting" );
            exit(1);
        }
        else
        {
            RPMS_FATAL( XML_ERR_LOG, err + " ... continuing" );
        }
    CATCH;
}

///////////////////////////////////////////
void SC_XMLErrHandler::resetErrors()
{
    mErrsFound = false;
}

///////////////////////////////////////////
