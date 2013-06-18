#ifndef __sc_recursivemutex_hpp
#define __sc_recursivemutex_hpp

#include <tools/sa_mutex.hpp>
#include <pthread.h>
#include <string>

namespace rpms
{

    /** 
     * This is a concrete wrapper implementation of the pthread mutex locks 
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SC_RecursiveMutex : public SA_Mutex
    {
        public:
            /** ctor */
            SC_RecursiveMutex( const std::string &aName );
            /** dtor */
            virtual ~SC_RecursiveMutex();
            /** lock the mutex */
            void lock();
            /** unlock the mutex */
            void unlock();
            
        protected:
        private:
            /** The actual underlying mutex */
            pthread_mutex_t mLock;
            /** name of the mutex */
            std::string mName;
            /** counter so we can be recursive */
            unsigned int mCounter;
            /** threadid of the locking thread */
            unsigned int mThreadId;
    };

} // namespace

#endif
