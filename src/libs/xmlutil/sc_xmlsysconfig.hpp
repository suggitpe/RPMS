#ifndef __sc_xmlsysconfig_hpp
#define __sc_xmlsysconfig_hpp

#include <base/st_spointer.hpp>
#include <xmlutil/sa_xmldocument.hpp>

#include <map>
#include <vector>
#include <deque>

namespace rpms
{

    // pre decl
    class SC_ProcessConfig;
    
    ///////////////////////////////////////////////////////////////
    /**
     * This is a system configuration class to extract the data from the xml
     *
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_XMLSysConfig : public SA_XMLDocument
    {
        public:
            /** Standard ctor.
             *      @param aPath (xml file) */
            SC_XMLSysConfig( const std::string &aPath );
            /** Dtor */
            virtual ~SC_XMLSysConfig();
            /** This method will initialise everything and also 
             * load up the xml and create the internal component list */
            void load();
            /** Used to give some level of debug for the component configuration.  Details the 
             * raw XML and also the resulting objects that have been built from the XML */
            void debug() const;
            /** Return a list of the components to run */
            std::vector<std::string> getCompList();
            /** Returns the process object for a given process name 
             *      @param aProcessName anme of the process to get */
            SC_ProcessConfig getProcessConfig( const std::string &aProcessName );
    
