//#############################
#include "sc_procstatsthread.hpp"
#include "sx_distributor.hpp"
#include "si_distributorlog.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>

#include <corbaimpl/ss_corbaimpl.hpp>
#include <corbaimpl/sx_corbaimpl.hpp>
#include <logging/si_logging.hpp>
#include <sockets/sc_inetaddress.hpp>
#include <utilities/ss_ini.hpp>
#include <utilities/sc_filesystemhelper.hpp>
#include <xmlutil/sc_xmlprocstats.hpp>

#include <fstream>

using rpms::SC_ProcStatsThread;
using rpms::ST_SPointer;
using rpms::SC_CompStat;
///////////////////////////////////////////
ST_SPointer<SC_ProcStatsThread> SC_ProcStatsThread::create()
{
    TRY(SC_ProcStatsThread::create());
        return new SC_ProcStatsThread( SS_Ini::instance()->getString("threadnames", "procstats") );
    CATCH;
}

///////////////////////////////////////////
SC_ProcStatsThread::SC_ProcStatsThread( const std::string &aName )
    : SA_ThreadBase(aName), mShouldRun(true), mDataLoaded(false), mInitialised(false)
{
    TRY(SC_ProcStatsThread::SC_ProcStatsThread( const std::string &aName ));
        mData = new std::map<std::string, ST_SPointer<SC_CompStat> >;
    CATCH;
}

///////////////////////////////////////////
SC_ProcStatsThread::~SC_ProcStatsThread()
{
    TRY(SC_ProcStatsThread::~SC_ProcStatsThread());
        delete mData;
    CATCH;
}

