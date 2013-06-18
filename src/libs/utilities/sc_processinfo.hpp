#ifndef __sc_processinfo_hpp
#define __sc_processinfo_hpp

#include <sys/resource.h>
#include <base/sa_baseobject.hpp>

namespace rpms
{

    /**
     * This struct is a data container class for the information retrieved from a /proc/[pid]/stat file
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Data container struct</b>
     * */
    struct proc_data
    { 
        /** process name */
        std::string     mProcName;
        /** time stamp in secs (since epoch) */
        long            mTimeStamp;
        /** User time (cpu) used by a process */
        unsigned long   mUserTime;
        /** system time (cpu) used by a process */
        unsigned long   mSysTime;
        /** Memory useage RSS */
        long            mRss;
        /** This is a valid flag that lets us know if the operation was successfull */
        bool            mSuccess;
    };

    /**
     * This class is a helper class that allows a particular process to be monitored.
     *      This is basically a wrapper class for the getrusage concept.
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Utility class</b>
     * */
    class SC_ProcessInfo : public SA_BaseObject
    {
        public:
            /** Main constructor */
            SC_ProcessInfo();
            /** Pid constructor used when analysing external systems */
            SC_ProcessInfo( const int aPid );
            /** Virtual destructor */
            virtual ~SC_ProcessInfo();
            /** Load up the latest data of the pid into the rusage struct */
            void load( const bool aBaseLine = false );
            /** Debug to get statistics of all the rusage parts */
            void debug() const;
            /** Get process RSS for the local object */
            long getRss() const { return getRssForPid(mPid); }
            /** Get the RSS for a given pid */
            static long getRssForPid( const int aPid );

        public: // data accessors
            // ######### VALIDITY
            /** check for validity of the baseline stats */
            bool isBaseValid() const {return mBaselineData.mSuccess;}
            /** check for validity of the latest stats */
            bool isLatestValid() const {return mLatestData.mSuccess;}
            // ########## PROC NAME
            /** gets the procses name */
            std::string getProcName() const {return mBaselineData.mProcName; }
            // ########## RSS
            /** Get latest RSS */
            long getLatestRss() const {return mLatestData.mRss;}
            /** Get baseline RSS */
            long getBaselineRss() const {return mBaselineData.mRss;}
            /** RSS diff */
            long getRssDiff() const {return mLatestData.mRss - mBaselineData.mRss;}
            /** Gets the average growth in RSS */
            double getRssAvg() const;
            // ########## UTIME 
            /** Get latest user time */
            unsigned long getUTime() const {return mLatestData.mUserTime;}
            /** Get baseline user time */
            unsigned long getBaselineUTime() const {return mBaselineData.mUserTime;}
            /** Get utime diff */
            unsigned long getUTimeDiff() const {return mLatestData.mUserTime - mBaselineData.mUserTime;}
            // ########### STIME
            /** Get latest system time */
            unsigned long getSTime() const {return mLatestData.mSysTime;}
            /** Get baseline system time */
            unsigned long getBaselineSTime() const {return mBaselineData.mSysTime;}
            /** Get stime diff */
            unsigned long getSTimeDiff() const {return mLatestData.mSysTime - mBaselineData.mSysTime;}
            // ########## TOTAL CPU
            /** Get latest total cpu time */
            unsigned long getCpuTime() const {return mLatestData.mUserTime + mLatestData.mSysTime;}
            /** Get baseline total cpu time */
            unsigned long getBaselineCpuTime() const {return mBaselineData.mUserTime + mBaselineData.mSysTime;}
            /** Get CPU total time diff */
            unsigned long getCpuTimeDiff() const {return (mLatestData.mUserTime + mLatestData.mSysTime) - (mBaselineData.mUserTime + mBaselineData.mSysTime);}
            /** gets the average cpu usage over the time span (per second) */
            double getCpuAvgPerSec() const;
            /** gets the average cpu usage over the time span (per minute) */
            double getCpuAvgPerMin() const;
            // ####### TIMESTAMP
            /** Get latest timestamp */
            long getTimeStamp() const {return mLatestData.mTimeStamp;}
            /** Get baseline timestamp */
            long getBaselineTimeStamp() const {return mBaselineData.mTimeStamp;}
            /** Get time diff */
            long getTimeStampDiff() const { return mLatestData.mTimeStamp - mBaselineData.mTimeStamp;}

        protected:

        private:
            /** Hideden copy ctor */
            SC_ProcessInfo( const SC_ProcessInfo &aRhs );
            /** Hidden assignment operator  */
            SC_ProcessInfo& operator==( const SC_ProcessInfo &aRhs );
            /** C-style member function that allows us to get the process stats from the /proc file system */
            static int get_proc_stats( const int aPid, struct proc_data *aData );

        private: //members
            /** The pid of the process that we want to monitor (this is set to 0 if we 
             *      want to monitor ourselves) */
            int mPid;
            /** The underlying baseline data container */
            struct proc_data mBaselineData;
            /** The underlying latest data container */
            struct proc_data mLatestData;
    };
        
} // namespace

#endif
