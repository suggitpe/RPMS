//#############################
#include "ss_ini.hpp"
#include "sx_ini.hpp"
#include <tools/st_guard.hpp>

#include <sys/stat.h>
#include <fstream>
#include <si_macros.hpp>

using rpms::SS_Ini;
using rpms::SC_SectionMap;
using rpms::ST_SPointer;

///////////////////////////////////////////
SS_Ini::SS_Ini() 
    : mHasData(false), mSectionData( new iniMap ), mFileName("Empty"), mLock("SS_Ini")
{
    TRY(SS_Ini::SS_Ini());
    CATCH;
}

///////////////////////////////////////////
SS_Ini::~SS_Ini()
{
    TRY(SS_Ini::~SS_Ini());
        delete mSectionData;
    CATCH;
}

///////////////////////////////////////////
SS_Ini::SS_Ini( const SS_Ini & aRhs )
    : mLock("SS_Ini")
{
    TRY(SS_Ini::SS_Ini( const SS_Ini & aRhs ));
        throw SX_Ini("Copy ctor is hidden");
    CATCH;
}

///////////////////////////////////////////
SS_Ini & SS_Ini::operator=( const SS_Ini &aRhs )
{
    TRY(SS_Ini & SS_Ini::operator=( const SS_Ini &aRhs ));
        throw SX_Ini("Assignment operator is hidden");
    CATCH;
}

///////////////////////////////////////////
/// Takes in the filename of a config file 
///     and parses it to extract the config data
void SS_Ini::initialise( const std::string &aFileName )
{
    TRY(SS_Ini::initialise( const std::string &aFileName ));
        if( mHasData )
        {
            throw SX_Ini("File already loaded .. need to implement reload");
        }
    
        // lock against other threads
        ST_Guard<SA_Mutex> m( &mLock );

        mFileName = aFileName;
        // make sure that the file actually exists
        struct stat stat_res;
        if( stat(aFileName.data(), &stat_res) != 0 )
        {
            throw SX_Ini("Cannot stat file [" + aFileName + "]"  );
        }

        unsigned int size = stat_res.st_size;
        std::ifstream inFile( aFileName.c_str(), std::ios::in );
        
        if( !inFile )
        {
            throw SX_Ini("Could not open [" + aFileName + "] for read");
        }
    
        std::string ln, nm, val;
        int pos;
        int end;
        int lnNum = 0;

        ST_SPointer<SC_SectionMap> section;
        std::string sectionName;

        while( getline(inFile, ln) )
        {
            pos = -1;
            ++lnNum;
            // get rid of all the rubbish and blank lines
            if( (!ln.length()) || (ln[0] == '#') || (ln[0] == ';') )
            {
                continue;
            }

            // now we look for a new section
            if( ln[0] == '[')
            {
                pos = ln.find(']');
                if( pos == -1 )
                {
                    throw SX_Ini(std::string("Malformed section header in line (") 
                        + ") of " + aFileName + "\n\t" + ln );
                }
                sectionName = ln.substr(1, ln.length() -2);
                section = new SC_SectionMap(sectionName);
                // now we add to the main data
                add( sectionName, section );
                continue;
            }

            // guard against any rogue ini settings
            if( section.empty() )
            {
                throw SX_Ini( "Malformed ini file line (): not a part of a section [" + ln + "]" );
            }

            // if we get this far then we have raw config settings
            pos = ln.find('=');
            if(pos == -1)
            {
                throw SX_Ini(std::string("Malformed configuration setting in line (") 
                    + ") of " + aFileName + "\n\t" + ln );
            }

            end = ln.find(' ');
        
            nm = ln.substr(0, pos);
            if( end == -1 ) // no spaces in the line
                val = ln.substr( pos+1);
            else
                val = ln.substr( pos+1, end - pos - 1);

            section->add(nm, val);
        }
        inFile.close();
        mHasData = true;
    CATCH;
}

///////////////////////////////////////////
void SS_Ini::add( const std::string &aName, const ST_SPointer<SC_SectionMap> &aSection )
{
    TRY(SS_Ini::add( const std::string &aName, const ST_SPointer<SC_SectionMap> &aSection ));
        iniMap::iterator p = mSectionData->find(aName);
        if( p != mSectionData->end() )
        {
            throw SX_Ini("Attempted to overwrite an existing section [" + aName + "]");
        }
        mSectionData->insert( iniPair(aName, aSection) );
    CATCH;
}