///////////////////////////////////////////
bool SC_ProcStatsThread::init()
{
    TRY(SC_ProcStatsThread::init());
        mDataFile = SS_Ini::instance()->getString("distributor", "distributor.stats.datafile");
        mWorkDir  = SS_Ini::instance()->getString("directories", "working");
        mPollWait = SS_Ini::instance()->getInt("distributor", "distributor.stats.poll.interval");
        std::string idlName = SS_Ini::instance()->getString("director", "director.idl.remotename");
        std::string port = SS_Ini::instance()->getString("CORBA", "corba.orb.port");
        std::string host = SC_InetAddress::getLocalHostName();
        try
        {
            CORBA::Object_var obj = SS_CorbaImpl::instance()->getRemoteObject( host, port, idlName );
            mRemote = SR_DirectorTask::_narrow(obj);
        }
        catch( SX_CorbaImpl &sex )
        {
            RPMS_ERROR(DST_PST_LOG, "Failed to connect to remote object [" 
                    + idlName + "] on host [" + host + ":" + port + "]:\n" + sex.reason());
            return false;
        }

        if( CORBA::is_nil(mRemote) )
        {
            throw SX_Distributor("Failed to create remote object SR_DirectorTask");
        }

        loadDataFromFile();

        mInitialised = true;
        RPMS_DEBUG(DST_PST_LOG, "Process statistics thread now active");
        return true;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStatsThread::run()
{
    using rpms::convert;
    TRY(SC_ProcStatsThread::run());
        mShouldRun = true;
        if(!init())
        {
            return;
        }

        while(mShouldRun)
        {
            // get the stats
            RPMS_INFO(DST_PST_LOG, "Getting fresh batch of process stats");
            std::string xml = mRemote->distGetStats();
            // process the xml
            processStatsXML(xml);
            // sleep for a bit
            RPMS_INFO(DST_PST_LOG, "Sleeping for [" + convert<std::string>(mPollWait) + "] seconds");
            sleep(mPollWait);
        }
        RPMS_INFO(DST_PST_LOG, "SC_ProcStatsThread: completed work loop");
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStatsThread::processStatsXML( const std::string &aXml )
{
    TRY(SC_ProcStatsThread::processStatsXML( const std::string &aXml ));
        RPMS_TRACE(DST_PST_LOG, "Processing statistics xml into the system");
        SC_XMLProcStats stats( aXml );
        stats.load();

        std::vector<ST_SPointer<SC_CompStat> > s = stats.getStatistics();
        int sz = s.size();
        if( sz < 1 )
        {
            RPMS_TRACE(DST_PST_LOG, "No statistics to process");
            return;
        }
        
        double cpuAvg = 0;
        long maxRss = 0;
        std::string name = "";
        // process the stats
        for( int i = 0; i < sz; ++i )
        {
            if( (s.at(i)->getMaxRss() == (long)0) && (s.at(i)->getCpuAvg() == (double)0 ) )
            {
                // we want to ignore these as they have no data that is of any use to us
                continue;
            }
            name = s.at(i)->getCompName();

            // now we have the good stuff
            statMap::iterator fIter = mData->find( name );
            if( fIter == mData->end() ) // we do not have this one
            {
                RPMS_TRACE(DST_PST_LOG, " ....... creating new statistic for " + name );
                mData->insert( statPair( name, new SC_CompStat( name, 0, s.at(i)->getCpuAvg(), s.at(i)->getMaxRss() ) ) );
            }
            else
            {
                // now we need to update them but only if we have a greater value than previously seen
                cpuAvg = s.at(i)->getCpuAvg();
                maxRss = s.at(i)->getMaxRss();
                if( cpuAvg > fIter->second->getCpuAvg() )
                {
                    RPMS_TRACE(DST_PST_LOG, " ....... updating cpuAvg for " + name );
                    fIter->second->setCpuAvg( cpuAvg );
                }

                if( maxRss > fIter->second->getMaxRss() )
                {
                    RPMS_TRACE(DST_PST_LOG, " ....... updating maxRss for " + name );
                    fIter->second->setMaxRss( s.at(i)->getMaxRss() );
                }
            }
        }
        mDataLoaded = true;
        // persist it
        persist();
    CATCH;
}

///////////////////////////////////////////
bool SC_ProcStatsThread::persist() const
{
    TRY(SC_ProcStatsThread::persist() const);
        RPMS_TRACE(DST_PST_LOG, "Peristsing statistics data to [" + mWorkDir  + "/" + mDataFile + "]");
        // first make sure that the dir exists
        if( ! SC_FileSystemHelper::createDirectory(mWorkDir) )
        {
            RPMS_WARN(DST_PST_LOG, "Failed to create working directory [" + mWorkDir + "] for writing data file");
            return false;
        }

        // now we can dump the contents of the stats hol;der to the file
        std::ofstream file( (mWorkDir + "/" + mDataFile).c_str(), std::ios::out );
        if(!file)
        {
            RPMS_WARN(DST_PST_LOG, "Failed to open data file [" + mDataFile + "] for writing");
            return false;
        }

        // iterate through the map dopping out the data as needed
        statMap::iterator iter = mData->begin();
        while( iter != mData->end() )
        {
            file << ":" 
                << iter->second->getCompName()  << ":" 
                << iter->second->getCpuAvg()    << ":"
                << iter->second->getMaxRss()    << ":"
                << std::endl;
            ++iter;
        }
        
        file.close();
        return true;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStatsThread::loadDataFromFile()
{
    using rpms::convert;
    TRY(SC_ProcStatsThread::loadDataFromFile());
        RPMS_TRACE(DST_PST_LOG, "Loading statistics data from [" + mWorkDir  + "/" + mDataFile + "]");
        std::ifstream datFile( (mWorkDir + "/" + mDataFile).c_str(), std::ios::in );
        if( !datFile )
        {
            RPMS_TRACE(DST_PST_LOG, "Data file does not exist");
            return;
        }

        std::string ln;
        // these will form the indexes of the ':' chars so we can do simple substrings
        int numPos = 4; // edit here if you need to add extra crierion
        int pos[numPos];
        std::string vals[numPos-1];

        while( getline(datFile, ln) )
        {
            pos[0] = ln.find(':', 0);
            for( int i = 1; i < numPos; ++i )
            {
                // get the next ':'
                pos[i] = ln.find(':', pos[i-1]+1);
                // get the substring for that delimeter
                vals[i-1] = ln.substr(pos[i-1]+1, pos[i] - pos[i-1] - 1);
            }

            // create the internal data structures
            mData->insert( statPair( vals[0], new SC_CompStat( vals[0], 0, convert<double>(vals[1]), convert<long>(vals[2]) ) ) );
        }
        
        datFile.close();
        mDataLoaded = true;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStatsThread::stop()
{
    TRY(SC_ProcStatsThread::stop());
        mShouldRun = false;
        // this should be in a lock
        stopDead();
    CATCH;
}

///////////////////////////////////////////
void SC_ProcStatsThread::debug() const
{
    TRY(SC_ProcStatsThread::debug() const);
        statMap::iterator iter = mData->begin();
        while( iter != mData->end() )
        {
            std::cout << "NAME[" << iter->second->getCompName() 
                << "], CPU[" <<  iter->second->getCpuAvg() 
                << "], RSS[" <<  iter->second->getMaxRss() << "]" << std::endl;
            ++iter;
        }
    CATCH;
}

///////////////////////////////////////////
std::map<std::string, ST_SPointer<SC_CompStat> > SC_ProcStatsThread::getStatistics() const
{
    TRY(ST_SPointer<SC_CompStat> > SC_ProcStatsThread::getStatistics() const);
        return std::map<std::string, ST_SPointer<SC_CompStat> >(*mData);
    CATCH;
}

// ######################################################
///////////////////////////////////////////
SC_ProcStatsThread::SC_ProcStatsThread( const SC_ProcStatsThread &aRhs )
    : SA_ThreadBase("dummy")
{
    TRY(SC_ProcStatsThread::SC_ProcStatsThread( const SC_ProcStatsThread &aRhs ));
        throw SX_Distributor("No copy constructor allowed in SC_ProcStatsThread");
    CATCH;
}

///////////////////////////////////////////
SC_ProcStatsThread& SC_ProcStatsThread::operator=( const SC_ProcStatsThread &aRhs )
{
    TRY(SC_ProcStatsThread::operator=( const SC_ProcStatsThread &aRhs ));
        throw SX_Distributor("No assign operator allowed in SC_ProcStatsThread");
    CATCH;
}

///////////////////////////////////////////
