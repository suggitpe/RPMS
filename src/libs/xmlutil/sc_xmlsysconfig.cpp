// #####################################
#include "sc_xmlsysconfig.hpp"
#include "sx_xmlutil.hpp"
#include "si_xmlutillog.hpp"
#include "ss_xmlutil.hpp"

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>
#include <utilities/st_conversion.hpp>
#include <si_macros.hpp>

using XERCES_CPP_NAMESPACE::XMLString;
using XERCES_CPP_NAMESPACE::DOMNodeList;
using XERCES_CPP_NAMESPACE::DOMElement;

using rpms::SC_XMLSysConfig;
using rpms::ST_SPointer;
using rpms::SC_ProcessConfig;
using rpms::SC_XMLDaemonConfig;
using rpms::convert;

/////////// SC_XMLSYSCONFIG ///////////////
///////////////////////////////////////////
SC_XMLSysConfig::~SC_XMLSysConfig()
{
    TRY(SC_XMLSysConfig::~SC_XMLSysConfig());
        delete mCompMap;
    CATCH;
}

///////////////////////////////////////////
SC_XMLSysConfig::SC_XMLSysConfig( const std::string &aPath )
    : SA_XMLDocument(), mDocPath(aPath)
{
    TRY(SC_XMLSysConfig::SC_XMLSysConfig( const std::string &aPath ));
        mCompMap = new compMap;
    CATCH;
}

