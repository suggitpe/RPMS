#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp> // to get the xmlstring
#include <xercesc/dom/DOM.hpp>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

/////////////////////////////////////////////////////////////////////
// pre decl
XERCES_CPP_NAMESPACE::DOMDocument *createNewDoc();
XERCES_CPP_NAMESPACE::DOMDocument *createNewDocWithType();
int getMemorySize();

/////////////////////////////////////////////////////////////////////
int main( int aArgC, char *aArgV[] )
{
    std::cout << "**************** starting" << std::endl;

    for( int i = 0; i < 100; ++i )
    {
        XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
        //XERCES_CPP_NAMESPACE::DOMDocument * doc = createNewDoc();
        XERCES_CPP_NAMESPACE::DOMDocument * doc = createNewDocWithType();
        std::cout << "    - Doing something with the document" << std::endl;
        //delete doc;
        doc->release();
        usleep(100000);
        std::cout << "* Memory (rss): " << getMemorySize() << std::endl;
        XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
        std::cout << "* Memory (rss): " << getMemorySize() << std::endl;
    }
    
    std::cout << "**************** ending" << std::endl;
    return 0;
}

/////////////////////////////////////////////////////////////////////
XERCES_CPP_NAMESPACE::DOMDocument *createNewDocWithType()
{
    // set up strings
    XMLCh *tName = XERCES_CPP_NAMESPACE::XMLString::transcode( "typeName" );
    XMLCh *pub = XERCES_CPP_NAMESPACE::XMLString::transcode( "pubId" );
    XMLCh *sys = XERCES_CPP_NAMESPACE::XMLString::transcode( "sysId" );
    XMLCh *name = XERCES_CPP_NAMESPACE::XMLString::transcode( "rootName" );

    XERCES_CPP_NAMESPACE::DOMImplementation *imp = XERCES_CPP_NAMESPACE::DOMImplementation::getImplementation();
    // create the document type and the doc
    XERCES_CPP_NAMESPACE::DOMDocumentType *type = imp->createDocumentType( tName, pub, sys );
    XERCES_CPP_NAMESPACE::DOMDocument *tmp = imp->createDocument( NULL, name, type );

    std::cout << "    - created a doc" << std::endl;
    // clean up
    XERCES_CPP_NAMESPACE::XMLString::release(&tName);
    XERCES_CPP_NAMESPACE::XMLString::release(&name);
    XERCES_CPP_NAMESPACE::XMLString::release(&pub);
    XERCES_CPP_NAMESPACE::XMLString::release(&sys);
    std::cout << "    - returning doc" << std::endl;
    return tmp;;
}

/////////////////////////////////////////////////////////////////////
XERCES_CPP_NAMESPACE::DOMDocument *createNewDoc()
{
    // set up strings

    XERCES_CPP_NAMESPACE::DOMImplementation *imp = XERCES_CPP_NAMESPACE::DOMImplementation::getImplementation();
    // create the document type and the doc
    XERCES_CPP_NAMESPACE::DOMDocument *tmp = imp->createDocument();

    std::cout << "    - returning doc" << std::endl;
    return tmp;;
}

/////////////////////////////////////////////////////////////////////
int getMemorySize()
{
    char fileName[64];
    int fd;

    snprintf( fileName, sizeof(fileName), "/proc/%d/statm", (int)getpid() );
    fd = open(fileName, O_RDONLY);
    if( fd == -1 )
    {
        return -1;
    }

    char memInfo[128];
    int rval = read( fd, memInfo, sizeof(memInfo)-1 );
    close(fd);
    if( rval <= 0 )
    {
        return -1;
    }
    memInfo[rval] = '\0';
    int rss;
    rval = sscanf( memInfo, "%*d %d", &rss );
    if( rval != 1 )
    {
        return -1;
    }
    return rss * getpagesize() / 1024;

}

/////////////////////////////////////////////////////////////////////
