// #############################
#include "sc_xmlentresolver.hpp"
#include "si_xmlutillog.hpp"
//#include "sa_xmldocument.hpp"
#include "ss_xmlutil.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <si_macros.hpp>

#include <xercesc/framework/LocalFileInputSource.hpp>

using XERCES_CPP_NAMESPACE::InputSource;
using XERCES_CPP_NAMESPACE::XMLString;
using XERCES_CPP_NAMESPACE::LocalFileInputSource;

using rpms::SC_XMLEntResolver;

///////////////////////////////////////////
InputSource * SC_XMLEntResolver::resolveEntity (const XMLCh* const publicId, const XMLCh* const systemId)
{
    TRY(SC_XMLEntResolver::resolveEntity(const XMLCh* const publicId, 
                const XMLCh* const systemId));
        RPMS_TRACE( XML_ENT_LOG, "Resolving entity from publicId [" 
            +  SS_XMLUtil::convertToString(publicId) 
            + "] and systemId [" + SS_XMLUtil::convertToString(systemId) +  "]" );
    
        // we have to create the full path in a raw string before we copnvert 
        // to XMLCh* else we have some weird errs coming out
        std::string rawDtd = SS_Ini::instance()->getString("directories", "dtd") + "/" 
            + SS_XMLUtil::convertToString(systemId);

        RPMS_TRACE(XML_ENT_LOG, "Using DTD [" + rawDtd + "] for this XML");
        
        XMLCh* dtd = XMLString::transcode(rawDtd.c_str());
        LocalFileInputSource *ret = new LocalFileInputSource(dtd);
        XMLString::release(&dtd);
        return ret; 
    CATCH;
}
