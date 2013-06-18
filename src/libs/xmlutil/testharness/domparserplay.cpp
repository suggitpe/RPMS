#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/dom/DOM.hpp>
#include <iostream>

#include <utilities/ss_ini.hpp>

using std::endl;
using std::cout;
using std::cerr;

XERCES_CPP_NAMESPACE_USE

int main( int argc, char *argv[] )
{
    std::cout << "****************************" << std::endl;
    std::cout << "XML DOM PARSER PLAY" << std::endl;
    std::cout << "****************************" << std::endl;

    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch( const XMLException& ex )
    {
        std::cerr << "error with xml initialisation:" << ex.getMessage() << std::endl;
        return 1;
    }

    std::cout << "Building parser ..." << std::endl;
    XercesDOMParser* parser = new XercesDOMParser();
    if( !parser )
    {
        std::cerr << "no parser" << std::endl;
        return 1;
    }
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setCreateEntityReferenceNodes(false);

    std::cout << "Building error handler" << std::endl;
    ErrorHandler* errHandler = dynamic_cast<ErrorHandler*>(new HandlerBase());
    if( errHandler == 0 )
    {
        std::cerr << "ibad cast" << std::endl;
        return 1;
    } 
    
    if( errHandler == 0 )
    {
        std::cerr << "error with errorhandler caszting:" << std::endl;
        return 1;
    }
    parser->setErrorHandler(errHandler);

    char* xmlFile = "/home/suggitpe/test/RPMS/config/rpms_config.xml";
    try
    {
        parser->parse(xmlFile);
        if( parser->getErrorCount() == 0 )
        {
            std::cerr << "errors in parsing" << std::endl;
            return 1;
        }
        DOMDocument * doc = parser->getDocument();
        if( doc->hasChildNodes() )
        {
            std::cout << "Num nodes: " << doc->getChildNodes()->getLength() << std::endl;
            for( int i = 0; i < doc->getChildNodes()->getLength();++i )
            {
                cout << "\t" << i << " type: " << doc->getChildNodes()->item(i)->getNodeType() << endl;
                if( doc->getChildNodes()->item(i)->getNodeType() == 1 )
                {
                    DOMElement *e = dynamic_cast<DOMElement*>(doc->getChildNodes()->item(i));
                    if( !e ) { cerr << "bad cast" << endl; }
                    if( e->hasChildNodes() )
                    {
                        cout << "\t\telement found with " << e->getChildNodes()->getLength() << " nodes"<< endl;
                    }
                    else
                    {
                        cout << "No child nodes" << endl;
                    }
                }
            }

            cout << "now for the other way" << endl;
            XMLCh * name = XMLString::transcode("system_components");
            DOMNodeList *list = doc->getDocumentElement()->getElementsByTagName( name );
            XMLString::release(&name);
            
            std::cout << "Got list [" << list->getLength() << "]" << std::endl;
            for( XMLSize_t i =0; i < list->getLength(); ++i )
            {
                std::cout << "." << std::endl;
            }
        }
        else
        {
            std::cout << "no child nodes" << std::endl;
        }
    }
    catch( const XMLException& ex )
    {
        std::cerr << "error with xml parser:" << ex.getMessage() << std::endl;
        return 1;
    }
    catch( const DOMException& dex )
    {
        std::cerr << "error with xml parser:" << dex.msg << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "unknown problems" << std::endl;
        return 1;
            
    }
    delete parser;
    delete errHandler;

    std::cout << "****************************" << std::endl;
    std::cout << "" << std::endl;
    return 0;
}
