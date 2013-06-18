//#############################
#include "sc_filesystemhelper.hpp"
#include "si_utilitieslog.hpp"
#include "sx_filehelper.hpp"

#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>
#include <utilities/st_conversion.hpp>

#include <sys/stat.h>
#include <queue>

using rpms::SC_FileSystemHelper;

///////////////////////////////////////////
bool SC_FileSystemHelper::createDirectory( const std::string &aDirName )
{
    // stat the file
    struct stat stat_res;
    if( 0 == ::stat(aDirName.c_str(), &stat_res) ) // dir exists
    {
        if(S_ISDIR(stat_res.st_mode) != 0)
        {
            // all is OK
            //RPMS_TRACE(UTL_FIL_LOG, "Directory already exists");
            return true;
        }
        else
        {
            RPMS_ERROR(UTL_FIL_LOG, "File [" + aDirName + "] exists but is not a directory.");
            throw SX_FileHelper("File [" + aDirName + "] exists but is not a directory.");
        }
    }
    else
    {
        if(SS_Ini::instance()->getBool("logging", "log.directories.createondemand"))
        {
            mode_t mode;
            mode = ACCESSPERMS;
            // now we have to actually create the directory .. this must be done in stages
            if( aDirName.at(0) == '/' )
            {
                int i = 1;
                int end = aDirName.size();
                while( end > i )
                {
                    i = aDirName.find('/', i);
                    if( i == -1 )
                    {
                        if( ! makeDirectory( aDirName, mode ) ) // this catches top level
                        {
                            RPMS_ERROR(UTL_FIL_LOG, "Failed to create directory [" + aDirName + "]");
                            return false;
                        }
                        break;
                    }

                    if(!makeDirectory( aDirName.substr(0, i), mode ))
                    {
                        RPMS_ERROR(UTL_FIL_LOG, "Failed to create directory [" + aDirName.substr(0, i) + "]");
                        return false;
                    }
                    ++i; // this is important in case the dir ends with a '/'
                }
            }
            else
            {
                throw SX_FileHelper("Attempt to create dir [" + aDirName
                        + "], aborted.  No relative directory names allowed.");
            }
        }
        else
        {
            // not allowed to create the dir
            throw SX_FileHelper("Dir [" + aDirName
                    + "] does not exist and the system is not configured to create the dir.");
        }
    }
    return true;
}

///////////////////////////////////////////
bool SC_FileSystemHelper::testForExecutable( const std::string &aExeName )
{
    TRY(SC_FileSystemHelper::testForExecutable( const std::string &aExeName ));
        std::string path = ::getenv("PATH");
        std::string::iterator iter = path.begin();
        std::queue<std::string> pathDirs;
        int count = 0;
        int end = 0;
        int start = 0;
        while( iter != path.end() )
        {
            if( *iter == ':' )
            {
                end = count - start;
                pathDirs.push( path.substr(start, end) );
                start = count+1;
            }
            ++iter;
            ++count;
        }
        // remember to get the last arg
        end = count - start;
        pathDirs.push( path.substr(start, end) );
        int sz = pathDirs.size();
        count = 0;
        struct stat stat_res;
        while( !pathDirs.empty() )
        {
            //RPMS_TRACE(UTL_FIL_LOG, "    - Checking for [" + pathDirs.front() + "/" + aExeName + "]"); 
            if( ::stat( ( pathDirs.front() + "/" + aExeName ).c_str(), &stat_res ) == 0 ) // we have found
            {
                if( S_ISREG(stat_res.st_mode) == 1 )
                {
                    // if the file is exe by all
                    if( stat_res.st_mode & S_IXOTH )
                    {
                        return true;
                    }
                    // if the file is exe by grp and we are part of that grp
                    else if( (stat_res.st_gid == ::getgid()) && stat_res.st_mode & S_IXGRP )
                    {
                        return true;
                    }
                    // if the file is exe by the user and we are that user
                    else if( (stat_res.st_uid == ::getuid()) && stat_res.st_mode & S_IXUSR )
                    {
                        return true;
                    }
                    RPMS_WARN(UTL_FIL_LOG, "Found [" + aExeName + "] in [" + pathDirs.front() 
                            + "] but it does not have the right file permisions for us");
                    return false;
                }
            }
            pathDirs.pop();
            ++count;
        }
        RPMS_WARN(UTL_FIL_LOG, "Could not find [" + aExeName + "] within the defined path (echo $PATH)");
        
        return false;
    CATCH;
}


///////////////////////////////////////////
bool SC_FileSystemHelper::ageLogFile( const std::string &aDirName, const std::string& aFileName )
{
    using rpms::convert;
    // first see if we need to actually do anything
    std::string path = aDirName + "/" + aFileName;
    struct stat stat_res;
    if( 0 != ::stat( path.c_str(), &stat_res ) )
    {
        // here we are only interested in creating space for the file
        return true;
    }

    // go through the files in the current system and rename them by appending a number to them
    int max = SS_Ini::instance()->getInt("logging", "log.max.logfiles");
    RPMS_TRACE(UTL_FIL_LOG, "Ageing log file [" + path + "]" );
    for( int i = max-2; i > 0; --i )
    {
        if( :: stat((path + "." + convert<std::string>(i)).c_str(), &stat_res) == 0 )
        {
            if( ::rename( (path + "." + convert<std::string>(i)).c_str(), (path + + "." + convert<std::string>(i+1)).c_str()) != 0 )
            {
                return false;
            }
        }
    }

    // now we just have to rename the main file to <name>.1
    if( ::rename(path.c_str(), (path + ".1").c_str()) != 0 )
    {
        return false;
    }
    
    return true;
}

///////////////////////////////////////////
bool SC_FileSystemHelper::makeDirectory( const std::string &aDirName, const mode_t aMode )
{
    struct stat stat_res;
    if( 0 != ::stat( aDirName.c_str(), &stat_res ) )
    {
        if( ::mkdir(aDirName.c_str(), aMode) != 0 )
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////
