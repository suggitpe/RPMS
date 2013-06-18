#ifndef __sc_xml_status_hpp
#define __sc_xml_status_hpp

#include <base/st_spointer.hpp>
#include <xmlutil/sa_xmldocument.hpp>

#include <map>
#include <vector>

namespace rpms
{

    //pre decl
    class SC_DaemonStatus;
    class SC_ComponentStatus;
    class SC_ProcessConfig;
    
    ///////////////////////////////////////////////////////////////
    /**
     * This class represents the Status of a daemon or many daemons. 
     *      this is purely a wrapper class for the XML
     *
     * @author  Peter Suggitt (2005) 
     * @note    <b>Concrete Class</b>
     * */
    class SC_XMLStatus : public SA_XMLDocument
    {
        public:
            SC_XMLStatus();
            SC_XMLStatus( const ST_SPointer<SC_DaemonStatus> &aStatus );
            SC_XMLStatus( const std::string aXMLStatus );
            virtual ~SC_XMLStatus();
            virtual void load();
            virtual void debug() const;
            std::string getSerializedDoc();
            void addDaemonData( XERCES_CPP_NAMESPACE::DOMNode *aNode );
            XERCES_CPP_NAMESPACE::DOMNode *getStatusNode();
            void addSystemData();
        protected:
        private:
            void loadDaemonData( XERCES_CPP_NAMESPACE::DOMElement *aElem );
            void loadSystemData( XERCES_CPP_NAMESPACE::DOMElement *aElem );
            void loadComponentData( XERCES_CPP_NAMESPACE::DOMElement *aElem );
            ST_SPointer<SC_DaemonStatus> mStatus;
            bool createdFromNew;
            std::string mXML;
    };
    
    ///////////////////////////////////////////////////////////////
    /** 
     * This class is a data container to represent the status of the daemon itself 
     *
     * @author  Peter Suggitt (2005) 
     * @note    <b>Concrete Class</b>
     * */
    class SC_DaemonStatus : public SA_BaseObject
    {
        public:
            SC_DaemonStatus(const std::string &aHostName, const bool aRegistered, 
                    const bool aRunning, const bool aDirectorHost);
            virtual ~SC_DaemonStatus();

            void addComponentStatus( const ST_SPointer<SC_ComponentStatus> &aCompStat );
            std::vector<ST_SPointer<SC_ComponentStatus> > getComponents() const 
                        { return std::vector<ST_SPointer<SC_ComponentStatus> >(*mComponents); }
            const std::string   getHostName() const     {return mHostName;}
            const bool          isRegistered() const    {return mRegistered;}
            const bool          isRunning() const       {return mRunning;}
            const bool          isDirHost() const       {return mDirectorHost;}
            
        protected:
        private:
            SC_DaemonStatus( const SC_DaemonStatus &aRhs );
            SC_DaemonStatus& operator=( const SC_DaemonStatus &aRhs );
            std::string mHostName;
            bool mRegistered;
            bool mRunning;
            bool mDirectorHost;
            std::vector<ST_SPointer<SC_ComponentStatus> > * mComponents;
    };
    
    ///////////////////////////////////////////////////////////////
    /** 
     * This class is a data container to represent the status of each process 
     *      type (component) running 
     *
     * @author  Peter Suggitt (2005) 
     * @note    <b>Concrete Class</b>
     * */
    class SC_ComponentStatus : public SA_BaseObject
    {
        public:
            /** Main constructor */
            SC_ComponentStatus( const ST_SPointer<SC_ProcessConfig> &aProcCfg, const bool aRunning, 
                    const std::string &aStartTime, const int aRetries, const int aPid, const int aRss,
                    const double aRssAvg, const long aCpuTime, const double aCpuAvg );
            /** Virtual destructor */
            virtual ~SC_ComponentStatus();

        public: // accessors
            std::string     getCompName()   const { return mCompName; }
            int             getInstance()   const { return mInstance; }
            bool            isRunnable()    const { return mRunnable; }
            bool            isRunning()     const { return mRunning; }
            std::string     getStartTime()  const { return mStartTime; }
            std::string     getLogName()    const { return mLogName; }
            std::string     getRunType()    const { return mRunType; }
            bool            isMoveable()    const { return mMoveable; }
            int             getMaxRetries() const { return mRetryMax; }
            int             getPid()        const { return mPid; }
            int             getRss()        const { return mRss; }
            double          getRssAvg()     const { return mRssAvg; }
            int             getCpuTime()    const { return mCpuTime; }
            double          getCpuAvg()     const { return mCpuAvg; }
        protected:
        private:
            std::string     mCompName;
            int             mInstance;
            bool            mRunning;
            std::string     mStartTime;
            bool            mRunnable;
            int             mRetryMax;
            bool            mMoveable;
            std::string     mRunType;
            std::string     mLogName;
            int             mPid;
            int             mRss;
            double          mRssAvg;
            long            mCpuTime;
            double          mCpuAvg;
    };

    ///////////////////////////////////////////////////////////////
    /** 
     * This class is a data container to represent the status of the system as a whole 
     *
     * @author  Peter Suggitt (2005) 
     * @note    <b>Concrete Class</b>
     * */
    class SC_SystemStatus : public SA_BaseObject
    {
        public:
            /** Main constructor */
            SC_SystemStatus();
            /** Virtual destructor */
            virtual ~SC_SystemStatus();
            std::string getSystemName() const {return mSystemName;}
            std::string getInstanceId() const {return mInstanceId;}
            std::string getNowDate() const {return mNowDate;}
            /** */
            void load();
        protected:
        private:
            std::string mSystemName;
            std::string mInstanceId;
            std::string mNowDate;
    };
    
} // namespace

#endif
