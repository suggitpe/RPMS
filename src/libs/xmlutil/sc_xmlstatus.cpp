//#############################
#include "sc_xmlstatus.hpp"
#include "si_xmlutillog.hpp"
#include "sx_xmlutil.hpp"
#include "ss_xmlutil.hpp"
#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>
#include <utilities/ss_ini.hpp>
#include <utilities/sc_datetime.hpp>
#include <logging/si_logging.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>

using XERCES_CPP_NAMESPACE::XMLString;

using rpms::SC_XMLStatus;
using rpms::SC_DaemonStatus;
using rpms::SC_SystemStatus;
using rpms::SC_ComponentStatus;

///////////////////////////////////////////
SC_XMLStatus::SC_XMLStatus()
    : SA_XMLDocument(), mStatus(0), createdFromNew(true)
{
    TRY(SA_XMLDocument::SA_XMLDocument());
    CATCH;
}

///////////////////////////////////////////
SC_XMLStatus::SC_XMLStatus( const ST_SPointer<SC_DaemonStatus> &aStatus )
    : SA_XMLDocument(), mStatus(aStatus), createdFromNew(true)
{
    TRY(SC_XMLStatus::SC_XMLStatus( const ST_SPointer<SC_DaemonStatus> &aStatus ));
    CATCH;
}

///////////////////////////////////////////
SC_XMLStatus::SC_XMLStatus( const std::string aXMLStatus )
    : SA_XMLDocument(), mStatus(0), createdFromNew(false), mXML(aXMLStatus)
{
    TRY(SC_XMLStatus::SC_XMLStatus( const std::string aXMLStatus ));
    CATCH;
}


