#ifndef __sc_bullyelection_hpp
#define __sc_bullyelection_hpp

#include <election/sa_activeelection.hpp>

namespace rpms
{

    /** 
     * Implementation of the Bully election algorithm
     *
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_BullyElection : public SA_ActiveElection
    {
        friend class SS_ElectionFactory;
        public:
            /** dtor */
            virtual ~SC_BullyElection();
            /** This method will simply try to connect to all nodes with 
             * a ranking higher than itself.  If it can make a connection 
             * with any of them, then it will return false.  If it cannot 
             * connect to any of the higher nodes then it will return 
             * true ... ie lead */
            virtual bool shouldILead();
            /** This method will attempt to connect to all nodes with a rank 
             * less than itself and send a message to them to get them to stop 
             * their director process */
            virtual bool allowMeToLead();
            /** process bully based election messages */
            virtual void processElectionMsg( const std::string &aMsg );
    
        protected:
            /** Hidden ctor so that we can be sure only one exists ... ever */
            SC_BullyElection(const std::string &aXmlConfigFile );
            /** Defined here so that will implicitly inline */
            virtual std::string getMessagePrefix() {return "BLY";}

        private:
            /** This will look in the node config for all nodes with a higher
             * rank than this node.  It will then test that they are all active
             * and will report back the hostname of the one with the highest rank */
            std::string findHighestActiveNodeAboveMe();
            /** This will look in the node config for all nodes with a lower rank
             * than this node.  For each that it finds it will try and connect to 
             * them executing a CORBA call to stop their director service */
            void tellLesserNodesToStopDirector();
            /** gets the leader from an election message */
            std::string getLeaderFromMsg( const std::string &aMsg );
    
        private:
            /** 
             * Thread class to determine if a node has  adaemon running on it.
             * This is a private helper class so that communications to the 
             * lesser nodes can take place concurrently
             *
             * @author Peter Suggitt (2005)
             * @note <b>Thread class</b>
             * */
            class SC_BullyElecDaemonCheckThread : public SA_ThreadBase
            {
                public:
                    /** Access to craete a new instance */
                    static ST_SPointer<SC_BullyElecDaemonCheckThread> create( const std::string &aDestHost);
                    /** stop running */
                    virtual void stop();
                    /** dtor */
                    virtual ~SC_BullyElecDaemonCheckThread();
                    /** reset the thread */
                    virtual void reset();
                    /** accessor to the results */
                    bool isDestAlive() const {return mDestAlive;}
                    /** accessor to the host name */
                    std::string getDest() const {return mDestHost;}
                protected:
                    /** ctor */
                    SC_BullyElecDaemonCheckThread( const std::string &aThreadName, 
                            const std::string &aDestHost );
                    /** main run method */
                    virtual void run();
                private:
                    /** flag for destination */
                    bool mDestAlive;
                    /** name of remote host */
                    std::string mDestHost;
            };

        private:
            /** 
             * Thread class to kill a director running on a particular node
             * This is a private helper class so that communications to the 
             * lesser nodes can take place concurrently
             *
             * @author Peter Suggitt (2005)
             * @note <b>Thread class</b>
             * */
            class SC_ActiveElectionDirectorKillerThread : public SA_ThreadBase
            {
                public:
                    /** Access to craete a new instance */
                    static ST_SPointer<SC_ActiveElectionDirectorKillerThread> create( const std::string &aDestHost, const std::string &mLocalHost );
                    /** stop thread execution */
                    virtual void stop();
                    /** dtor */
                    virtual ~SC_ActiveElectionDirectorKillerThread();
                    /** reset the thread */
                    virtual void reset();
                    /** accessor to flag to determine if the end host is alive */
                    bool isDestAlive() const {return mDestAlive;}
                    /** accessor to flag to detrmine if anything was achieved from this thread execution */
                    bool didYouKill() const {return mKilled;}
                    /** accessor to the remote host name */
                    std::string getDestHost() const {return mDestHost;}
                protected:
                    /**  ctor */
                    SC_ActiveElectionDirectorKillerThread( const std::string &aThreadName, 
                            const std::string &aDestHost, const std::string &mLocalHost );
                    /** main run methid */
                    virtual void run();
                private:
                    /** remote hsot name */
                    std::string mDestHost;
                    /** local host name */
                    std::string mLocalHost;
                    /** flag to denote if the destination is alive */
                    bool mDestAlive;
                    /** flag to denote if any action was taken */
                    bool mKilled;
            };
    
            
    };

} // namespace

#endif
