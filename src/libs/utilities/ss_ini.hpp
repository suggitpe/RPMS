#ifndef __ss_ini_hpp
#define __ss_ini_hpp

#include <base/st_singleton.hpp>
#include <base/sa_baseobject.hpp>
#include <base/st_spointer.hpp>
#include <tools/sc_recursivemutex.hpp>

#include <string>
#include <map>

namespace rpms
{

    // pre decl
    class SC_SectionMap;
    
    /** 
     * @author:  Peter Suggitt (2005)
     * @note <b>Ini file class</b>\n 
     * This class is used to house ini file settings It has been created as a 
     * singleton so that it does not end up creating lots of the same object It 
     * has been made thread safe through mutex locking */
    
    /** Singleton configuration file class */
    class SS_Ini : public ST_Singleton<SS_Ini>
    {
        friend class ST_Singleton<SS_Ini>;
        public:
            /** virtual dtor */
            virtual ~SS_Ini();
    
        private:
            /** default ctor */
            SS_Ini();
            /** hidden copy ctor */
            SS_Ini( const SS_Ini & aRhs );
            /** hidden assignment operator */
            SS_Ini & operator=( const SS_Ini &aRhs );
    
        public:
            /** Loads aFilename into the structure */
            void initialise( const std::string &aFileName );
            /** returns a copy of a given section identified by aSectionName */
            ST_SPointer<SC_SectionMap> getSection( const std::string &aSectionName ) const;
            /** outputs debug level information about the contents of the ini file */
            std::string debug() const;
    
            /** gets a string from the underlying data container */
            std::string getString( const std::string &aSectionName, const std::string &aName ) const;
            /** gets an int from the underlying data container */
            int getInt( const std::string &aSectionName, const std::string &aName ) const;
            /** gets an unsigned long from the underlying data container */
            unsigned long getULong( const std::string &aSectionName, const std::string &aName ) const;
            /** gets a boolean from the underlying data container */
            bool getBool( const std::string &aSectionName, const std::string &aName ) const;
    
        private:
            /** typedef to make life easier */
            typedef std::map< std::string, ST_SPointer<SC_SectionMap> > iniMap;
            /** typedef to make life easier */
            typedef std::pair< std::string, ST_SPointer<SC_SectionMap> > iniPair;
            /** states whether we actually have any data for the  */
            bool mHasData;
            /** add a new section */
            void add( const std::string &aName, const ST_SPointer<SC_SectionMap> &aSection );
            /** map holding the sections */
            iniMap * mSectionData;
            /** name of the file loaded */
            std::string mFileName;
            /** mutex to lock for thread safety */
            mutable SC_RecursiveMutex mLock;
    };
    
    /** Data container used by SS_Ini to hold each of the configuration file sections */
    class SC_SectionMap : public SA_BaseObject
    {
        public:
            /** default ctor */
            SC_SectionMap( const std::string &aSectionName );
            /** construct a new one with some initial values */
            SC_SectionMap( const std::string &aName, const std::string &aValue );
            /** virtual dtor */
            virtual ~SC_SectionMap();
            /** construct a new one with an existing one (copy ctor) */
            SC_SectionMap( const SC_SectionMap &aRhs );
            /** construct a new one with an existing one (assignment operator) */
            SC_SectionMap & operator=( const SC_SectionMap &aRhs );
            /** add a new key/value pair to the section */
            void add( const std::string &aName, const std::string &aValue );
            /** debug the contents of the underlying map */
            std::string debug() const;
            /** returns a the std::string value associated with the key aKey */
            std::string getValue( const std::string &aKey ) const;
    
        private:
            /** hidden def ctor */
            SC_SectionMap();
            /** typedef to make life easier */
            typedef std::map<std::string, std::string> secMap;
            /** typedef to make life easier */
            typedef std::pair<std::string, std::string> secPair;
            /** pointer to a string/string map */
            secMap * mData;
            /** Name of the section */
            std::string mSecName;
    };

} // namespace

#endif