///////////////////////////////////////////
SC_XMLStatus::~SC_XMLStatus()
{
    TRY(SC_XMLStatus::~SC_XMLStatus());
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::load()
{
    TRY(SC_XMLStatus::load());
        if(createdFromNew)
        {
            createTreeFromNewDocument(SS_Ini::instance()->getString("xml", "daemon.status.docname"), 
                SS_Ini::instance()->getString("xml", "daemon.status.system"), 
                SS_Ini::instance()->getString("xml", "daemon.status.public") );
            if(! mStatus.empty())
            {
                XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("daemon_status");
                loadDaemonData(elem);
                addChild(elem);
            }
        }
        else
        {
            createTreeFromString(mXML);
        }
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::loadSystemData( XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    TRY(SC_XMLStatus::loadSystemData( XERCES_CPP_NAMESPACE::DOMElement *aElem ));
        SC_SystemStatus i;
        i.load();
        XMLCh *sysNameTag = XMLString::transcode("systemname");
        XMLCh *instTag = XMLString::transcode("instanceid");
        XMLCh *dateTag = XMLString::transcode("date");
        XMLCh *sysNameVal = XMLString::transcode(i.getSystemName().c_str());
        XMLCh *instVal = XMLString::transcode(i.getInstanceId().c_str());
        XMLCh *dateVal = XMLString::transcode(i.getNowDate().c_str());
        //now set up the attributes
        aElem->setAttribute(sysNameTag, sysNameVal);
        aElem->setAttribute(instTag, instVal);
        aElem->setAttribute(dateTag, dateVal);
            
        XMLString::release(&sysNameTag);
        XMLString::release(&instTag);
        XMLString::release(&dateTag);
        XMLString::release(&sysNameVal);
        XMLString::release(&instVal);
        XMLString::release(&dateVal);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::loadDaemonData( XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    TRY(loadDaemonData( XERCES_CPP_NAMESPACE::DOMElement *aElem ));
        // set up all the values
        XMLCh *hostTag = XMLString::transcode("hostname");
        XMLCh *regTag = XMLString::transcode("registered");
        XMLCh *runningTag = XMLString::transcode("running");
        XMLCh *dirHostTag = XMLString::transcode("directorhost");
        XMLCh *trueVal = XMLString::transcode("1");
        XMLCh *falseVal = XMLString::transcode("0");
        XMLCh *hostVal = XMLString::transcode(mStatus->getHostName().c_str());
        // add all the attributes
        // set hostname
        aElem->setAttribute(hostTag, hostVal);
        // set registered
        if(mStatus->isRegistered()) {aElem->setAttribute(regTag, trueVal);}
        else {aElem->setAttribute(regTag, falseVal);}
        // set running
        if(mStatus->isRunning()) {aElem->setAttribute(runningTag, trueVal);}
        else {aElem->setAttribute(runningTag, falseVal);}
        // set dirhost
        if(mStatus->isDirHost()) {aElem->setAttribute(dirHostTag, trueVal);}
        else {aElem->setAttribute(dirHostTag, falseVal);}

        // cleanup
        XMLString::release(&hostTag);
        XMLString::release(&regTag);
        XMLString::release(&runningTag);
        XMLString::release(&dirHostTag);
        XMLString::release(&trueVal);
        XMLString::release(&falseVal);
        XMLString::release(&hostVal);

        loadComponentData(aElem);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::loadComponentData( XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    using rpms::convert;
    TRY(SC_XMLStatus::loadComponentData( XERCES_CPP_NAMESPACE::DOMElement *aElem ));
        // set up the tag names for the tree
        XMLCh *compTag      = XMLString::transcode("compname");
        XMLCh *instTag      = XMLString::transcode("sysinstance");
        XMLCh *runningTag   = XMLString::transcode("running");
        XMLCh *startTag     = XMLString::transcode("starttime");
        XMLCh *runnableTag  = XMLString::transcode("runnable");
        XMLCh *maxRetryTag  = XMLString::transcode("maxretries");
        XMLCh *moveableTag  = XMLString::transcode("moveable");
        XMLCh *runTypeTag   = XMLString::transcode("runtype");
        XMLCh *lognameTag   = XMLString::transcode("logname");
        XMLCh *pidTag       = XMLString::transcode("pid");
        XMLCh *rssTag       = XMLString::transcode("rss");
        XMLCh *rssAvgTag    = XMLString::transcode("rssavg");
        XMLCh *cpuTag       = XMLString::transcode("cpu");
        XMLCh *cpuAvgTag    = XMLString::transcode("cpuavg");
        XMLCh *trueVal      = XMLString::transcode("1");
        XMLCh *falseVal     = XMLString::transcode("0");
    
        std::vector<ST_SPointer<SC_ComponentStatus> > vec = mStatus->getComponents();
        int sz = vec.size();
        for( int i = 0; i < sz; ++i )
        {
            XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("component_status");


            XMLCh *compVal      = XMLString::transcode( vec.at(i)->getCompName().c_str() );
            XMLCh *instVal      = XMLString::transcode( convert<std::string>(vec.at(i)->getInstance()).c_str() );
            XMLCh *startVal     = XMLString::transcode( vec.at(i)->getStartTime().c_str() );
            XMLCh *maxRetryVal  = XMLString::transcode( convert<std::string>(vec.at(i)->getMaxRetries()).c_str() );
            XMLCh *runTypeVal   = XMLString::transcode( vec.at(i)->getRunType().c_str());
            XMLCh *lognameVal   = XMLString::transcode( vec.at(i)->getLogName().c_str());
            XMLCh *pidVal       = XMLString::transcode( convert<std::string>(vec.at(i)->getPid()).c_str() );
            XMLCh *rssVal       = XMLString::transcode( convert<std::string>(vec.at(i)->getRss()).c_str() );
            XMLCh *rssAvgVal    = XMLString::transcode( convert<std::string>(vec.at(i)->getRssAvg()).c_str() );
            XMLCh *cpuVal       = XMLString::transcode( convert<std::string>(vec.at(i)->getCpuTime()).c_str() );
            XMLCh *cpuAvgVal    = XMLString::transcode( convert<std::string>(vec.at(i)->getCpuAvg()).c_str() );

            elem->setAttribute(compTag,     compVal );
            elem->setAttribute(instTag,     instVal );
            elem->setAttribute(startTag,    startVal );
            elem->setAttribute(maxRetryTag, maxRetryVal );
            elem->setAttribute(runTypeTag,  runTypeVal );
            elem->setAttribute(lognameTag,  lognameVal );
            elem->setAttribute(pidTag,      pidVal );
            elem->setAttribute(rssTag,      rssVal );
            elem->setAttribute(rssAvgTag,   rssAvgVal );
            elem->setAttribute(cpuTag,      cpuVal );
            elem->setAttribute(cpuAvgTag,   cpuAvgVal );
            if( vec.at(i)->isRunning() ) elem->setAttribute(runningTag, trueVal );
                else elem->setAttribute(runningTag, falseVal );
            if( vec.at(i)->isMoveable() ) elem->setAttribute(moveableTag, trueVal );
                else elem->setAttribute(moveableTag, falseVal );
            if(vec.at(i)->isRunnable()) elem->setAttribute(runnableTag, trueVal);
                else elem->setAttribute(runnableTag, falseVal);

            addChild( elem, aElem );

            // clean up
            XMLString::release(&compVal);
            XMLString::release(&instVal);
            XMLString::release(&startVal);
            XMLString::release(&maxRetryVal);
            XMLString::release(&runTypeVal);
            XMLString::release(&lognameVal);
            XMLString::release(&pidVal);
            XMLString::release(&rssVal);
            XMLString::release(&rssAvgVal);
            XMLString::release(&cpuVal);
            XMLString::release(&cpuAvgVal);
        }

        XMLString::release(&compTag);
        XMLString::release(&instTag);
        XMLString::release(&runningTag);
        XMLString::release(&startTag);
        XMLString::release(&runnableTag);
        XMLString::release(&maxRetryTag);
        XMLString::release(&moveableTag);
        XMLString::release(&runTypeTag);
        XMLString::release(&lognameTag);
        XMLString::release(&pidTag);
        XMLString::release(&rssTag);
        XMLString::release(&rssAvgTag);
        XMLString::release(&cpuTag);
        XMLString::release(&cpuAvgTag);
        XMLString::release(&trueVal);
        XMLString::release(&falseVal);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::debug() const
{
    TRY(SC_XMLStatus::debug() const);
        RPMS_DEBUG(XML_STS_LOG, "SC_XMLStatus debug:\n" + SS_XMLUtil::instance()->serializeXML( getDoc() ) );
    CATCH;
}

///////////////////////////////////////////
std::string SC_XMLStatus::getSerializedDoc()
{
    TRY(SC_XMLStatus::getSerializedDoc());
        return SS_XMLUtil::instance()->serializeXML( getDoc() );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::addDaemonData( XERCES_CPP_NAMESPACE::DOMNode * aNode )
{
    TRY(SC_XMLStatus::addDaemonData( XERCES_CPP_NAMESPACE::DOMNode * aNode ));
        addChild(aNode);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLStatus::addSystemData()
{
    TRY(SC_XMLStatus::addSystemData());
        XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("system_status");
        try
        {
            loadSystemData(elem);
            addChild(elem);
        }
        catch( SX_Exception &ex )
        {
            RPMS_ERROR(XML_STS_LOG, "Collecting system data caused exception:\n" + ex.reason());
        }
    CATCH;
}

///////////////////////////////////////////
XERCES_CPP_NAMESPACE::DOMNode* SC_XMLStatus::getStatusNode()
{
    TRY(SC_XMLStatus::getStatusNode());
        XMLCh * statName = XMLString::transcode( 
                SS_Ini::instance()->getString("xml_elemnames", "statusnodename").c_str() );
        XERCES_CPP_NAMESPACE::DOMNode * ret = getDoc()->getDocumentElement()->getElementsByTagName( statName )->item(0);
        XMLString::release(&statName);
        return ret;
    CATCH;
}

///////////////////////////////////////////
// #######################################################################

///////////////////////////////////////////
SC_DaemonStatus::SC_DaemonStatus( const std::string &aHostName, const bool aRegistered, 
        const bool aRunning, const bool aDirectorHost )
    : mHostName(aHostName), mRegistered(aRegistered), mRunning(aRunning), 
        mDirectorHost(aDirectorHost)
{
    TRY(SC_DaemonStatus::SC_DaemonStatus(const std::string &aHostName, const bool aRegistered, 
                const bool aRunning, const bool aDirectorHost));
        mComponents = new std::vector<ST_SPointer<SC_ComponentStatus> >;
    CATCH;
}

///////////////////////////////////////////
SC_DaemonStatus::~SC_DaemonStatus()
{
    TRY(SC_DaemonStatus::~SC_DaemonStatus());
        delete mComponents;
    CATCH;
}

///////////////////////////////////////////
void SC_DaemonStatus::addComponentStatus( const ST_SPointer<SC_ComponentStatus> &aCompStat )
{
    TRY(SC_DaemonStatus::addComponentStatus( const ST_SPointer<SC_ComponentStatus> &aCompStat ));
        mComponents->push_back( aCompStat );
    CATCH;
}

///////////////////////////////////////////
// #######################################################################

///////////////////////////////////////////
SC_SystemStatus::SC_SystemStatus()
{
    TRY(SC_SystemStatus::SC_SystemStatus());
    CATCH;
}

///////////////////////////////////////////
SC_SystemStatus::~SC_SystemStatus()
{
    TRY(SC_SystemStatus::~SC_SystemStatus());
    CATCH;
}

///////////////////////////////////////////
void SC_SystemStatus::load()
{
    TRY(SC_SystemStatus::load());
        mSystemName = SS_Ini::instance()->getString("general", "system.name");
        mInstanceId = SS_Ini::instance()->getString("general", "system.instanceid");
        mNowDate = SC_DateTime::now().toString();
    CATCH;
}

// #######################################################################
///////////////////////////////////////////
SC_ComponentStatus::SC_ComponentStatus( const ST_SPointer<SC_ProcessConfig> &aProcCfg, const bool aRunning, 
        const std::string &aStartTime, const int aRetries, const int aPid, 
        const int aRss, const double aRssAvg, const long aCpuTime, const double aCpuAvg )
    : mCompName( aProcCfg->getCompName() ), 
        mInstance( aProcCfg->getSysInstance()), 
        mRunning(aRunning),
        mStartTime(aStartTime), 
        mRunnable( aProcCfg->isRunnable()), 
        mRetryMax( aProcCfg->getMaxRetries()),
        mMoveable( aProcCfg->isMoveable()),
        mLogName( aProcCfg->getLoggingName()),
        mPid(aPid),
        mRss(aRss),
        mRssAvg(aRssAvg),
        mCpuTime(aCpuTime),
        mCpuAvg(aCpuAvg)
{
    TRY(SC_ComponentStatus::SC_ComponentStatus(const ST_SPointer<SC_ProcessConfig> &aProcCfg, 
                const bool aRunning, const std::string &aStartTime, const int aRetries, 
                const int aPid, const int aRss, const double aRssAvg const long aCpuTime, const double aCpuAvg ) );
        if( aProcCfg->getRunType() == SC_ProcessConfig::_ONCE )
        {
            mRunType = "once";
        }
        else if( aProcCfg->getRunType() == SC_ProcessConfig::_ONCE )
        {
            mRunType = "continuous";
        }
        else
        {
            mRunType = "unknown";
        }
    CATCH;
}

SC_ComponentStatus::~SC_ComponentStatus()
{
    TRY(SC_ComponentStatus::SC_ComponentStatus());
    CATCH;
}


///////////////////////////////////////////
