#ifndef __sc_streamtonullreader_hpp
#define __sc_streamtonullreader_hpp

#include <streamreader/sa_streamreader.hpp>

namespace rpms
{

    /**
     * This class is a thread class to manage a stream and get rid of the output.  
     * This is really nothing more than a simple way to make sure that the output 
     *      of a pipe does not cause any instability in the application.
     *
     * @author Peter Suggitt (2005)
     * @note Concrete thread class
     * */
    class SC_StreamToNullReader : public SA_StreamReader
    {
        public:
            /** Main constructor */
            SC_StreamToNullReader(const std::string &aThreadName, const int aPipe);
            /** Virtual destructor */
            virtual ~SC_StreamToNullReader();
        protected:
            /** Main functionality */
            virtual void execute();
        private:
    };

} // namespace

#endif
