//#############################
#include "sc_xmlprocstats.hpp"
#include "si_xmlutillog.hpp"
#include "ss_xmlutil.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>

#include <xmlutil/sc_xmlsysconfig.hpp>
#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>

using XERCES_CPP_NAMESPACE::XMLString;
using XERCES_CPP_NAMESPACE::DOMNodeList;
using XERCES_CPP_NAMESPACE::DOMElement;

using rpms::SC_CompStat;
using rpms::SC_ProcStats;
using rpms::SC_XMLProcStats;
using rpms::ST_SPointer;

///////////////////////////////////////////
SC_XMLProcStats::SC_XMLProcStats()
    : SA_XMLDocument(), mStats(0), createdFromNew(true)
{
    TRY(SC_XMLProcStats::SC_XMLProcStats());
    CATCH;
}

///////////////////////////////////////////
SC_XMLProcStats::SC_XMLProcStats( const ST_SPointer<SC_ProcStats> &aStats )
    : SA_XMLDocument(), mStats(aStats), createdFromNew(true)
{
    TRY(SC_XMLProcStats::SC_XMLProcStats( const ST_SPointer<SC_ProcStats> &aStats ));
    CATCH;
}

///////////////////////////////////////////
SC_XMLProcStats::SC_XMLProcStats( const std::string &aXML )
    : SA_XMLDocument(), mStats(0), createdFromNew(false), mXML(aXML)
{
    TRY(SC_XMLProcStats::SC_XMLProcStats( const std::string &aXML ));
    CATCH;
}


///////////////////////////////////////////
SC_XMLProcStats::~SC_XMLProcStats()
{
    TRY(SC_XMLProcStats::~SC_XMLProcStats());
    CATCH;
}

///////////////////////////////////////////
void SC_XMLProcStats::load()
{
    TRY(SC_XMLProcStats::load());
        if(createdFromNew)
        {
            createTreeFromNewDocument(SS_Ini::instance()->getString("xml", "daemon.proc_stats.docname"),
                    SS_Ini::instance()->getString("xml", "daemon.proc_stats.system"), 
                    SS_Ini::instance()->getString("xml", "daemon.proc_stats.public") );
            if(! mStats.empty())
            {
                XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("daemon_proc_stats");
                loadHostData(elem);
                addChild(elem);
            }
        }
        else
        {
            createTreeFromString(mXML);
            buildStatisticsObjects();
        }
    CATCH;
}

