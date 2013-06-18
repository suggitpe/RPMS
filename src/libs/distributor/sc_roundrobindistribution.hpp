#ifndef __sc_roundrobindistribution_hpp
#define __sc_roundrobindistribution_hpp

#include <distributor/sa_distribution.hpp>

namespace rpms
{
    /**
     * This class managed the distribution of components across the system and will only manage it 
     *      using a process balance sequence (ie ignoring what the processes are).
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_RoundRobinDistribution : public SA_Distribution
    {
        public:
            /** Main constructor */
            SC_RoundRobinDistribution( const std::string &aXmlFile );
            /** Virtual destructor */
            virtual ~SC_RoundRobinDistribution();
            /** Distribution mechanism (this does the full distribution for initial and 
             *              recongigurated distribution) */
            virtual std::map<std::string, std::string> distributeComponents( 
                    const std::vector<std::string> &aNodeList, const bool aRedistribution, const bool aForced );
            /** Placeholder */
            virtual void stop() {}

        protected:
            
        private: //members
            
    };
    
} // namespace

#endif
