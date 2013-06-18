#ifndef __ss_distributor_hpp
#define __ss_distributor_hpp

#include <base/st_singleton.hpp>
#include <base/st_spointer.hpp>

#include <vector>
#include <map>

namespace rpms
{

    //pre decl
    class SA_Distribution;

    /**
     * This class is a singleton class used for the management of compoent allocation.  
     * It is used by the director processes and will also have a local data store from 
     *      previous component executions.
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Singleton class</b>
     * */
    class SS_Distributor : public ST_Singleton<SS_Distributor>
    {
        friend class ST_Singleton<SS_Distributor>;
        public:
            /** Virtual destructor */
            virtual ~SS_Distributor();
            /** Initialise here with the correct distribution type and also load up the main components */
            void initialise();
            /** Here we do the node allocation of the actual components */
            std::map<std::string, std::string> distribute( const std::vector<std::string> &aNodeList, const bool aRedistribution, const bool aForced = false );
            /** This is an accessor to the underlying distribution to get the 
             *      getDistributionForNode method */
            std::string getDistributionForNode( const std::string &aNodeName );
            /** reset the distribution */
            void resetDistribution();
            /** generic accessor to the underlying stop mechanism */
            void stopActiveProcesses();
            
        protected:

        private:
            /** Main constructor */
            SS_Distributor();
            /** Hidden copy ctor */
            SS_Distributor( const SS_Distributor &aRhs );
            /** Hidden assignment operator */
            SS_Distributor& operator=( const SS_Distributor &aRhs );

        private: //members
            /** initialisation flag */
            bool mInitialised;
            /** Distribution method */
            ST_SPointer<SA_Distribution> mDist;
    };


} // namespace

#endif