///////////////////////////////////////////
void SC_XMLProcStats::loadHostData( XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    TRY(SC_XMLProcStats::loadHostData( XERCES_CPP_NAMESPACE::DOMElement *aElem ));
        // set up vars
        XMLCh *hostTag = XMLString::transcode("hostname");
        XMLCh *hostVal = XMLString::transcode(mStats->getHostname().c_str());
        // apply new element
        aElem->setAttribute(hostTag, hostVal);
        // cleanup
        XMLString::release(&hostTag);
        XMLString::release(&hostVal);
        
        // now load up the process stats
        loadStatsData(aElem);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLProcStats::loadStatsData( XERCES_CPP_NAMESPACE::DOMElement *aElem )
{
    using rpms::convert;
    TRY(SC_XMLProcStats::loadStatsData( XERCES_CPP_NAMESPACE::DOMElement *aElem ));
        XMLCh *compTag      = XMLString::transcode("compname");
        XMLCh *instTag      = XMLString::transcode("sysinstance");
        XMLCh *cpuTag       = XMLString::transcode("cpuavg");
        XMLCh *rssTag       = XMLString::transcode("maxrss");

        std::vector<ST_SPointer<SC_CompStat> > vec = mStats->getStats();
        int sz = vec.size();
        for( int i = 0; i < sz; ++i )
        {
            XERCES_CPP_NAMESPACE::DOMElement * elem = createElementForDoc("proc_stat");
            XMLCh *compVal      = XMLString::transcode( vec.at(i)->getCompName().c_str() );
            XMLCh *instVal      = XMLString::transcode( convert<std::string>(vec.at(i)->getCompInstance()).c_str() );
            XMLCh *cpuVal       = XMLString::transcode( convert<std::string>(vec.at(i)->getCpuAvg()).c_str() );
            XMLCh *rssVal       = XMLString::transcode( convert<std::string>(vec.at(i)->getMaxRss()).c_str() );

            // add the new attributes to the element
            elem->setAttribute(compTag,     compVal );
            elem->setAttribute(instTag,     instVal );
            elem->setAttribute(cpuTag,      cpuVal );
            elem->setAttribute(rssTag,      rssVal );

            // add the new element
            addChild( elem, aElem );

            // clean up
            XMLString::release(&compVal);
            XMLString::release(&instVal);
            XMLString::release(&cpuVal);
            XMLString::release(&rssVal);
        }

        // clean up
        XMLString::release(&compTag);
        XMLString::release(&instTag);
        XMLString::release(&cpuTag);
        XMLString::release(&rssTag);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLProcStats::debug() const
{
    TRY(SC_XMLProcStats::debug());
        RPMS_DEBUG(XML_PST_LOG, "SC_XMLProcStats debug:\n" + SS_XMLUtil::instance()->serializeXML( getDoc() ) );
    CATCH;
}

///////////////////////////////////////////
std::string SC_XMLProcStats::getSerializedDoc()
{
    TRY(SC_XMLProcStats::getSerializedDoc());
        return SS_XMLUtil::instance()->serializeXML( getDoc() );
    CATCH;
}

///////////////////////////////////////////
void SC_XMLProcStats::addProcData( XERCES_CPP_NAMESPACE::DOMNode * aNode )
{
    TRY(SC_XMLProcStats::addProcData( XERCES_CPP_NAMESPACE::DOMNode * aNode ));
        addChild(aNode);
    CATCH;
}

///////////////////////////////////////////
XERCES_CPP_NAMESPACE::DOMNode *SC_XMLProcStats::getProcStatsNode()
{
    TRY(SC_XMLProcStats::getProcStatsNode());
        XMLCh * statName = XMLString::transcode( 
                SS_Ini::instance()->getString("xml_elemnames", "procstatsnodename").c_str() );
        XERCES_CPP_NAMESPACE::DOMNode * ret = getDoc()->getDocumentElement()->getElementsByTagName( statName )->item(0);
        XMLString::release(&statName);
        return ret;
    CATCH;
}

///////////////////////////////////////////
std::vector<ST_SPointer<SC_CompStat> > SC_XMLProcStats::getStatistics()
{
    // this methid will break the concept of OO in that it 
    //      breaks encapsualtion but hey, what the hell!
    TRY(SC_XMLProcStats::getStatistics());
        return mStats->getStats();
    CATCH;
}

///////////////////////////////////////////
void SC_XMLProcStats::buildStatisticsObjects()
{
    TRY(SC_XMLProcStats::buildStatisticsObjects());
        mStats = new SC_ProcStats("unknown");

        // get the daemon instance stats
        XMLCh * statsName = XMLString::transcode( 
                SS_Ini::instance()->getString("xml_elemnames", "procstatsnodename").c_str() );
        // iterate through the daemon proc stats nodes and then delegate the extraction
        XERCES_CPP_NAMESPACE::DOMNodeList *childNodes = getDoc()->getDocumentElement()->getElementsByTagName( statsName );
        XERCES_CPP_NAMESPACE::DOMElement * statsNode;
        for( int i = 0; i < childNodes->getLength(); ++i )
        {
            statsNode = dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement*>( childNodes->item(i) );
            if( statsNode )
            {
                mStats->extractStatisticsFromNode( statsNode );
            }
        }

        //clean up
        XMLString::release( &statsName );
    CATCH;
}

// #######################################################################
///////////////////////////////////////////
SC_ProcStats::SC_ProcStats(const std::string &aHostname)
    :mHostname(aHostname)
{
    TRY(SC_ProcStats::SC_ProcStats(const std::string &aHostname));
        mStats = new std::vector<ST_SPointer<SC_CompStat> >;
    CATCH;
}

///////////////////////////////////////////
SC_ProcStats::~SC_ProcStats()
{
    TRY(SC_ProcStats::~SC_ProcStats());
        delete mStats;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStats::addProcStats( const ST_SPointer<SC_CompStat> &aCompStat )
{
    TRY(SC_ProcStats::addProcStats( const ST_SPointer<SC_CompStat> &aCompStat ));
        mStats->push_back(aCompStat);
    CATCH;
}

///////////////////////////////////////////
std::vector<ST_SPointer<SC_CompStat> > SC_ProcStats::getStats() const
{
    TRY(SC_ProcStats::getStats() const);
        return std::vector<ST_SPointer<SC_CompStat> >(*mStats);
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStats::extractStatisticsFromNode( const XERCES_CPP_NAMESPACE::DOMElement * aElem )
{
    TRY(SC_ProcStats::extractStatisticsFromNode( const XERCES_CPP_NAMESPACE::DOMElement * aElem ));
        XERCES_CPP_NAMESPACE::DOMNodeList *childNodes =aElem->getChildNodes();
        XERCES_CPP_NAMESPACE::DOMElement * statNode;
        if( childNodes->getLength() < 1 )
        {
            RPMS_DEBUG(XML_PST_LOG, "No statistics for node");
        }
        
        for( int i =0; i < childNodes->getLength(); ++i )
        {
            statNode = dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement*>( childNodes->item(i) );
            if( statNode )
            {
                addProcStats( SC_CompStat::buildFromElement( statNode ) );
            }
        }
    CATCH;
}
    
// #######################################################################
///////////////////////////////////////////
SC_CompStat::SC_CompStat( const ST_SPointer<SC_ProcessConfig> &aProcCfg, const double aCpuAvg, 
        const long aMaxRss )
    : mCompName(aProcCfg->getCompName()), mCompInstance(aProcCfg->getSysInstance()), mCpuAvg(aCpuAvg), 
            mMaxRss(aMaxRss)
{
    TRY( SC_CompStat::SC_CompStat(const ST_SPointer<SC_ProcessConfig> &aProcCfg, const double aCpuAvg, const long aMaxRss) );
    CATCH;
}

///////////////////////////////////////////
SC_CompStat::SC_CompStat( const std::string &aCompName, const int aCompInst, const double aCpuAvg, const long aMaxRss )
    : mCompName(aCompName), mCompInstance(aCompInst), mCpuAvg(aCpuAvg), mMaxRss(aMaxRss)
{
    TRY(SC_CompStat::SC_CompStat( const std::string &aCompName, const int aCompInst, const double aCpuAvg, const long aMaxRss ););
    CATCH;
}

///////////////////////////////////////////
SC_CompStat::~SC_CompStat()
{
    TRY(SC_CompStat::~SC_CompStat());
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_CompStat> SC_CompStat::buildFromElement( const XERCES_CPP_NAMESPACE::DOMElement * aElem )
{
    using rpms::convert;
    TRY(SC_CompStat::buildFromElement( const XERCES_CPP_NAMESPACE::DOMElement * aElem ));
        XERCES_CPP_NAMESPACE::DOMNamedNodeMap * attr = aElem->getAttributes();
        return new SC_CompStat
            (
                SA_XMLDocument::getAttribute(attr, "compname"),
                convert<int>    (SA_XMLDocument::getAttribute(attr, "sysinstance")),
                convert<double> (SA_XMLDocument::getAttribute(attr, "cpuavg")),
                convert<long>   (SA_XMLDocument::getAttribute(attr, "maxrss"))
            );
    CATCH;
}

///////////////////////////////////////////
