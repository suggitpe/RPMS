// #####################################
#include "sa_xmldocument.hpp"
#include "si_xmlutillog.hpp"
#include "sx_xmlutil.hpp"
#include "ss_xmlutil.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>

#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

// XERCES
using XERCES_CPP_NAMESPACE::XMLString;
using XERCES_CPP_NAMESPACE::DOMException;
using XERCES_CPP_NAMESPACE::DOMDocument;
using XERCES_CPP_NAMESPACE::DOMElement;
using XERCES_CPP_NAMESPACE::DOMNode;
using XERCES_CPP_NAMESPACE::DOMNodeList;
using XERCES_CPP_NAMESPACE::DOMDocumentType;
using XERCES_CPP_NAMESPACE::DOMImplementation;
// XALAN 
using XALAN_CPP_NAMESPACE::XercesDOMSupport;
using XALAN_CPP_NAMESPACE::XercesParserLiaison;
using XALAN_CPP_NAMESPACE::XercesDOMWrapperParsedSource;
using XALAN_CPP_NAMESPACE::XalanTransformer;
using XALAN_CPP_NAMESPACE::XSLTResultTarget;
using XALAN_CPP_NAMESPACE::XalanCompiledStylesheet;
using XALAN_CPP_NAMESPACE::XSLTInputSource;

using rpms::SA_XMLDocument;

///////////////////////////////////////////
SA_XMLDocument::SA_XMLDocument()
    : mInitialised(false), mDoc(0)
{
    TRY(SA_XMLDocument::SA_XMLDocument());
    CATCH;
}

///////////////////////////////////////////
const DOMDocument * SA_XMLDocument::getDoc() const
{
    TRY(SA_XMLDocument::getDoc() const);
        if( !mDoc )
        {
            throw SX_XMLUtil("Trying to get DOMDocument when we have not created one");
        }
        return mDoc;
    CATCH;
}

///////////////////////////////////////////
SA_XMLDocument::~SA_XMLDocument()
{
    TRY(SA_XMLDocument::~SA_XMLDocument());
        delete mDoc;
    CATCH;
}
///////////////////////////////////////////
void SA_XMLDocument::createTreeFromString(const std::string &aXMLString)
{
    TRY(SA_XMLDocument::createTreeFromString(const std::string &aXMLString));
        if(mDoc)
        {
            delete mDoc;
            mDoc = 0;
        }
        mDoc = SS_XMLUtil::instance()->parseXMLString( aXMLString );
    CATCH;
}

///////////////////////////////////////////
void SA_XMLDocument::createTreeFromNewDocument(const std::string &aRootName, 
        const std::string &aPubId, const std::string &aSysId)
{
    TRY(SA_XMLDocument::createTreeFromNewDocument(const std::string &aRootName, 
                const std::string &aPubId, const std::string &aSysId));
        if(mDoc)
        {
            delete mDoc;
            mDoc = 0;
        }
        mDoc = buildNewDOMDocument(aRootName, aPubId, aSysId);
    CATCH;
}

///////////////////////////////////////////
void SA_XMLDocument::createTreeFromFile( const std::string &aDocumentPath )
{
    TRY(SA_XMLDocument::createTreeFromFile(const std::string &aDocumentPath));
        if(mDoc)
        {
            delete mDoc;
            mDoc = 0;
        }
        mDoc = SS_XMLUtil::instance()->parseXMLFile(aDocumentPath);
    CATCH;
}

///////////////////////////////////////////
bool SA_XMLDocument::transformDoc( const std::string &aXSLPath )
{
    TRY(SA_XMLDocument::transformDoc( const std::string &aXSLPath ));
        mDoc->normalize();
        
        RPMS_TRACE(XML_DOC_LOG, "Creating transformer");
        XercesDOMSupport *support = new XercesDOMSupport();
        XercesParserLiaison *liaison = new XercesParserLiaison();
        XalanTransformer trans;
        XSLTResultTarget out(std::cout);
        const XalanCompiledStylesheet *ss = 0;
        const XSLTInputSource in(aXSLPath.c_str());
        
        RPMS_TRACE(XML_DOC_LOG, "Compiling stylesheet");
        if( 0 != (trans.compileStylesheet(in, ss ))  )
        {
            throw SX_XMLUtil("Failed to compile stylesheet [" + aXSLPath + "]");
        }
        else
        {
            try
            {
                const XercesDOMWrapperParsedSource parsedSrc(mDoc, *liaison, *support);
                RPMS_INFO(XML_DOC_LOG, "Transforming document with [" + aXSLPath + "]");
                trans.transform( parsedSrc, ss, out );
            }
            catch(...)
            {
                return false;
            }
        }

        delete support;
        delete liaison;
        return true;
    CATCH;
}

