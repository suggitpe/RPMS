#ifndef __sc_xmlprocstatuses_hpp
#define __sc_xmlprocstatuses_hpp

#include <base/st_spointer.hpp>
#include <xmlutil/sa_xmldocument.hpp>

#include <vector>


namespace rpms
{

    // pre decl
    class SC_ProcessConfig;
    class SC_ProcStats;
    class SC_CompStat;

    ///////////////////////////////////////////////////////////////
    /**
     * This class is the main process statistics wrapper class
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_XMLProcStats : public SA_XMLDocument
    {
        public:
            /** def ctor */
            SC_XMLProcStats();
            /** Main constructor */
            SC_XMLProcStats( const ST_SPointer<SC_ProcStats> &aStatus );
            /** String constructor for when a recipient gets some xml */
            SC_XMLProcStats( const std::string &aXML );
            /** Virtual destructor */
            virtual ~SC_XMLProcStats();
            virtual void load();
            virtual void debug() const;
            std::string getSerializedDoc();
            void addProcData( XERCES_CPP_NAMESPACE::DOMNode *aNode );
            XERCES_CPP_NAMESPACE::DOMNode *getProcStatsNode();
            std::vector<ST_SPointer<SC_CompStat> > getStatistics();
            
        protected:
            void loadHostData( XERCES_CPP_NAMESPACE::DOMElement *aElem );
            void loadStatsData( XERCES_CPP_NAMESPACE::DOMElement *aElem );

        private:
            void buildStatisticsObjects();

            
        private: //members
            ST_SPointer<SC_ProcStats> mStats;
            bool createdFromNew;
            std::string mXML;
    };


    ///////////////////////////////////////////////////////////////
    /**
     * This class is a data container for the process statistics
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_ProcStats : public SA_BaseObject
    {
        public:
            /** Main constructor */
            SC_ProcStats( const std::string &aHostname );
            /** Virtual destructor */
            virtual ~SC_ProcStats();
            void addProcStats( const ST_SPointer<SC_CompStat> &aCompStat );
            std::vector<ST_SPointer<SC_CompStat> > getStats() const;
            const std::string   getHostname() const     {return mHostname;}
            void extractStatisticsFromNode( const XERCES_CPP_NAMESPACE::DOMElement * aElem );
        protected:
        private: //members
            SC_ProcStats( const SC_ProcStats &aRhs );
            SC_ProcStats& operator=( const SC_ProcStats &aRhs );
            std::string mHostname;
            std::vector<ST_SPointer<SC_CompStat> > * mStats;
    };

    ///////////////////////////////////////////////////////////////
    /**
     * This class is a simple data container class for process stats at the individual process level
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_CompStat : public SA_BaseObject
    {
        public:
            /** Main constructor */
            SC_CompStat( const std::string &aCompName, const int aCompInst, const double aCpuAvg, const long aMaxRss );
            /** Dtaa gathering constructor */
            SC_CompStat( const ST_SPointer<SC_ProcessConfig> &aProcCfg, const double aCpuAvg, const long aMaxRss );
            /** Virtual destructor */
            virtual ~SC_CompStat();
            /** Builder method to construct one of itself from a dom node */
            static ST_SPointer<SC_CompStat> buildFromElement( const XERCES_CPP_NAMESPACE::DOMElement * aElem );

        public: // accessors
            std::string getCompName()       const   {return mCompName;}
            int         getCompInstance()   const   {return mCompInstance;}
            double      getCpuAvg()         const   {return mCpuAvg;}
            long        getMaxRss()         const   {return mMaxRss;}
            void        setCpuAvg(const double aCpuAvg)  { mCpuAvg = aCpuAvg; }
            void        setMaxRss(const long aMaxRss)    { mMaxRss = aMaxRss; }
            
        protected:
        private: //members
            std::string mCompName;
            int         mCompInstance;
            double      mCpuAvg;
            long        mMaxRss;
    };

    
}// namespace


#endif
