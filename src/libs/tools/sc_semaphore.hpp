#ifndef __sc_semaphore_hpp
#define __sc_semaphore_hpp

#include <tools/sa_mutex.hpp>
#include <semaphore.h>

namespace rpms
{

    /** 
     * Semapore implementation 
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SC_Semaphore : public SA_Mutex
    {
        public:
            /** Constructor */
            SC_Semaphore();
            /** Destructir */
            virtual ~SC_Semaphore();
            /** Lock the semaphore */
            virtual void lock();
            /** Unlock the semaphore */
            virtual void unlock();
    
        protected:
        private:
            /** The underlying semaphore */
            sem_t mLock;
    };

} // namespace

#endif
