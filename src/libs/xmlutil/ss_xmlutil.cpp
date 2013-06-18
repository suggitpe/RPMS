#include "ss_xmlutil.hpp"
#include "si_xmlutillog.hpp"
#include "sx_xmlutil.hpp"
#include "sc_xmlerrhandler.hpp"
#include "sc_xmlentresolver.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>
#include <tools/st_guard.hpp>

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <xalanc/XalanTransformer/XalanTransformer.hpp>


// xerces
using XERCES_CPP_NAMESPACE::XMLPlatformUtils;
using XERCES_CPP_NAMESPACE::XMLException;
using XERCES_CPP_NAMESPACE::DOMException;
using XERCES_CPP_NAMESPACE::XMLString;
using XERCES_CPP_NAMESPACE::XercesDOMParser;
using XERCES_CPP_NAMESPACE::DOMDocument;
using XERCES_CPP_NAMESPACE::MemBufInputSource;
using XERCES_CPP_NAMESPACE::DOMImplementationLS;
using XERCES_CPP_NAMESPACE::DOMImplementation;
using XERCES_CPP_NAMESPACE::DOMImplementationRegistry;
using XERCES_CPP_NAMESPACE::XMLUni;


// xalan
using XALAN_CPP_NAMESPACE::XalanTransformer;

using rpms::SS_XMLUtil;


///////////////////////////////////////////
SS_XMLUtil::~SS_XMLUtil()
{
    TRY(SS_XMLUtil::~SS_XMLUtil());
        if(mInitialised)
        {
            if(mParser)
            {
                delete mParser;
            }

            if(mErrHandler)
            {
                delete mErrHandler;
            }

            if(mEntResolver)
            {
                delete mEntResolver;
            }

            if(mWriter)
            {
                mWriter->release();
            }
        
            XalanTransformer::terminate();
            XMLPlatformUtils::Terminate();
            XalanTransformer::ICUCleanUp();
        }
    CATCH;
}

///////////////////////////////////////////
SS_XMLUtil::SS_XMLUtil()
    : mLock("ss_xmlutil_lock"), mInitialised(false), mParser(0), mErrHandler(0), mEntResolver(0), mWriter(0)
{
    TRY(SS_XMLUtil::SS_XMLUtil());
    CATCH;
}

///////////////////////////////////////////
SS_XMLUtil::SS_XMLUtil( const SS_XMLUtil &aRhs )
    : mLock("dummy"), mInitialised(false), mParser(0), mErrHandler(0), mEntResolver(0), mWriter(0)
{
    TRY(SS_XMLUtil::SS_XMLUtil( const SS_XMLUtil &aRhs ));
        throw SX_XMLUtil("SS_XMLUtil copy ctor not allowed");
    CATCH;
}

///////////////////////////////////////////
SS_XMLUtil& SS_XMLUtil::operator=( const SS_XMLUtil &aRhs )
{
    TRY(SS_XMLUtil::operator=( const SS_XMLUtil &aRhs ));
        throw SX_XMLUtil("SS_XMLUtil assignment operator not allowed here");
    CATCH;
}

///////////////////////////////////////////
void SS_XMLUtil::initialise()
{
    TRY(SS_XMLUtil::initialise());
        ST_Guard<SA_Mutex> x(&mLock);
        if( !mInitialised )
        {
            // set up the platform vars
            try
            {
                XMLPlatformUtils::Initialize();
                XalanTransformer::initialize();
            }
            catch( XMLException& ex )
            {
                throw SX_XMLUtil( convertToString( ex.getMessage()) );
            }
            
            mParser = new XercesDOMParser();
            if(!mParser)
            {
                throw SX_XMLUtil("Failed to create XercesDomParser");
            }

            mErrHandler = new SC_XMLErrHandler();
            if( !mErrHandler )
            {
                throw SX_XMLUtil("Failed to create Error Handler for parser");
            }

            mEntResolver = new SC_XMLEntResolver();
            if( !mEntResolver )
            {
                throw SX_XMLUtil("Failed to create an Entity Resolver for the parser");
            }

            setParserOptions();
            DOMImplementation * domImp = DOMImplementationRegistry::getDOMImplementation(NULL);
            
            mWriter = (dynamic_cast<DOMImplementationLS*>(domImp))->createDOMWriter();
            setWriterOptions();
            mInitialised = true;
        }
    CATCH;
}

