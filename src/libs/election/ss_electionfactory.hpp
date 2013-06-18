#ifndef __ss_electionfactory_hpp
#define __ss_electionfactory_hpp

#include <base/st_singleton.hpp>
#include <base/st_spointer.hpp>
#include <tools/sc_recursivemutex.hpp>

namespace rpms
{

    class SA_Election;
    
    /** This is a factory class to create the right type of election */
    class SS_ElectionFactory : public ST_Singleton<SS_ElectionFactory>
    {
        friend class ST_Singleton<SS_ElectionFactory>;
        public:
            virtual ~SS_ElectionFactory();
            ST_SPointer<SA_Election> getElection( const bool aElectable = false );

        protected:
            SS_ElectionFactory();

        private:
            void init( const bool aElectable );

        private: // memnbers
            std::string mElectionType;
            bool mElectable;
            mutable SC_RecursiveMutex mLock;
            ST_SPointer<SA_Election> mData;
    };

} // namespace

#endif
