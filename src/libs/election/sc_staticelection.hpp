#ifndef __sc_staticelection_hpp
#define __sc_staticelection_hpp

#include <election/sa_election.hpp>

namespace rpms
{

    /** This is the most simple of the elections - only the highest configured rank will win */
    class SC_StaticElection : public SA_Election
    {
        friend class SS_ElectionFactory;
        public:
            virtual ~SC_StaticElection();
            virtual bool shouldILead();
            virtual bool allowMeToLead();
            virtual std::string findLeaderHost();
            virtual bool suggestLeader( const std::string &aHost);
            virtual void processElectionMsg( const std::string &aMsg );
        protected:
            SC_StaticElection(const std::string &aXmlConfigFile );
            virtual std::string getMessagePrefix() {return "STA";}
        private:
            
    };

} // namespace

#endif
