#ifndef __sc_filesystemhelper_hpp
#define __sc_filesystemhelper_hpp

#include <string>

namespace rpms
{
    class SC_FileSystemHelper
    {
        public:
            /** This will recursively create a directory for you (acts like mkdir -p), it will create 
             *      all the directories as needed.  Unless it tries to create a directory on top of a file 
             *      or there are no permissions to alow the directory creation */
            static bool createDirectory( const std::string &aDirName );
            /** Ageing method for log files. This method will look in a given directory looking 
             *      for a given filename, if there is no file in there it will do nothing.  If there 
             *      are files in there it will append a '.x' suffix to the old log files up to a
             *      configured maximum logfiles.  Thus the end result will be an aged log file */
            static bool ageLogFile( const std::string &aDirName, const std::string& aFileName );
            /** This method will look throughout the path env variable looking for a given executable.  
             *      If it finds one and it is executable it will return true, else it will return false. */
            static bool testForExecutable( const std::string &aExeName );
            
        private:
            static bool makeDirectory( const std::string &aDirName, const mode_t aMode );
    };
} // namespace

#endif
