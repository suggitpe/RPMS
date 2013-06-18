#ifndef __sc_guard_hpp
#define __sc_guard_hpp

/** The standard RPMS namespace */
namespace rpms
{

    /**
     * This is a wrapper for the SA_Mutex classes.
     * This class can be very easily implemented using 
     *      the base class SA_Mutex as class T, thus the only
     *      thing needed is to construct the actual mutex through 
     *      the base class ctor
     *
     * @author Peter Suggitt (2005)
     * @note Template class
     **/
    template <class T>
    class ST_Guard
    {
        public:
            /** Ctor - Calls lock on the underlying mutex 
             * @param aMutex a mutex type object
             * */
            ST_Guard( T * aMutex );
            /** Dtor - Calls unlock on the underlying mutex */
            ~ST_Guard();
            
        private:
            /** Default ctor hidden - no point having it really */
            ST_Guard();
            /** Pointer to a mutex obj */
            T * mMutex;
    };
    
    ///////////// CLASS IMPL //////////////////
    template <class T>
    ST_Guard<T>::ST_Guard() 
        : mMutex(0)
    {
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_Guard<T>::ST_Guard( T * aMutex )
        : mMutex( aMutex )
    {
        mMutex->lock();
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_Guard<T>::~ST_Guard()
    {
        mMutex->unlock();
    }

} // namespace

#endif
