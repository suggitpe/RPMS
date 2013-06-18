// #####################################
#include "sc_xmlnodeconfig.hpp"
#include "sx_xmlutil.hpp"
#include "si_xmlutillog.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>

using XERCES_CPP_NAMESPACE::XMLString;
using XERCES_CPP_NAMESPACE::DOMNodeList;
using XERCES_CPP_NAMESPACE::DOMElement;
using XERCES_CPP_NAMESPACE::DOMNamedNodeMap;

using rpms::SC_XMLNodeConfig;
using rpms::ST_SPointer;
using rpms::SC_NodeConfig;

/////////// SC_XMLNODECONFIG //////////////
///////////////////////////////////////////
SC_XMLNodeConfig::~SC_XMLNodeConfig()
{
    TRY(SC_XMLNodeConfig::~SC_XMLNodeConfig());
        delete mNodeMap;
    CATCH;
}

///////////////////////////////////////////
SC_XMLNodeConfig::SC_XMLNodeConfig( const std::string &aPath )
    : SA_XMLDocument(), mDocPath(aPath)
{
    TRY(SC_XMLNodeConfig::SC_XMLNodeConfig( const std::string &aPath ));
        mNodeMap = new nodeMap;
    CATCH;
}

///////////////////////////////////////////
void SC_XMLNodeConfig::load()
{
    TRY(SC_XMLNodeConfig::load());
        mNodeMap->clear();
        createTreeFromFile(mDocPath);
        RPMS_TRACE(XML_NOD_LOG, "Extracting RPMS node listing");

        XMLCh * nodelist = XMLString::transcode( SS_Ini::instance()->getString("xml_elemnames", "hostlist").c_str() );
        DOMNodeList *list = getDoc()->getDocumentElement()->getElementsByTagName( nodelist );
        if( 1 < list->getLength() )
        {
            throw SX_XMLUtil("More than one node list defined in [" + mDocPath + "]");
        }

        // get the items in the group
        DOMNodeList *nodes = list->item(0)->getChildNodes();
        for( int i = 0; i < nodes->getLength(); ++i )
        {
            // we are only interested in elements
            DOMElement * cfgNode = dynamic_cast<DOMElement*>( nodes->item(i) );
            if( cfgNode )
            {
                ST_SPointer<SC_NodeConfig> c = SC_NodeConfig::build(cfgNode);
                add( c->getHostName(), c );
            }
        }

        //clean up
        XMLString::release(&nodelist);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLNodeConfig::add( const std::string& aHostName, const ST_SPointer<SC_NodeConfig> &aNodeConfig )
{
    TRY(SC_XMLNodeConfig::add( const std::string& aHostName, const ST_SPointer<SC_NodeConfig> &aNodeConfig ));
        nodeMap::iterator i = mNodeMap->find(aHostName);
        if( i == mNodeMap->end() )
        {
            mNodeMap->insert( nodePair( aHostName, aNodeConfig ) );
        }
        else
        {
            throw SX_XMLUtil("Trying to add a duplicate entry to the Node map [" + aHostName + "]");
        }
    CATCH;
}

///////////////////////////////////////////
void SC_XMLNodeConfig::debug() const
{
    TRY(SC_XMLNodeConfig::debug());
        RPMS_DEBUG(XML_NOD_LOG, "SC_XMLNodeConfig::debug ......................");
        nodeMap::iterator i = mNodeMap->begin();
        while( i != mNodeMap->end() )
        {
            i->second->debug();
            ++i;
        }
        RPMS_DEBUG(XML_NOD_LOG, "..............................................");
    CATCH;
}
///////////////////////////////////////////
std::map<int, ST_SPointer<SC_NodeConfig > > SC_XMLNodeConfig::getElections()
{
    TRY(SC_XMLNodeConfig::getElections());
        std::map<int, ST_SPointer<SC_NodeConfig > > ret;
        nodeMap::iterator i = mNodeMap->begin();
        while( i != mNodeMap->end() )
        {
            if( ret.find( i->second->getRank() ) != ret.end() )
            {
                throw SX_XMLUtil("Found duplicate rankings in node selections");
            }

            // exclude the ones that are not usable
            if( i->second->isUsable() )
            {
                // take a copy
                ret.insert( std::pair<int,ST_SPointer<SC_NodeConfig > >(i->second->getRank(), new SC_NodeConfig( *(i->second) ) ));
            }
            ++i;
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
std::deque<std::string> SC_XMLNodeConfig::getNodeHosts()
{
    TRY(SC_XMLNodeConfig::getNodeHosts());
        std::deque<std::string> ret;
        nodeMap::iterator i = mNodeMap->begin();
        while( i != mNodeMap->end() )
        {
            ret.push_back(i->first);
            ++i;
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
int SC_XMLNodeConfig::getRank( const std::string &aHostName )
{
    TRY(SC_XMLNodeConfig::getRank( const std::string &aHostName ));
        nodeMap::iterator i = mNodeMap->find(aHostName);
        if( i == mNodeMap->end() )
        {
            RPMS_WARN(XML_NOD_LOG, "Could not find rank for hostname [" + aHostName + "]" );
            return -1;
        }
        else
        {
            if( ! i->second->isUsable() )
            {
                return -1;
            }
        }
        return i->second->getRank();
    CATCH;
}

// #######################################################################
/////////// SC_XMLNODECONFIG //////////////
///////////////////////////////////////////
SC_NodeConfig::SC_NodeConfig( const std::string &aHost, const int aRank, const std::string &aType, const bool aUsable )
    : mHostName(aHost), mRank(aRank), mType(_UNKNOWN), mUsable(aUsable)
{
    TRY(SC_NodeConfig::SC_NodeConfig( const std::string &aHost, const int aRank, const std::string &aType, const bool aUsable ));
        if( "linux" == aType )
        {
            mType = _LINUX;
        }
        else if("win32" == aType )
        {
            ///TODO: implement WIN32 types
            mType = _WIN32;
            throw SX_XMLUtil("WIN32 application types are not supported as yet");
        }
        else if( "unix" == aType )
        {
            ///TODO: Implement UNIX types
            mType = _UNIX;
            throw SX_XMLUtil("UNIX application types are not supported as yet");
        }
        else
        {
            throw SX_XMLUtil("Unknown operating system type specified [" + aType + "]");
        }
    CATCH;
}

///////////////////////////////////////////
SC_NodeConfig::~SC_NodeConfig()
{
    TRY(SC_NodeConfig::~SC_NodeConfig());
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_NodeConfig> SC_NodeConfig::build( const XERCES_CPP_NAMESPACE::DOMElement * aElem )
{
    using rpms::convert;
    TRY(SC_NodeConfig::build( const XERCES_CPP_NAMESPACE::DOMElement * aElem ));
        DOMNamedNodeMap *attr = aElem->getAttributes();
        return new SC_NodeConfig(   SA_XMLDocument::getAttribute( attr, "hostname" ), 
                                atoi(SA_XMLDocument::getAttribute(attr, "rank").c_str()), 
                                SA_XMLDocument::getAttribute(attr, "type"),
                                convert<bool>(SA_XMLDocument::getAttribute(attr, "usable")) );
    CATCH;
}

///////////////////////////////////////////
void SC_NodeConfig::debug() const
{
    using rpms::convert;
    TRY(SC_NodeConfig::debug());
        std::string out = "";
        out += "SC_NodeConfig::debug() ............\n";
        out += "\thostname [" + mHostName + "], ";
        out += "rank [" + convert<std::string>(mRank) + "], ";
        switch(mType)
        {
            case _LINUX:
                out += "type [_LINUX], ";
                break;
            case _UNIX:
                out += "type [_UNIX], ";
                break;
            case _WIN32:
                out += "type [_WIN32], ";
                break;
            default:
                out += "type [_UNKNOWN], ";
                break;
        }
        out += "usable [" + std::string(mUsable?"true":"false") +"]";
        RPMS_DEBUG(XML_NOD_LOG, out);
    CATCH;
}

///////////////////////////////////////////
