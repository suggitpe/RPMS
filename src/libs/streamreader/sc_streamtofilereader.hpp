#ifndef __sc_streamtofilereader_hpp
#define __sc_streamtofilereader_hpp

#include <streamreader/sa_streamreader.hpp>

namespace rpms
{

    /**
     * This class is a thread class to manage getting the contents of a stream to a file.
     *
     * @author Peter Suggitt (2005)
     * @note Concrete thread class
     * */
    class SC_StreamToFileReader : public SA_StreamReader
    {
        public:
            /** Main constructor */
            SC_StreamToFileReader(const std::string &aThreadName, const int aPipe, const std::string &aFilePostFix, const std::string &aLogFileName);
            /** Virtual destructor */
            virtual ~SC_StreamToFileReader();
        protected:
            /** Main functionality */
            virtual void execute();
        private: // members
            std::string mFilePostFix;
            std::string mLogFileName;
    };

} // namespace

#endif
