#ifndef __sc_basicmutex_hpp
#define __sc_basicmutex_hpp

#include <tools/sa_mutex.hpp>
#include <pthread.h>
#include <string>

namespace rpms
{

    /**
     * This class is a very simple implementation of a mutex class.  If the mutex is being used, 
     *      no one can lock it. If it is free then they can enter.
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SC_BasicMutex : public SA_Mutex
    {
        public:
            /** Main constructor */
            SC_BasicMutex( const std::string &aName );
            /** Virtual destructor */
            virtual ~SC_BasicMutex();
            /** Lock the mutex, if the mutex is currently locked it will wait */
            void lock();
            /** Unlock the mutex.  Any queued lock requests will be allowed in */
            void unlock();
            
        protected:
        private:
            /** The underlying mutex */
            pthread_mutex_t mLock;
            /** name of the mutex */
            std::string mName;
    };

} // namespace

#endif
