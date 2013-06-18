#ifndef __st_singleton_hpp
#define __st_singleton_hpp

#include <si_macros.hpp>

namespace rpms
{

    /** 
     * Singleton template implementation.
     * This class should be used as a baseclass for any class 
     *      that should be created as a base class
     *
     * @author Peter Suggitt (2005) 
     * @note Template Class
     * */
    template <class T>
    class ST_Singleton
    {
        public:
            /** singleton instance */
            static T* instance();
            
        protected:
            /** Hidden def ctor */
            ST_Singleton();
            /** virtual destructor - all singletons will derive from here */
            virtual ~ST_Singleton();
        private:
            /** instance pointer */
            static T* _instance;
            /** used to make sure the singleton is destroyed correctly */
            static void destroy();
    };
    
    ///////////// CLASS IMPL //////////////////
    template <class T>
    T* ST_Singleton<T>::instance()
    {
        if( ST_Singleton::_instance == 0 )
        {
            ST_Singleton::_instance = new T;
            std::atexit(ST_Singleton::destroy); // make sure it is cleaned up
            ASSERT( ST_Singleton::_instance != 0, "ST_Singleton<T>::instance() - failed to create singleton" );
        }
        return ST_Singleton::_instance;
    }
    
    ///////////////////////////////////////////
    template <class T>
    void ST_Singleton<T>::destroy()
    {
        if( ST_Singleton::_instance != 0 )
        {
            delete ST_Singleton::_instance;
            ST_Singleton::_instance = 0;
        }
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_Singleton<T>::ST_Singleton()
    {
        ASSERT(ST_Singleton::_instance == 0, "ST_Singleton<T>::ST_Singleton() - trying to create a new singleton when actually there is one already there");
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_Singleton<T>::~ST_Singleton()
    {
    }
    
    ///////////////////////////////////////////
    template <class T> T* ST_Singleton<T>::_instance = 0;

} // namespace

#endif