///////////////////////////////////////////
void SA_XMLDocument::debugDoc() const
{
    TRY(SA_XMLDocument::debugDoc() const);
        if( !mDoc )
        {
            RPMS_DEBUG(XML_DOC_LOG, "No document loaded yet");
        }
        else
        {
            RPMS_DEBUG(XML_DOC_LOG, "XML debug:\n" + SS_XMLUtil::instance()->serializeXML( getDoc() ) );
        }
    CATCH;
}

///////////////////////////////////////////
bool SA_XMLDocument::addChild( XERCES_CPP_NAMESPACE::DOMNode * aNode, XERCES_CPP_NAMESPACE::DOMNode * aParentNode )
{
    TRY(SA_XMLDocument::addChild( XERCES_CPP_NAMESPACE::DOMNode * aNode, XERCES_CPP_NAMESPACE::DOMNode * aParentNode ));
        try
        {
            if( aParentNode == NULL )
            {
                mDoc->getDocumentElement()->appendChild( mDoc->importNode(aNode, true) );
            }
            else
            {
                aParentNode->appendChild( mDoc->importNode(aNode, true) );
            }
        }
        catch( const DOMException& dex )
        {
            throw SX_XMLUtil("XMLException thrown: [" + SS_XMLUtil::convertToString( dex.msg ) + "]");
        }
        return true;
    CATCH;
}

///////////////////////////////////////////
XERCES_CPP_NAMESPACE::DOMElement * SA_XMLDocument::createElementForDoc( const std::string &aName )
{
    TRY(SA_XMLDocument::addElementToRoot( const std::string &aName ));
        XMLCh *elemName = XMLString::transcode( aName.c_str() );
        DOMElement *ret = mDoc->createElement(elemName);
        XMLString::release(&elemName);
        return ret;
    CATCH;
}

///////////////////////////////////////////
int SA_XMLDocument::removeAllElementsForName( const std::string &aTagName )
{
    TRY(SA_XMLDocument::removeAllElementsForName( const std::string &aTagName ));
        XMLCh* tag = XMLString::transcode( aTagName.c_str() );
        DOMNodeList * list = getDoc()->getElementsByTagName(tag);
        DOMNode * node = 0;
        DOMNode * par = 0;
        for( int i = 0; i < list->getLength(); )
        {
            node = list->item(i);
            par = node->getParentNode();
            if( XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE == node->getNodeType() )
            {
                par->removeChild(node);
            }
        }
        XMLString::release(&tag);
        return 0;
    CATCH;
}

///////////////////////////////////////////
XERCES_CPP_NAMESPACE::DOMDocument *SA_XMLDocument::buildNewDOMDocument( const std::string &aRootName, 
                                                  const std::string &aPubId, 
                                                  const std::string &aSysId )
{
    TRY(SA_XMLDocument::buildNewDOMDocument( const std::string &aRootName, 
            const std::string &aPubId, const std::string &aSysId ));
        //create all the parts that we need
        XMLCh *tName = XMLString::transcode( aRootName.c_str() );
        XMLCh *name = XMLString::transcode( aRootName.c_str() );
        XMLCh *pub = XMLString::transcode( aPubId.c_str() );
        XMLCh *sys = XMLString::transcode( aSysId.c_str() );
        XMLCh *enc = XMLString::transcode(SS_Ini::instance()->getString("xmlutil", "parser.encoding").c_str());
        // get the implementation
        DOMImplementation *imp = DOMImplementation::getImplementation();
        // create the document type and the doc
        /// @todo find out what the XERCES guys are doing about this memory leak in DOMImplementation::createDocumentType()
        DOMDocumentType *type = imp->createDocumentType( tName, pub, sys );
        DOMDocument *ret = imp->createDocument( NULL, name, type );
        ret->setEncoding(enc);
        ret->setActualEncoding(enc);
        // clean up
        XMLString::release(&tName);
        XMLString::release(&name);
        XMLString::release(&pub);
        XMLString::release(&sys);
        XMLString::release(&enc);
        return ret;
    CATCH;
}

///////////////////////////////////////////
std::string SA_XMLDocument::getAttribute( const XERCES_CPP_NAMESPACE::DOMNamedNodeMap * aMap, const std::string aAttName )
{
    TRY(SA_XMLDocument::getAttribute( const XERCES_CPP_NAMESPACE::DOMNamedNodeMap * aMap, 
                const std::string aAttName ));
        std::string ret;
        XMLCh* name = XMLString::transcode(aAttName.c_str());
        ret = SS_XMLUtil::convertToString(aMap->getNamedItem(name)->getNodeValue());
        XMLString::release(&name);
        return ret;
    CATCH;
}

///////////////////////////////////////////
