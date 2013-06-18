#ifndef __sc_xmlnodeconfig_hpp
#define __sc_xmlnodeconfig_hpp

#include <base/st_spointer.hpp>
#include <xmlutil/sa_xmldocument.hpp>

#include <map>
#include <deque>

namespace rpms
{

    // pre decl
    class SC_NodeConfig;
    
    /**
     * Class to represent the configuration of the system nodes.
     * @author  Peter Suggitt (2005) 
     * @note    Concrete Class
     * */
    class SC_XMLNodeConfig : public SA_XMLDocument
    {
        public:
            /** Standard constructor.
             * @param aPath (xml file) */
            SC_XMLNodeConfig( const std::string &aPath );
            /** Dtor */
            virtual ~SC_XMLNodeConfig();
            /** This method will initialise everything and also 
             * load up the xml and create the internal node list */
            void load();
            /** Used to give some level of debug for the Node configuration.  Details the 
             * raw XML and also the resulting objects that have been built from the XML */
            void debug() const;
            /**  */
            std::map<int, ST_SPointer<SC_NodeConfig> > getElections();
            /**  */
            int getRank( const std::string &aHostName );
            /** get node hostnames */
            std::deque<std::string> getNodeHosts();
    
        private:
            SC_XMLNodeConfig( const SC_XMLNodeConfig &aRhs );
            SC_XMLNodeConfig& operator=( const SC_XMLNodeConfig &aRhs );
            /** typedefs to make life much easier .. defines a nodeMap of string and SC_NodeConfig */
            typedef std::map< std::string, ST_SPointer<SC_NodeConfig> > nodeMap;
            /** typedefs are so much easier .. defines a pair for nodeMap (easier to insert) */
            typedef std::pair< std::string, ST_SPointer<SC_NodeConfig> > nodePair;
            /** add a new hostname to the node map 
             * @param aHostName key
             * @param aNodeMap data*/
            void add( const std::string& aHostName, const ST_SPointer<SC_NodeConfig> &aNodeMap );
            /** map of the hosts */
            nodeMap *mNodeMap;
            /** Document path to the xml file */
            std::string mDocPath;
    };
    
    /** This class is really just a data container to be used by SC_XMLNodeConfig to represent the configuration for each node */
    class SC_NodeConfig : public SA_BaseObject
    {
        public:
            /** The only ctor allowed 
             * @param aHost host name that the node resides on
             * @param aRank rank for the node
             * @param aType type of host .. O/S (is converted to enum internally) 
             * @param aUsable allows you to disable the node */
            SC_NodeConfig( const std::string &aHost, const int aRank, const std::string &aType, const bool aUsable );
            /** standard dtor */
            virtual ~SC_NodeConfig();
            /** This enum is purely there so we can control the types that are supported */
            enum EType { _UNKNOWN, _LINUX, _WIN32, _UNIX };
            /** Simple debug method */
            void debug() const;
            /** Accessor method for mHostName 
             * @return string mHostName */
            std::string getHostName() const { return mHostName;}
            /** Accessor method for mRank
             * @return int mRank */
            int getRank() const         { return mRank; }
            /** Accessor method for mType 
             * @return enum EType mType */
            EType getType() const       { return mType; }
            /** accessor to mUsable */
            bool isUsable() const { return mUsable; }
            /** This is a builder method that will allow it to build an object of type itself */
            static ST_SPointer<SC_NodeConfig> build( const XERCES_CPP_NAMESPACE::DOMElement * aElem );
    
        private:
            /** string for the name of the host */
            std::string mHostName;
            /** int rank of the host */
            int         mRank;
            /** enum value for the O/S */
            EType       mType;
            /*  bool to say if the node should be used */
            bool        mUsable;
    };

}

#endif
