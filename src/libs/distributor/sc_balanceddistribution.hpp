#ifndef __sc_balanceddistribution_hpp
#define __sc_balanceddistribution_hpp

#include <base/st_spointer.hpp>
#include <distributor/sa_distribution.hpp>

namespace rpms
{
    // pre decl
    class SC_ProcStatsThread;
    class SC_CompStat;

    /**
     * This class is a concrete class for the balanced distribution functionality
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_BalancedDistribution : public SA_Distribution
    {
        public:
            /** Main constructor */
            SC_BalancedDistribution( const std::string &aXmlFile );
            /** Virtual destructor */
            virtual ~SC_BalancedDistribution();
            /** Distribution nechanism */
            virtual std::map<std::string, std::string> distributeComponents( 
                    const std::vector<std::string> &aNodeList, 
                    const bool aRedistribution, 
                    const bool aForced );
            /** This is used to stop the monitor thread */
            virtual void stop();
            
        protected:
        private:
            /** Statistics distribution based on memory stats */
            void distributeForMemory(const std::vector<std::string> &aComps, 
                    const std::vector<std::string> &aNodeList, 
                    const std::map<std::string, ST_SPointer<SC_CompStat> > &aStats);
            /** Statistics distribution based on cpu average stats */
            void distributeForCpu(const std::vector<std::string> &aComps, 
                    const std::vector<std::string> &aNodeList, 
                    const std::map<std::string, ST_SPointer<SC_CompStat> > &aStats);
            
        private: //members#
            /** Defined the two distribution criteria */
            enum ECriteria {_UNKNOWN, _CPU, _MEM};
            /** Statistics gathering thread */
            ST_SPointer<SC_ProcStatsThread> mStatsThread;
            /** distribution criteria */
            ECriteria mCriteria;
    };
    
} // namespace 

#endif