///////////////////////////////////////////
void SC_XMLSysConfig::load()
{
    TRY(SC_XMLSysConfig::load());
        mCompMap->clear();
        createTreeFromFile(mDocPath);
        RPMS_TRACE(XML_SYS_LOG, "Extracting RPMS component listing");

        XMLCh * complistName = XMLString::transcode( 
                SS_Ini::instance()->getString("xml_elemnames", "complist").c_str() );

        // we need this as an element
        XERCES_CPP_NAMESPACE::DOMElement *compRoot = 0;
        if( (compRoot = dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement*>(getDoc()->getDocumentElement()->getElementsByTagName( complistName )->item(0))) == 0)
        {
            throw SX_XMLUtil("Unexpected node type retrieved from tree .. expecting an ELEMENT");
        }

        // get the default values
        defMap defaults = loadUpCompDefaults( compRoot );

        // now we have to go through all the components and build a config for each one
        //   we use the default map to make sure that we get them all to build up a correct picture of the node
        RPMS_TRACE(XML_SYS_LOG, "Loading component configs");
        XMLCh * compsName = XMLString::transcode( 
                SS_Ini::instance()->getString("xml_elemnames", "compsname").c_str() );
        XERCES_CPP_NAMESPACE::DOMNodeList *childNodes = 
            compRoot->getElementsByTagName( compsName )->item(0)->getChildNodes();
        for( int i = 0; i < childNodes->getLength(); ++i )
        {
            // only interested in element nodes .. dynamic cast sorts that out for us
            XERCES_CPP_NAMESPACE::DOMElement * compNode = 
                dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement*>( childNodes->item(i) );
            if( compNode )
            {
                std::vector<ST_SPointer<SC_ProcessConfig> > x = SC_ProcessConfig::build(compNode, defaults );
                for( int i = 0; i < x.size(); ++i )
                {
                    add( std::string(x.at(i)->getCompName() +  "_" + convert<std::string>(x.at(i)->getSysInstance())), 
                        x.at(i) );
                }
            }
        }
    
        //clean up
        XMLString::release( &complistName );
        XMLString::release( &compsName );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLSysConfig::add( const std::string& aCompName, const ST_SPointer<SC_ProcessConfig> &aCompMap )
{
    TRY(SC_XMLSysConfig::add( const std::string& aCompName, const ST_SPointer<SC_ProcessConfig> &aCompMap ));
        compMap::iterator i = mCompMap->find(aCompName);
        if( i == mCompMap->end() )
        {
            mCompMap->insert( compPair(aCompName, aCompMap) );
        }
        else
        {
            throw SX_XMLUtil("Trying to add duplicate entry to component map [" + aCompName + "]");
        }
    CATCH;
}

///////////////////////////////////////////
void SC_XMLSysConfig::debug() const
{
    TRY(SC_XMLSysConfig::debug());
        RPMS_DEBUG(XML_SYS_LOG, "SC_XMLSysConfig::debug ......................");
        //debugDoc();
        compMap::iterator i = mCompMap->begin();
        while( i != mCompMap->end() )
        {
            i->second->debug();
            ++i;
        }
        RPMS_DEBUG(XML_SYS_LOG, "..............................................");
    CATCH;
}

///////////////////////////////////////////
std::vector<std::string> SC_XMLSysConfig::getCompList()
{
    TRY(SC_XMLSysConfig::getCompList());
        std::vector<std::string> ret;
        compMap::iterator i = mCompMap->begin();
        while( i!= mCompMap->end() )
        {
            if(i->second->isRunnable())
            {
                ret.push_back(i->first);
            }
            ++i;
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
SC_ProcessConfig SC_XMLSysConfig::getProcessConfig( const std::string &aProcessName )
{
    TRY(SC_XMLSysConfig::getProcessConfig( const std::string &aProcessName ));
        return *mCompMap->find( aProcessName )->second; // will make a copy
    CATCH;
}

///////////////////////////////////////////
std::map<std::string, std::string> SC_XMLSysConfig::loadUpCompDefaults( const XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    TRY(SC_XMLSysConfig::loadUpCompDefaults( const XERCES_CPP_NAMESPACE::DOMElement *aElem ));
        RPMS_TRACE(XML_SYS_LOG, "Loading component defaults");
        defMap ret;
        XMLCh * deflist = XMLString::transcode( SS_Ini::instance()->getString("xml_elemnames", "compdefaults").c_str() );
        XERCES_CPP_NAMESPACE::DOMNodeList *childNodes = getDoc()->getDocumentElement()->getElementsByTagName( deflist )->item(0)->getChildNodes();
        for( int i = 0; i < childNodes->getLength(); ++i )
        {
            if( XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE == childNodes->item(i)->getNodeType() )
            {
                if( ! childNodes->item(i)->hasChildNodes() )
                {
                    throw SX_XMLUtil("Badly constructed XMl for component defaults (no value specified)");
                }
            
                ret.insert( defPair( SS_XMLUtil::convertToString( childNodes->item(i)->getNodeName() ), 
                            SS_XMLUtil::convertToString( childNodes->item(i)->getFirstChild()->getNodeValue() ) ) );
            }
        }

        XMLString::release(&deflist);
        return ret;
    CATCH;
}
// #######################################################################
/////////// SC_XMLSysNodeConfig //////////////////
///////////////////////////////////////////
SC_XMLDaemonConfig::SC_XMLDaemonConfig()
    : SA_XMLDocument(), createFromNew(true)
{
    TRY(SC_XMLDaemonConfig::SC_XMLDaemonConfig());
        mProcs = new procVect;
    CATCH;
}

///////////////////////////////////////////
SC_XMLDaemonConfig::SC_XMLDaemonConfig( const std::string &aXML )
    : SA_XMLDocument(), createFromNew(false), mXML(aXML)
{
    TRY(SC_XMLDaemonConfig::SC_XMLDaemonConfig( const std::string &aXML ));
        mProcs = new procVect;
    CATCH;
}

///////////////////////////////////////////
SC_XMLDaemonConfig::~SC_XMLDaemonConfig()
{
    //TRY(SC_XMLDaemonConfig::~SC_XMLDaemonConfig());
        mProcs->clear();
        delete mProcs;
    //CATCH;
}

///////////////////////////////////////////
void SC_XMLDaemonConfig::load()
{
    TRY(SC_XMLDaemonConfig::load());
        if(createFromNew)
        {
            createTreeFromNewDocument(SS_Ini::instance()->getString("xml", "daemon.config.docname"),
                SS_Ini::instance()->getString("xml", "daemon.config.system"), 
                SS_Ini::instance()->getString("xml", "daemon.config.public") );
        }
        else
        {
            createTreeFromString( mXML );
            buildFromTree();
        }
            
    CATCH;
}

///////////////////////////////////////////
std::string SC_XMLDaemonConfig::createDummyDaemonConfig()
{
    TRY(SC_XMLDaemonConfig::createDummyDaemonConfig());
        return SS_XMLUtil::instance()->serializeXML( getDoc() );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLDaemonConfig::debug() const
{
    TRY(SC_XMLDaemonConfig::debug() const);
        RPMS_DEBUG(XML_SYS_LOG, "SC_XMLDaemonConfig debug:\n" + SS_XMLUtil::instance()->serializeXML( getDoc() ) );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLDaemonConfig::addProcess( const SC_ProcessConfig &aProcConfig )
{
    TRY(SC_XMLDaemonConfig::addProcess( const SC_ProcessConfig &aProcConfig ));
        mProcs->push_back( new SC_ProcessConfig(aProcConfig) ); // take a new one
        XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("process_config");
        loadTreeWithProcessData( aProcConfig, elem );
        // add the new element to the tree
        addChild(elem);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLDaemonConfig::removeMoveableProcesses()
{
    TRY(SC_XMLDaemonConfig::removeMoveableProcesses());
        std::vector<ST_SPointer<SC_ProcessConfig> >::iterator iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            if( iter->data()->isMoveable() )
            {
                mProcs->erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        removeAllElementsForName("process_config"); // remove all the existing elements
        // then pop back in the ones that we know we want to keep
        iter = mProcs->begin();
        while( iter != mProcs->end() )
        {
            XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("process_config");
            loadTreeWithProcessData( *(iter->data()), elem );
            addChild(elem);
            ++iter;
        }
    CATCH;
}

///////////////////////////////////////////
std::vector<ST_SPointer<SC_ProcessConfig> > SC_XMLDaemonConfig::getProcesses()
{
    TRY(SC_XMLDaemonConfig::getProcesses());
        std::vector<ST_SPointer<SC_ProcessConfig> > ret;
        for(int i =0; i < mProcs->size(); ++i )
        {
            ret.push_back( mProcs->at(i) );
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
std::vector<std::string> SC_XMLDaemonConfig::getProcessNames()
{
    TRY(SC_XMLDaemonConfig::getProcessNames());
        std::vector<std::string> ret;
        for(int i =0; i < mProcs->size(); ++i )
        {
            ret.push_back( mProcs->at(i)->getCompName() );
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
void SC_XMLDaemonConfig::buildFromTree()
{
    TRY(SC_XMLDaemonConfig::buildFromTree());
        // this could possibly be more efficient if it were to simply delete the old 
        //      mProcs and assign a new one based on the version passed back.  Seeing 
        //      as it is not run that often I am assuming a few nanoseconds is OK.
        std::vector<ST_SPointer<SC_ProcessConfig> > x = 
            SC_ProcessConfig::buildAllProcesses( getDoc()->getDocumentElement() );
        for( int i = 0; i < x.size(); ++i )
        {
            mProcs->push_back( x.at(i) );
        }
    CATCH;
}

///////////////////////////////////////////
std::string SC_XMLDaemonConfig::getSerializedDoc()
{
    TRY(SC_XMLDaemonConfig::getSerializedDoc());
        return SS_XMLUtil::instance()->serializeXML( getDoc() );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLDaemonConfig::loadTreeWithProcessData( const SC_ProcessConfig &aProcConfig, XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    TRY(SC_XMLDaemonConfig::loadTreeWithProcessData( const SC_ProcessConfig &aProcConfig, XERCES_CPP_NAMESPACE::DOMElement *aElem ));

        // keys
        XMLCh *compNameTag      = XMLString::transcode("compname");
        XMLCh *compInstanceTag  = XMLString::transcode("compinstance");
        XMLCh *runnableTag      = XMLString::transcode("runnable");
        XMLCh *moveableTag      = XMLString::transcode("moveable");
        XMLCh *retryMaxTag      = XMLString::transcode("retrymax");
        XMLCh *retryWindowTag   = XMLString::transcode("retrywindow");
        XMLCh *runTypeTag       = XMLString::transcode("runtype");
        XMLCh *stdOutTag        = XMLString::transcode("stdout");
        XMLCh *stdErrTag        = XMLString::transcode("stderr");
        XMLCh *argsTag          = XMLString::transcode("args");
        XMLCh *logNameTag       = XMLString::transcode("loggingname");

        // values
        XMLCh *compNameVal      = XMLString::transcode(aProcConfig.getCompName().c_str() );
        XMLCh *compInstanceVal  = XMLString::transcode(convert<std::string>(aProcConfig.getSysInstance()).c_str() );
        XMLCh *runnableVal      = XMLString::transcode( (aProcConfig.isRunnable()?"1":"0") );
        XMLCh *moveableVal      = XMLString::transcode( (aProcConfig.isMoveable()?"1":"0") );
        XMLCh *retryMaxVal      = XMLString::transcode(convert<std::string>(aProcConfig.getMaxRetries()).c_str() );
        XMLCh *retryWindowVal   = XMLString::transcode(convert<std::string>(aProcConfig.getRetryWindow()).c_str() );

        XMLCh *runTypeVal;
        if( aProcConfig.getRunType() == SC_ProcessConfig::_CONTINUOUS )
        {
            runTypeVal          = XMLString::transcode("continuous");
        }
        else if( aProcConfig.getRunType() == SC_ProcessConfig::_ONCE )
        {
            runTypeVal          = XMLString::transcode("once");
        }
        else
        {
            runTypeVal          = XMLString::transcode("unknown");
        }

        XMLCh *stdOutVal        = XMLString::transcode(aProcConfig.getStdOut().c_str() );
        XMLCh *stdErrVal        = XMLString::transcode(aProcConfig.getStdErr().c_str() );
        XMLCh *argsVal          = XMLString::transcode(aProcConfig.getArgs().c_str() );
        XMLCh *logNameVal       = XMLString::transcode(aProcConfig.getLoggingName().c_str() );

        aElem->setAttribute(compNameTag,     compNameVal);
        aElem->setAttribute(compInstanceTag, compInstanceVal);
        aElem->setAttribute(runnableTag,     runnableVal);
        aElem->setAttribute(moveableTag,     moveableVal);
        aElem->setAttribute(retryMaxTag,     retryMaxVal);
        aElem->setAttribute(retryWindowTag,  retryWindowVal);
        aElem->setAttribute(runTypeTag,      runTypeVal);
        aElem->setAttribute(stdOutTag,       stdOutVal);
        aElem->setAttribute(stdErrTag,       stdErrVal);
        aElem->setAttribute(argsTag,         argsVal);
        aElem->setAttribute(logNameTag,      logNameVal);

        // keys
        XMLString::release(&compNameTag);
        XMLString::release(&compInstanceTag);
        XMLString::release(&runnableTag);
        XMLString::release(&moveableTag);
        XMLString::release(&retryMaxTag);
        XMLString::release(&retryWindowTag);
        XMLString::release(&runTypeTag);
        XMLString::release(&stdOutTag);
        XMLString::release(&stdErrTag);
        XMLString::release(&argsTag);
        XMLString::release(&logNameTag);
        // values
        XMLString::release(&compNameVal);
        XMLString::release(&compInstanceVal);
        XMLString::release(&runnableVal);
        XMLString::release(&moveableVal);
        XMLString::release(&retryMaxVal);
        XMLString::release(&retryWindowVal);
        XMLString::release(&runTypeVal);
        XMLString::release(&stdOutVal);
        XMLString::release(&stdErrVal);
        XMLString::release(&argsVal);
        XMLString::release(&logNameVal);
    CATCH;
}



// #######################################################################
/////////// SC_ProcessCONFIG //////////////////
///////////////////////////////////////////
SC_ProcessConfig::~SC_ProcessConfig()
{
    TRY(SC_ProcessConfig::~SC_ProcessConfig());
    CATCH;
}

///////////////////////////////////////////
SC_ProcessConfig::SC_ProcessConfig( const std::string &aCompName,
                            const int aSysInstance,
                            const bool aRunnable,
                            const bool aMoveable,
                            const int aRetryMax, 
                            const long aRetryWindow, 
                            const std::string &aRunType,
                            const std::string &aStdOut,
                            const std::string &aStdErr,
                            const std::string &aArgs,
                            const std::string &aLoggingName )
   : mCompName(aCompName), mSysInstance(aSysInstance), mRunnable(aRunnable), mMoveable(aMoveable),
        mRetryMax(aRetryMax), mRetryWindow(aRetryWindow), mRunType(_UNKNOWN), mStdOut(aStdOut), 
        mStdErr(aStdErr), mArgs(aArgs), mLoggingName(aLoggingName)
{
    TRY(SC_ProcessConfig::SC_ProcessConfig( const std::string &aCompName, const int aSysInstance, const bool aRunnable, const bool aMoveable,
                const int aRetryMax, const long aRetryWindow, const std::string &aRunType, const std::string &aStdOut, 
                const std::string &aStdErr, const std::string &aArgs, const std::string &aLoggingName ));
        if( aRunType == "once" )
        {
            mRunType = _ONCE;
        }
        else if( aRunType == "continuous" )
        {
            mRunType = _CONTINUOUS;
        }
    CATCH;
}

///////////////////////////////////////////
SC_ProcessConfig::SC_ProcessConfig( const SC_ProcessConfig &aRhs )
    : mCompName(aRhs.mCompName), mSysInstance(aRhs.mSysInstance), mRunnable(aRhs.mRunnable),
    mMoveable(aRhs.mMoveable), mRetryMax(aRhs.mRetryMax), mRetryWindow(aRhs.mRetryWindow),
    mRunType(aRhs.mRunType), mStdOut(aRhs.mStdOut), mStdErr(aRhs.mStdErr),
    mArgs(aRhs.mArgs), mLoggingName(aRhs.mLoggingName)
{
    TRY(SC_ProcessConfig::SC_ProcessConfig(const SC_ProcessConfig &aRhs) );
    CATCH;
}

///////////////////////////////////////////
SC_ProcessConfig& SC_ProcessConfig::operator=( const SC_ProcessConfig &aRhs )
{
    TRY(SC_ProcessConfig::operator=( const SC_ProcessConfig &aRhs ));
        if( this != &aRhs )
        {
            mCompName       = aRhs.mCompName;
            mSysInstance    = aRhs.mSysInstance;
            mRunnable       = aRhs.mRunnable;
            mMoveable       = aRhs.mMoveable;
            mRetryMax       = aRhs.mRetryMax;
            mRetryWindow    = aRhs.mRetryWindow;
            mRunType        = aRhs.mRunType;
            mStdOut         = aRhs.mStdOut;
            mStdErr         = aRhs.mStdErr;
            mArgs           = aRhs.mArgs;
            mLoggingName    = aRhs.mLoggingName;
        }
        return * this;
    CATCH;
}

///////////////////////////////////////////
bool SC_ProcessConfig::operator==( const SC_ProcessConfig &aRhs )
{
    TRY(SC_ProcessConfig::operator==( const SC_ProcessConfig &aRhs ));
        if(mCompName    != aRhs.mCompName)      return false;
        if(mSysInstance != aRhs.mSysInstance)   return false;
        if(mRunnable    != aRhs.mRunnable)      return false;
        if(mMoveable    != aRhs.mMoveable)      return false;
        if(mRetryMax    != aRhs.mRetryMax)      return false;
        if(mRetryWindow != aRhs.mRetryWindow)   return false;
        if(mRunType     != aRhs.mRunType)       return false;
        if(mStdOut      != aRhs.mStdOut)        return false;
        if(mStdErr      != aRhs.mStdErr)        return false;
        if(mArgs        != aRhs.mArgs)          return false;
        if(mLoggingName != aRhs.mLoggingName)   return false;
        return true;
    CATCH;
}

///////////////////////////////////////////
bool SC_ProcessConfig::operator!=( const SC_ProcessConfig &aRhs )
{
    TRY(SC_ProcessConfig::operator!=( const SC_ProcessConfig &aRhs ));
        return !( *this == aRhs );
    CATCH;
}

///////////////////////////////////////////
std::string SC_ProcessConfig::getInternalName() const
{
    TRY(SC_ProcessConfig::getInternalName());
        //if(!mLoggingName.empty())
        //{
        //    return mLoggingName + "_" + convert<std::string>(mSysInstance);
        //}
        return mCompName + "_" + convert<std::string>(mSysInstance);
    CATCH;
}

///////////////////////////////////////////
std::vector<ST_SPointer<SC_ProcessConfig> > SC_ProcessConfig::build( const XERCES_CPP_NAMESPACE::DOMElement * aElem, std::map<std::string, std::string> &aDefaults )
{
    TRY(SC_ProcessConfig::build( const XERCES_CPP_NAMESPACE::DOMElement * aElem, std::map<std::string, std::string> &aDefaults ));
        // return obj
        std::vector<ST_SPointer<SC_ProcessConfig> > ret;
        XERCES_CPP_NAMESPACE::DOMNamedNodeMap * attr = aElem->getAttributes();
        XERCES_CPP_NAMESPACE::DOMNode * node;
        std::map<std::string, std::string> values;
        std::map<std::string, std::string>::iterator i = aDefaults.begin();
        XERCES_CPP_NAMESPACE::DOMNodeList * list = aElem->getChildNodes();
        int j = 0;

        while( i != aDefaults.end() )
        {
            // bin all the non element nodes
            if( list->item(j) && XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE != list->item(j)->getNodeType() )
            {
                ++j;
                continue;
            }
            // now we get the elements
            node = list->item(j);
            if( node && ( SS_XMLUtil::convertToString(node->getNodeName()) == i->first  ) )
            {
                values.insert( std::pair<std::string, std::string>(i->first, 
                            SS_XMLUtil::convertToString(node->getFirstChild()->getNodeValue()) ));
                ++j;
                ++i;
                continue;
            }
            values.insert( std::pair<std::string, std::string>(i->first, i->second ));
            ++i;
        }

        // now actually create the objects
        for(int i = 0; i < atoi(values.find("instances")->second.c_str()); ++i)
        {
            ret.push_back(new SC_ProcessConfig( SA_XMLDocument::getAttribute(attr, "name"),
                                        i,
                                        convert<bool>(SA_XMLDocument::getAttribute(attr, "runnable")),
                                        convert<bool>(SA_XMLDocument::getAttribute(attr, "moveable")),
                                        atoi(values.find("retry_max")->second.c_str()), 
                                        atol(values.find("retry_window")->second.c_str()), 
                                        values.find("run_type")->second ,
                                        SA_XMLDocument::getAttribute(attr, "stdout"),
                                        SA_XMLDocument::getAttribute(attr, "stderr"),
                                        SA_XMLDocument::getAttribute(attr, "args"),
                                        SA_XMLDocument::getAttribute(attr, "loggingname")));
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
std::vector<ST_SPointer<SC_ProcessConfig> > SC_ProcessConfig::buildAllProcesses( const XERCES_CPP_NAMESPACE::DOMElement * aElem )
{
    TRY(buildAllProcesses( const XERCES_CPP_NAMESPACE::DOMElement * aElem ));
        std::vector<ST_SPointer<SC_ProcessConfig> > ret;
            /// @todo build this boy
        RPMS_TRACE(XML_SYS_LOG, "Building daemon config objects from dom element" );
        XERCES_CPP_NAMESPACE::DOMNodeList * list = aElem->getChildNodes();
        XERCES_CPP_NAMESPACE::DOMNode *node;
        XERCES_CPP_NAMESPACE::DOMElement *elem;
        XERCES_CPP_NAMESPACE::DOMNamedNodeMap *attr;
        int j = list->getLength();
        for(int i = 0; i < j; ++i )
        {
            node = list->item(i);
            if( (elem = dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement*>(node)) != 0)
            {
                attr = elem->getAttributes();
                ret.push_back( new SC_ProcessConfig(
                            SA_XMLDocument::getAttribute(attr, "compname"),
                            atoi(SA_XMLDocument::getAttribute(attr, "compinstance").c_str()),
                            convert<bool>(SA_XMLDocument::getAttribute(attr, "runnable")),
                            convert<bool>(SA_XMLDocument::getAttribute(attr, "moveable")),
                            atoi(SA_XMLDocument::getAttribute(attr, "retrymax").c_str()),
                            atol(SA_XMLDocument::getAttribute(attr, "retrywindow").c_str()),
                            SA_XMLDocument::getAttribute(attr, "runtype"),
                            SA_XMLDocument::getAttribute(attr, "stdout"),
                            SA_XMLDocument::getAttribute(attr, "stderr"),
                            SA_XMLDocument::getAttribute(attr, "args"),
                            SA_XMLDocument::getAttribute(attr, "loggingname")
                        ));
            }
            // else we ignore it as we only are interestedin elements
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessConfig::debug() const
{
    TRY(SC_ProcessConfig::debug());
        std::string out = "";
        out += "SC_ProcessConfig::debug() ............\n";
        out += "\tcompName [" + mCompName + "], ";
        out += "sysInstance [" + convert<std::string>(mSysInstance) + "], ";
        out += "runnable [" + std::string(mRunnable?"true":"false") + "], ";
        out += "moveable [" + std::string(mMoveable?"true":"false") + "], ";
        out += "retryMax [" + convert<std::string>(mRetryMax) + "], ";
        out += "retryWindow [" + convert<std::string>(mRetryWindow) + "],\n";
        switch( mRunType )
        {
            case _CONTINUOUS:
                out += "\trunType [_CONTINUOUS], ";
                break;
            case _ONCE:
                out += "\trunType [_ONCE], ";
                break;
            default:
                out += "\trunType [_UNKNOWN], ";
        }
        out += "stdout [" + mStdOut + "], ";
        out += "stderr [" + mStdErr + "], ";
        out += "loggingname [" + mLoggingName + "], ";
        out += "args [" + mArgs + "]";

        RPMS_TRACE(XML_SYS_LOG, out );
    CATCH;
}

///////////////////////////////////////////