        private:
            /** hidden copy ctor */
            SC_XMLSysConfig( const SC_XMLSysConfig &aRhs );
            /** hidden ass op */
            SC_XMLSysConfig& operator=( const SC_XMLSysConfig &aRhs );
            /** typedef to make life easier .. defMap for the defaults map (string, string) */
            typedef std::map<std::string, std::string> defMap;
            /** typedef to make life easier for adding to the defaults map */
            typedef std::pair<std::string, std::string> defPair;
            /** to get the defaults list */
            defMap loadUpCompDefaults( const XERCES_CPP_NAMESPACE::DOMElement *aElem );
            /** typedefs are so much easier  */
            typedef std::map< std::string, ST_SPointer<SC_ProcessConfig> > compMap;
            /** typedefs are so much easier */
            typedef std::pair< std::string, ST_SPointer<SC_ProcessConfig> > compPair;
            /** add a new component to the comp map 
             *      @param aCompName name of a component
             *      @param aCompMap the data itself */
            void add( const std::string& aCompName, const ST_SPointer<SC_ProcessConfig> &aCompMap );
            /** map of the hosts */
            compMap *mCompMap;
            /** Document path to the xml file */
            std::string mDocPath;
    };


    /**
     * This class represents the actual configuration that will be sent to the
     *  daemons so actually run the processes.  There will be one of these class created for
     *  every daemon in the system that is being used to run processes
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_XMLDaemonConfig  : public SA_XMLDocument
    { 
        public:
            /** Main constructor */
            SC_XMLDaemonConfig();
            /** String constructor. Used when we receive xml in memory. */
            SC_XMLDaemonConfig( const std::string &aXML );
            /** Virtual destructor */
            virtual ~SC_XMLDaemonConfig();
            /** Create the basis for the document */
            virtual void load();
            /** simple debug method */
            virtual void debug() const;
            /** add a process configuration to the daemon config 
             *      @param aProcConfig a process configuration object*/
            void addProcess( const SC_ProcessConfig &aProcConfig );
            /** Removes all processes that are moveable and rebuilds the document accrordingly */
            void removeMoveableProcesses();
            /** This method will return the serialized version of the underlying document */
            std::string getSerializedDoc();
            /** We must be able to create a single empty dummy XML document so that it can
             *      be sent to the daemons when they join if the system is not performing
             *      a reconfiguration in the extra node joining the system */
            std::string createDummyDaemonConfig();
            /** Get a copy of the internal SC_ProcessConfig objects */
            std::vector<ST_SPointer<SC_ProcessConfig> > getProcesses();
            /** Returns the number of processes in this object */
            int getNumProcs() const {return mProcs->size();}
            /** Get a list of the configured processes */
            std::vector<std::string> getProcessNames();

        protected:
        private:
            /** Load the process info to the underlying document */
            void loadTreeWithProcessData( const SC_ProcessConfig &aProcConfig, XERCES_CPP_NAMESPACE::DOMElement *aElem );
            /** build all of the process configurations from the tree */
            void buildFromTree();
            SC_XMLDaemonConfig( const SC_XMLDaemonConfig &aRhs );
            SC_XMLDaemonConfig& operator=( const SC_XMLDaemonConfig &aRhs );

        private: //members
            /** type def to make life easier! */
            typedef std::vector<ST_SPointer<SC_ProcessConfig> > procVect;
            /** Flag to work out if we are creatng a new one (director) or building from a xml string (daemon) */
            bool createFromNew;
            /** String xml passed to us */
            std::string mXML;
            /** The actual processes */
            procVect * mProcs;
    };


    ///////////////////////////////////////////////////////////////
    /** This class is really just a data container to be used by SC_XMLSysConfig to represent the configuration for each process (component) */
    class SC_ProcessConfig : public SA_BaseObject
    {
        public:
            /** The only ctor allowed
             *      @param aCompName .. name of the component
             *      @param aSysInstance .. 
             *      @param aRunnable
             *      @param aMoveable
             *      @param aRetryMax
             *      @param aRetryWindow
             *      @param aRunType
             *      @param aStdOut
             *      @param aStdErr
             *      @param aArgs
             *      @param aLoggingName
             * */
            SC_ProcessConfig( const std::string &aCompName,
                              const int aSysInstance,
                              const bool aRunnable,
                              const bool aMoveable,
                              const int aRetryMax, 
                              const long aRetryWindow, 
                              const std::string &aRunType,
                              const std::string &aStdOut,
                              const std::string &aStdErr,
                              const std::string &aArgs,
                              const std::string &aLoggingName);
            /** dtor */
            virtual ~SC_ProcessConfig();
            /** copy ctor */
            SC_ProcessConfig( const SC_ProcessConfig &aRhs );
            /** assignment operator */
            SC_ProcessConfig& operator=( const SC_ProcessConfig &aRhs );

            bool operator==( const SC_ProcessConfig &aRhs );
            bool operator!=( const SC_ProcessConfig &aRhs );

            /** This enum is purely there so we can control the types that are supported */
            enum ERunType {_UNKNOWN, _CONTINUOUS, _ONCE};
            /** Simple debug method */
            void debug() const;
            /** This is a static builder method that will create an object of type itself. This method will return 
             *      many instance of itself if there are more than once instance specified in the configuration xml 
             *      @param aElem a DOM element from which to build the object
             *      @param aDefaults process defaults in case there are process variables missing */
            static std::vector<ST_SPointer<SC_ProcessConfig> > build( const XERCES_CPP_NAMESPACE::DOMElement * aElem, 
                                                                    std::map<std::string, std::string> &aDefaults );
            /** This method does a similar thing to the other build method, but this time
             *      we do not expect a defaults map because we expect this version to have a different structure.
             *  One additional diff between the previous implementation of build is that it will build all the 
             *      processes from the given xml daemon config file */
            static std::vector<ST_SPointer<SC_ProcessConfig> > buildAllProcesses( const XERCES_CPP_NAMESPACE::DOMElement * aElem );
    
            // accessors
            /** Accessor to compname */
            std::string getCompName() const {return mCompName;}
            /** Accessor to sysinstance */
            int getSysInstance() const {return mSysInstance;}
            /** Accessor to runnable */
            bool isRunnable() const {return mRunnable;}
            /** Accessort to moveable flag */
            bool isMoveable() const {return mMoveable; }
            /** Accessor to max retries */
            int getMaxRetries() const {return mRetryMax;}
            /** Accessor to retry window */
            long getRetryWindow() const {return mRetryWindow;}
            /** Acessor to mRunType */
            ERunType getRunType() const {return mRunType;}
            /** Accessor to stdout */
            std::string getStdOut() const {return mStdOut;}
            /** Accessor to stderr flag */
            std::string getStdErr() const {return mStdErr;}        
            /** Accessor to the args */
            std::string getArgs() const { return mArgs;}
            /** Accessor to loggingName */
            std::string getLoggingName() const {return mLoggingName;}
            /** This is where we define how we describe the component */
            std::string getInternalName() const;
    
        private:
            std::string mCompName;
            int         mSysInstance;
            bool        mRunnable;
            bool        mMoveable;
            int         mRetryMax;
            long        mRetryWindow;
            ERunType    mRunType;
            std::string mStdOut;
            std::string mStdErr;
            std::string mArgs;
            std::string mLoggingName;
    };

} // namespace

#endif
