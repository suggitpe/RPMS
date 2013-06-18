#ifndef __sa_election_hpp
#define __sa_election_hpp

#include <base/sa_baseobject.hpp>
#include <base/st_spointer.hpp>
#include <idllib/idls/sr_daemontask.h>

#include <map>

namespace rpms
{

    // pre decl
    class SC_ElectionCommThread;
    class SC_NodeConfig;
    
    /**
     * This is the base class for all the election classes
     *
     * @author Peter Suggitt (2005)
     * @note <b>Abstract class</b>
     * */
    class SA_Election : public SA_BaseObject
    {
        /** so we can create safely */
        friend class SS_ElectionFactory;
        public:
            /** gets the remote director after it has found it */
            CORBA::Object_var getRemoteObject(const std::string &aRemoteName, const std::string &aPort);
            /** virtual dtor */
            virtual ~SA_Election();
            /** pure virtual method to work out if the current node should be the leader */
            virtual bool shouldILead() = 0;
            /** This will allow a node to become leader */
            virtual bool allowMeToLead() = 0;
            /** This will actievly find the leader host */
            virtual std::string findLeaderHost() = 0;
            /** This will suggest a new leader (system may overrule) */
            virtual bool suggestLeader( const std::string &aHost) = 0;
            /** This will process an election message calling down the tree to the correct implemented functionality */
            virtual void processElectionMsg( const std::string &aMsg ) = 0;

        public:// accessors
            /** accessor to the loacl host */
            std::string getLocalHost() const { return mLocalHost;}
            /** accesssor to the election port */
            int getElectionPort() const { return mElectionPort;}
            /** setter for the leader flag */
            void setLeader() {mLeader = true;}
            /** setter for the leader flag */
            void setSlave()  {mLeader = false;}
            /** returns the local host rank */
            int getMyRank() const {return mRank;}
            /** clears the leader hsost so that we can re-elect (called when the leader host is old news) */
            void clearLeaderHost() {mLeaderHost = std::string();}
            /** Pure virtual method to get the a prefix for all probe messages */
            virtual std::string getMessagePrefix() = 0;
            /** Gets a list of all the nodes that can possibly lead */
            std::map<int, std::string> getElectionShortList();
    
        protected:
            /** initialise the active election parts */
            void initialiseElectable();
            /** initialise the no election parts */
            void initialise();
            /** create through an xml file (called by factory) */
            SA_Election(const std::string &aXmlConfigFile);
            /** sets the leader host */
            void setLeaderHost( const std::string &aHost ) {mLeaderHost = aHost;}
            /** returns the leader host */
            std::string getLeaderHost() const { return mLeaderHost;}
    
        protected: // members
            /** the map of node configurations */
            std::map<int, ST_SPointer<SC_NodeConfig> > mCandidates;
            /** active election thread */
            ST_SPointer<SC_ElectionCommThread> mCommThread;
            /** remote object to remote daemon (local host) */
            SR_DaemonTask_var mDaemon;
    
        private: // members
            /** path to the config file */
            std::string mXmlConfigFile;
            /** local rank */
            int mRank;
            /** leader flag */
            bool mLeader;
            /** name of the leader host */
            std::string mLeaderHost;
            /** name of the local host */
            std::string mLocalHost;
            /** port number for elections */
            int mElectionPort;
    };

} // namespace

#endif