///////////////////////////////////////////
DOMDocument * SS_XMLUtil::parseXMLString( const std::string &aXML )
{
    using rpms::convert;
    TRY(SS_XMLUtil::parseXMLString( const std::string &aXML ));
        ST_Guard<SA_Mutex> x(&mLock);
        DOMDocument *ret;
        initialise();
        try
        {
            if( aXML.empty() )
            {
                throw SX_XMLUtil("No document path set .. used def ctor");
            }
            RPMS_TRACE(XML_UTL_LOG, "Parsing xml string");

            MemBufInputSource inputSrc( reinterpret_cast<const XMLByte *>(aXML.c_str()), 
                                    strlen(aXML.c_str()),
                                    "fake_system_id",
                                    false);
            mParser->parse( inputSrc );
            if( mParser->getErrorCount() > 0 )
            {
                RPMS_WARN(XML_UTL_LOG, "Parser error count is [" 
                    + convert<std::string>(mParser->getErrorCount()) + "]");
            }
            else
            {
                RPMS_TRACE(XML_UTL_LOG, "Extracting document from parser");
                ret = dynamic_cast<DOMDocument*>(mParser->getDocument()->cloneNode(true));
            }
            resetParser();
        }
        catch( XMLException& ex )
        {
            throw SX_XMLUtil("XMLException thrown: [" + convertToString( ex.getMessage()) + "]");
        }
        catch( DOMException& dex )
        {
            throw SX_XMLUtil("XMLException thrown: [" + convertToString( dex.msg ) + "]");
        }
        catch( SX_XMLUtil& x )
        {
            throw;
        }
        catch(...)
        {
            throw SX_Unknown("Unknown exception thrown from parser");
        }
        
        if( 0 == ret )
        {
            throw SX_XMLUtil("Failed to cast DOMDocument after clone");
        }

        resetParser(); // you have to do this to stop memory leaks

        return ret;
    CATCH;
}

///////////////////////////////////////////
DOMDocument * SS_XMLUtil::parseXMLFile( const std::string &aFile )
{
    using rpms::convert;
    TRY(SS_XMLUtil::parseXMLFile( const std::string &aFile ));
        ST_Guard<SA_Mutex> x(&mLock);
        DOMDocument *ret;
        initialise();
        try
        {
            if( aFile.empty() )
            {
                throw SX_XMLUtil("No file name passed in from which to parse");
            }
            RPMS_TRACE(XML_UTL_LOG, "Parsing xml file [" + aFile + "]");
            mParser->parse(aFile.c_str() );
            if( mParser->getErrorCount() > 0 )
            {
                RPMS_WARN(XML_UTL_LOG, "Parser error count is [" 
                    + convert<std::string>(mParser->getErrorCount()) + "]");
            }
            else
            {
                // here we actually clone the parsed document so that the parser can act on it
                //      its own and not rely on the calling functions to clean upthe parser.  They
                //      will however be reposible for deleting the returned document.
                RPMS_TRACE(XML_UTL_LOG, "Extracting document from parser");
                ret = dynamic_cast<DOMDocument*>(mParser->getDocument()->cloneNode(true));
            }
        }
        catch(XMLException& ex)
        {
            throw SX_XMLUtil("XMLException thrown: [" + convertToString( ex.getMessage()) + "]");
        }
        catch(DOMException& dex)
        {
            throw SX_XMLUtil("XMLException thrown: [" + convertToString( dex.msg ) + "]");
        }
        catch( SX_XMLUtil& x )
        {
            throw;
        }
        catch(...)
        {
            throw SX_Unknown("Unknown exception thrown from parser");
        }

        if( 0 == ret )
        {
            throw SX_XMLUtil("Failed to cast DOMDocument after clone");
        }

        resetParser(); // you have to do this to stop memory leaks

        return ret;
    CATCH;
}

