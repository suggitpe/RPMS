#ifndef __sa_mutex_hpp
#define __sa_mutex_hpp

namespace rpms
{

    /** 
     * Base class for all mutual exclusion locks  
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SA_Mutex
    {
        public:
            /** This is just to make sure that the dtor is called on 
             *      derived classes */
            virtual ~SA_Mutex() {};
            /** pure virtual method to lock mutex */
            virtual void lock() = 0;
            /** pure virtual method to unlock mutex */
            virtual void unlock() = 0;
    };

} // namespace

#endif
