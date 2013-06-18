#ifndef __sa_activeelection_hpp
#define __sa_activeelection_hpp

#include <base/sa_threadbase.hpp>
#include <election/sa_election.hpp>

namespace rpms
{

    /** 
     * Base class for active elections (elections that actually communicate) 
     *
     * @author Peter Suggitt (2005)
     * @note <b>Abstract class</b>
     * */
    class SA_ActiveElection : public SA_Election
    {
        public:
            /** virtual dtor */
            virtual ~SA_ActiveElection();
            /** Finds leader host in the system */
            virtual std::string findLeaderHost();
            /** suggests a new leader (this may be overruled by the system) */
            virtual bool suggestLeader( const std::string &aHost);
        protected:
            /** Protected constructor .. abstract class */
            SA_ActiveElection( const std::string &aXmlConfigFile );
        private:
    };

} // namespace

#endif