///////////////////////////////////////////
std::string SS_Ini::debug() const
{
    TRY(SS_Ini::debug() const);
        std::string ret = "SS_Ini debug:\n";
        ret += "************************\n";
        if( mHasData && mSectionData->size() > 0 )
        {
            iniMap::iterator iter = mSectionData->begin();
            while( iter != mSectionData->end() )
            {
                ret += "section " + iter->first + ":\n";
                iter->second->debug();
                ++iter;
            }
        }
        else
        {
            ret += " - no data to report on\n";
        }
        ret+= "************************";
        return ret;
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_SectionMap> SS_Ini::getSection( const std::string &aSectionName ) const
{
    TRY(SS_Ini::getSection( const std::string &aSectionName ) const);
        if( !mHasData )
        {
            throw SX_Ini("Ini file has not been initialised");
        }
    
        // lock against other threads
        //ST_Guard<SA_Mutex> m( &mLock );

        iniMap::iterator i = mSectionData->find(aSectionName);
        if( i == mSectionData->end() )
        {
            throw SX_Ini("No such sectionName [" + aSectionName + "] found in file [" + mFileName + "]");
        }
        return i->second;
    CATCH;
}

///////////////////////////////////////////
std::string SS_Ini::getString( const std::string &aSectionName, const std::string &aName ) const
{
    TRY(SS_Ini::getString( const std::string &aSectionName, const std::string &aName ) const);
        // lock against other threads
        ST_Guard<SA_Mutex> m( &mLock );
        return getSection(aSectionName)->getValue(aName);
    CATCH;
}

///////////////////////////////////////////
unsigned long SS_Ini::getULong( const std::string &aSectionName, const std::string &aName ) const
{
    TRY(SS_Ini::getULong( const std::string &aSectionName, const std::string &aName ) const);
        // lock against other threads
        ST_Guard<SA_Mutex> m( &mLock );
        std::string test = getSection(aSectionName)->getValue(aName);
        if( "0" == test )
        {
            return 0;
        }
        else
        {
            int ret = atol(test.c_str());
            if( 0 == ret )
            {
                throw SX_Ini("Ini setting in section [" + aSectionName + "], key [" + aName + "] is not a numeric value (" + test + ")");
            }
            return ret;
        }
    CATCH;
}

///////////////////////////////////////////
int SS_Ini::getInt( const std::string &aSectionName, const std::string &aName ) const
{
    TRY(SS_Ini::getInt( const std::string &aSectionName, const std::string &aName ) const);
        // lock against other threads
        ST_Guard<SA_Mutex> m( &mLock );
        std::string test = getSection(aSectionName)->getValue(aName);
        if( "0" == test )
        {
            return 0;
        }
        else
        {
            int ret = atoi(test.c_str());
            if( 0 == ret )
            {
                throw SX_Ini("Ini setting in section [" + aSectionName + "], key [" + aName + "] is not a numeric value (" + test + ")");
            }
            return ret;
        }
    CATCH;
}

///////////////////////////////////////////
bool SS_Ini::getBool( const std::string &aSectionName, const std::string &aName ) const
{
    TRY(SS_Ini::getBool( const std::string &aSectionName, const std::string &aName ) const);
        // lock against other threads
        ST_Guard<SA_Mutex> m( &mLock );
        return ("true" == getSection(aSectionName)->getValue(aName))? true: false;
    CATCH;
}

// ##############################################################
// ##############################################################
// ##############################################################

///////////////////////////////////////////
SC_SectionMap::SC_SectionMap() 
    : mData( new secMap ), mSecName("unknown")
{
    TRY(SC_SectionMap::SC_SectionMap());
        throw SX_Ini("Cannot use def ctor");
    CATCH;
}

///////////////////////////////////////////
SC_SectionMap::SC_SectionMap( const std::string &aSectionName) 
    : mData( new secMap ), mSecName(aSectionName)
{
    TRY(SC_SectionMap::SC_SectionMap( const std::string &aSectionName));
    CATCH;
}

///////////////////////////////////////////
SC_SectionMap::SC_SectionMap( const std::string &aName, const std::string &aValue )
    : mData( new secMap )
{
    TRY(SC_SectionMap::SC_SectionMap( const std::string &aName, const std::string &aValue ));
        mData->insert( secPair(aName, aValue) );
    CATCH;
}

///////////////////////////////////////////
// dtor
SC_SectionMap::~SC_SectionMap()
{
    TRY(SC_SectionMap::~SC_SectionMap());
        delete mData;
    CATCH;
}

///////////////////////////////////////////
// copy ctor
SC_SectionMap::SC_SectionMap( const SC_SectionMap &aRhs )
    : SA_BaseObject(aRhs), mData( new secMap(*aRhs.mData) )
{
    TRY(SC_SectionMap::SC_SectionMap( const SC_SectionMap &aRhs ));
    CATCH;
}

///////////////////////////////////////////
SC_SectionMap & SC_SectionMap::operator=( const SC_SectionMap &aRhs )
{
    TRY(SC_SectionMap::operator=( const SC_SectionMap &aRhs ));
        if( this != &aRhs )
        {
            delete mData;
            SA_BaseObject::operator=(aRhs);
            mData = new secMap(*aRhs.mData);
        }
        return *this;
    CATCH;
}

///////////////////////////////////////////
void SC_SectionMap::add( const std::string &aName, const std::string &aValue )
{
    TRY(SC_SectionMap::add( const std::string &aName, const std::string &aValue ));
        secMap::iterator p = mData->find(aName);
        if( p != mData->end() )
        {
            throw SX_Ini("Attempted to overwrite an existing key [" + aName + "] in section [" + mSecName + "]");
        }
        mData->insert( secPair(aName, aValue) );
    CATCH;
}


///////////////////////////////////////////
std::string SC_SectionMap::debug() const
{
    TRY(SC_SectionMap::debug() const);
        secMap::iterator iter = mData->begin();
        std::string ret = "Section contents...\n";
        for( ; iter != mData->end(); ++iter )
        {
            ret += " - key [" + iter->first + "]\tvalue [" + iter->second + "]\n";
        }
        return ret;
    CATCH;
}
    
///////////////////////////////////////////
std::string SC_SectionMap::getValue( const std::string &aKey ) const
{
    TRY(SC_SectionMap::getValue( const std::string &aKey ) const);
        secMap::iterator iter = mData->find(aKey);
        if( iter == mData->end() )
        {
            throw SX_Ini("No such key [" + aKey + "] found in section [" + mSecName + "]");
        }
        return iter->second;
    CATCH;
}


