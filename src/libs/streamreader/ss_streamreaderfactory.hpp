#ifndef __ss_streamreaderfactory_hpp
#define __ss_streamreaderfactory_hpp

#include <base/st_singleton.hpp>
#include <base/st_spointer.hpp>

namespace rpms
{

    class SA_StreamReader;
    
    /**
     * Factory class for creating stream readers.
     * Used by the SC_ProcessWrapperThread class.
     * 
     * @author Peter Suggitt (2005)
     * @note Singleton class
     * */
    class SS_StreamReaderFactory : public ST_Singleton<SS_StreamReaderFactory>
    {
        friend class ST_Singleton<SS_StreamReaderFactory>;
        public:
            enum EStreamType {_STDOUT, _STDERR};
            /** Virtual destructor */
            virtual ~SS_StreamReaderFactory();
            /** Method to actually get the relevant reader object */
            ST_SPointer<SA_StreamReader> getReader(const int aPipe, const std::string aReaderType, 
                    const EStreamType aStreamType, const std::string &aProcessLogName, 
                    const std::string &aProcessInstance );
            /** This method will create space for a new log file to be created by firstly 
             *      checking that the directory exists (ini defined) and creating it if 
             *      necessary and secondly by looking at the file name that it will be 
             *      creating and versioning the files accordingly up to a maximum log 
             *      number, anything old than that and it will remove old versions */
            void createSpaceForLogFile( const std::string &aLogFileName );
        protected:
            /** Main constructor */
            SS_StreamReaderFactory();
        private:
    };

} // namespace

#endif