///////////////////////////////////////////
std::string SS_XMLUtil::serializeXML( const XERCES_CPP_NAMESPACE::DOMDocument * aDoc )
{
    TRY(SS_XMLUtil::serializeXML( const XERCES_CPP_NAMESPACE::DOMDocument * aDoc ));
        XMLCh *output = mWriter->writeToString(*aDoc);
        std::string ret =  SS_XMLUtil::convertToString(output);
        XMLString::release(&output);
        return ret;
    CATCH;
}

///////////////////////////////////////////
// I know this is horrible but the Xerces API has to manage the difference 
//      between 8 bit and 16 bit
std::string SS_XMLUtil::convertToString( const XMLCh *aXMLString )
{
    TRY(SA_XMLDocument::convertToString( const XMLCh *aXMLString ));
        char *pMsg = XMLString::transcode( aXMLString );
        std::string ret(pMsg);
        XMLString::release(&pMsg);
        return ret;
    CATCH;
}

///////////////////////////////////////////
void SS_XMLUtil::resetParser()
{
    TRY(SS_XMLUtil::resetParser());
        ST_Guard<SA_Mutex> x(&mLock);
        mParser->reset();
        mParser->resetDocumentPool();
    CATCH;
}

///////////////////////////////////////////
void SS_XMLUtil::setParserOptions()
{
    TRY(SA_XMLDocument::setParserOptions());
        RPMS_TRACE(XML_UTL_LOG, "Setting parser options");
        std::string validation = SS_Ini::instance()->getString("xmlutil", "parser.validationScheme");
        RPMS_TRACE(XML_UTL_LOG, " - parser validation         [" + validation + "]");
        if( validation == "never" )
        {
            mParser->setValidationScheme( XercesDOMParser::Val_Never );
        }
        else if( validation == "auto" )
        {
            mParser->setValidationScheme( XercesDOMParser::Val_Auto );
        }
        else if( validation == "always" )
        {
            mParser->setValidationScheme( XercesDOMParser::Val_Always );
        }
        else
        {
            throw SX_XMLUtil("Unknown parser validation option [" + validation 
                + "] .. please use never/auto/always");
        }

        bool nsOnOff = SS_Ini::instance()->getBool("xmlutil", "parser.namespaceEnabled");
        RPMS_TRACE(XML_UTL_LOG, " - parser validation         [" + std::string(nsOnOff?"on":"off") + "]");
        mParser->setDoNamespaces( nsOnOff );

        bool scOnOff = SS_Ini::instance()->getBool("xmlutil", "parser.useSchemas");
        RPMS_TRACE(XML_UTL_LOG, " - parser use schemas        [" + std::string(scOnOff?"on":"off") + "]");
        mParser->setDoSchema( scOnOff );

        if( scOnOff )
        {
            bool scfOnOff = SS_Ini::instance()->getBool("xmlutil", "parser.schemaFullCheck");
            RPMS_TRACE(XML_UTL_LOG, " - parser full check schemas [" + std::string(scfOnOff?"on":"off") + "]");
            mParser->setValidationSchemaFullChecking( scfOnOff );
        }

        bool cerOnOff = SS_Ini::instance()->getBool("xmlutil", "parser.createEntityRefNodes");
        RPMS_TRACE(XML_UTL_LOG, " - parser entity ref nodes   [" + std::string(cerOnOff?"on":"off") + "]");
        mParser->setCreateEntityReferenceNodes( cerOnOff );
    
        mParser->setErrorHandler(mErrHandler);
        mParser->setEntityResolver(mEntResolver);
    CATCH;
}

///////////////////////////////////////////
void SS_XMLUtil::setWriterOptions()
{
    TRY(SS_XMLUtil::setWriterOptions());
        XMLCh *enc = XMLString::transcode(SS_Ini::instance()->getString("xmlutil", "parser.encoding").c_str());
        mWriter->setEncoding(enc);
        if (mWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        {
            mWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
        }
        XMLString::release(&enc);
    CATCH;
}

///////////////////////////////////////////
