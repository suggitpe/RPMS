#ifndef __sa_streamreader_hpp
#define __sa_streamreader_hpp

#include <base/sa_threadbase.hpp>

namespace rpms
{

    /**
     * This class is a 
     *
     * @author Peter Suggitt (2005)
     * @note Abstract thread class
     * */
    class SA_StreamReader : public SA_ThreadBase
    {
        public:
            /** Main constructor */
            SA_StreamReader( const std::string &aThreadName, const int aPipe );
            /** Virtual destructor */
            virtual ~SA_StreamReader();
            /** Stop method */
            void stop();
        protected:
            /** Main run method */
            virtual void run();
            /** Pure virtual method for managing the stream */
            virtual void execute() = 0;
    
        private:
    
        protected: // members
            /** Read end of a pipe */
            int mPipe;
            /** Flag to allow us to stop the thread */
            bool mShouldRun;
    
    };

} // namespace

#endif
