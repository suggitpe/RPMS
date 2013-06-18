#ifndef __sa_distribution_hpp
#define __sa_distribution_hpp

#include <base/sa_baseobject.hpp>
#include <base/st_spointer.hpp>
#include <tools/sc_basicmutex.hpp>

#include <vector>
#include <map>

namespace rpms
{

    //pre decl
    class SC_XMLSysConfig;
    class SC_XMLDaemonConfig;

    /**
     * This class is a base class for all distribition classes
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Abstract class</b>
     * */
    class SA_Distribution : public SA_BaseObject
    {
        public:
            /** Virtual destructor */
            virtual ~SA_Distribution();
            /** Pure virtual method to allow for the distribution of the components */
            virtual std::map<std::string, std::string> distributeComponents( 
                    const std::vector<std::string> &aNodeList, 
                    const bool aRedistribution, 
                    const bool aForced ) = 0;
            /** Initialise and load up the components */
            void initialise();
            /** distributed accessor */
            bool isDistributed() const { return mDistributed; }
            /** Accessor method to get the distribution of components for a particular node */
            std::string getDistributionForNode( const std::string &aNodeName );
            /** This will allow us to reset the distribution mechanism */
            void reset();
            /** Pure virtual to allow us to issue a stop command to the derived classes */
            virtual void stop() = 0;
            
        protected:
            /** Main constructor */
            SA_Distribution( const std::string &aXmlFile );
            /** distributed accessor method */
            void setDistributed( const bool aBool ) { mDistributed = aBool;}
            /** Helper methid to set up the component list for a redistribution */
            void prepareForRedistribution( std::vector<std::string> &aComps, const bool aForced );
            /** set up any additional node config containers for the distribution */
            void prepareNodeConfigContainers( const std::vector<std::string> &aNodeList );
            /**  This is used for any circumstance where we need a quick and dirty 
             *      distribution (e.g. when the balanced distribution has no data to go on) */
            std::map<std::string, std::string> defaultDistributeComponents( const std::vector<std::string> &aNodeList );

        protected: //members
            /** System configuration data */
            ST_SPointer<SC_XMLSysConfig> mComponents;
            /** This is the current state of distribution */
            std::map<std::string, std::string> * mDistState;
            /** This is a collection of the underlying xml objects */
            std::map< std::string, ST_SPointer<SC_XMLDaemonConfig> > * mCompConfig;
            /** Internal mutex */
            mutable SC_BasicMutex mLock;

        private: //members
            /** Flag to define whether the distyribution is completed */
            bool mDistributed;
            
    };


} // namespace

#endif
