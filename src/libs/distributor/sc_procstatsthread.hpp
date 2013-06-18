#ifndef __sc_procstatsthread_hpp
#define __sc_procstatsthread_hpp

#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>
#include <idllib/idls/sr_directortask.h>

#include <map>

namespace rpms
{
    // pre decl
    class SC_CompStat;
    
     
    /**
     * This class is a process monotor class that will poll the daemons 
     *  looking for process statistics
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Thread class</b>
     * */
    class SC_ProcStatsThread : public SA_ThreadBase
    {
        public:
            /** The only way to actually create one of these */
            static ST_SPointer<SC_ProcStatsThread> create();
            /** Virtual destructor */
            virtual ~SC_ProcStatsThread();
            /** stop thread execution */
            virtual void stop();

        public: // accessors
            /** Returns whether the data has been loaded */
            bool isDataLoaded() const {return mDataLoaded;}
            /** This will get a copy of the underlying data structure */
            std::map<std::string, ST_SPointer<SC_CompStat> > getStatistics() const;
            /** has the thread been initialsied yet */
            bool isInitialised() const { return mInitialised;}

        protected:
            /** Main constructor */
            SC_ProcStatsThread( const std::string &aName );
            /** The main run method */
            virtual void run();

        private: 
            /** Initialise the thread */
            bool init();
            /**  Hidden copy ctor */
            SC_ProcStatsThread( const SC_ProcStatsThread &aRhs );
            /**  Hidden assignment operator */
            SC_ProcStatsThread& operator=( const SC_ProcStatsThread &aRhs );
            /** process the returned xml containing the process statistics */
            void processStatsXML( const std::string &aXml );
            /** Persist the internal data to file */
            bool persist() const;
            /** Load the data from file */
            void loadDataFromFile();
            /** simple one to help with any debugging in a development arena */
            void debug() const;

        private: //members
            // make my life easier
            typedef std::map<std::string, ST_SPointer<SC_CompStat> > statMap;
            // make my life easier
            typedef std::pair<std::string, ST_SPointer<SC_CompStat> > statPair;
            /** Flag to denote whether the thread should run */
            bool mShouldRun;
            /** The name of the data file to persist to and read from */
            std::string mDataFile;
            /** The working directory for file creation */
            std::string mWorkDir;
            /** The loop poll time */
            int mPollWait;
            /** The remote object for the director */
            SR_DirectorTask_var mRemote;
            /** data loaded flag */
            bool mDataLoaded;
            /** statistics data */
            statMap * mData;
            /** flag to state whether this thread has been initialised */
            bool mInitialised;
    };
    
}// namespace

#endif
