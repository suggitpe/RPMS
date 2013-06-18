#ifndef __sc_ringelection_hpp
#define __sc_ringelection_hpp

#include <election/sa_activeelection.hpp>

namespace rpms
{

    /**
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     *      This class is a 
     * */
    
    /** Implementation of the ring election algorithm */
    class SC_RingElection : public SA_ActiveElection
    {
        friend class SS_ElectionFactory;
        public:
            /** Virtual destructor */
            virtual ~SC_RingElection();
            virtual bool shouldILead();
            virtual bool allowMeToLead();
            virtual void processElectionMsg( const std::string &aMsg );
            bool isElecting() {return mElecting;}
    
        protected:
            /** Main constructor */
            SC_RingElection( const std::string &aXmlConfigFile );
            /** Defined here so that will implicitly inline */
            virtual std::string getMessagePrefix() {return "RNG";}
        private:
            std::string createSelfForElecMsg();
            bool sendMsgToNextNode( const std::string &aMsg );
            void initiateCoOrdMsg( const std::string &aMsg );
            std::string findNextRingNode( const std::string &aHost );
            void initiateRingElection();
            std::string createInitialMsg( const std::string & aPrefix );
            static std::string getOriginatingHostFromElecMsg( const std::string &aMsg );
            static std::string getElectedLeaderFromElecMsg(const std::string &aMsg );
            static std::string getElectedLeaderFromCoordMsg(const std::string &aMsg );
    
        private:
            bool mElecting;
            std::map<int, std::string> * mShortList;
            
    };

} // namespace

#endif
