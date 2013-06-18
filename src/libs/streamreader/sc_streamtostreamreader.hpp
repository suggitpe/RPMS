#ifndef __sc_streamtostreamreader_hpp
#define __sc_streamtostreamreader_hpp

#include <streamreader/sa_streamreader.hpp>

namespace rpms
{

    /**
     * This class is a thread class to manage a stream and collect it in a buffer.  
     * As soon as the pipe is closed off, it will dump the contents of the buffer to a logging stream.
     *
     * @author Peter Suggitt (2005)
     * @note Concrete thread class
     * */
    class SC_StreamToStreamReader : public SA_StreamReader
    {
        public:
            /** Main constructor */
            SC_StreamToStreamReader(const std::string &aThreadName, const int aPipe, 
                    const bool aToStdOut );
            /** Virtual destructor */
            virtual ~SC_StreamToStreamReader();
        protected:
            /** Main functionality */
            virtual void execute();
        private:
    
        private: // members
            bool mToStdOut;
    };

} // namespace

#endif
