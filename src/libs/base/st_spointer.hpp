#ifndef __st_spointer_hpp
#define __st_spointer_hpp

namespace rpms
{
    
    /**
     * Smart pointer implementation.
     * This implamentation is deemed intrusive because it requires that every object has a lock object 
     *          that can bee accsesed by the amart pointer implementation.
     *
     * @author Peter Suggitt (2005)
     * @note Template Class
     * */
    template <class T>
    class ST_SPointer
    {
        public:
            /** def ctor */
            ST_SPointer();
            /** pointer ctor @param aData an object pointer that must derived from SA_BaseObject */
            ST_SPointer( T* aData );
            /** copy ctor */
            ST_SPointer( const ST_SPointer &aRhs );
            /** virtual dtor */
            virtual ~ST_SPointer();
            /** assignment operator @param aRhs an object of type ST_SPointer<T> */
            ST_SPointer & operator=(const ST_SPointer & aRhs );
            /** checks of both the pointers point to same ref or both are empty @param aRhs and object of type ST_SPointer<T> to compare against */
            bool operator==( const ST_SPointer &aRhs ) const;
            /** reverse check of operator== @param aRhs and object of type ST_SPointer<T> to compare against */
            bool operator!=( const ST_SPointer &aRhs ) const;
            /** dereference operator */
            T& operator*() const;
            /** dereference the underlying pointer  */
            T* operator->() const;
            /** dereference the underlying pointer */
            const T * data() const;
            /** dereference the underlying pointer */
            T * data();
            /** shows if the pointer is empty */
            inline bool empty() const;
            /** displays the num of pointers pointing to underlying */
            int locks() const;
            
        private:
            /** underlying pointer */
            T* mData;
    };
    
    ///////////// CLASS IMPL //////////////////
    template <class T>
    ST_SPointer<T>::ST_SPointer( T* aData) :
        mData(aData)
    {
        if( !empty() )
        {
            mData->incRefCount();
        }
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_SPointer<T>::ST_SPointer( const ST_SPointer &aRhs ) :
        mData(aRhs.mData)
    {
        if( !empty() )
        {
            mData->incRefCount();
        }
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_SPointer<T>::ST_SPointer()
        : mData(0)
    {
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_SPointer<T>::~ST_SPointer()
    {
        if( !empty() && (0 == mData->decRefCount()) ) // else segmentation faults
        {
            delete mData;
        }
    }
    
    ///////////////////////////////////////////
    template <class T>
    ST_SPointer<T> & ST_SPointer<T>::operator=( const ST_SPointer & aRhs )
    {
        if( this != &aRhs)
        {
            if( !empty() && (mData->decRefCount() == 0) )
            {
                delete mData;
            }
            mData = aRhs.mData;
            if( !empty() )
            {
                mData->incRefCount();
            }
        }
        return *this;
    }
    
    ///////////////////////////////////////////
    template <class T>
    bool ST_SPointer<T>::operator==( const ST_SPointer &aRhs ) const
    {
        if( aRhs.mData != 0 && mData !=0 )
        {
            return (*aRhs.mData == *mData);
        }
        else
        {
            return (aRhs.mData == 0 && mData == 0);
        }
    }
    
    ///////////////////////////////////////////
    template <class T>
    bool ST_SPointer<T>::operator!=( const ST_SPointer &aRhs ) const
    {
        return !( *this == aRhs );
    }
    
    ///////////////////////////////////////////
    template <class T>
    T & ST_SPointer<T>::operator*() const
    {
        // throw if null?
        return *mData;
    }
    
    ///////////////////////////////////////////
    template <class T>
    T * ST_SPointer<T>::operator->() const
    {
        // throw if null?
        return mData;
    }
    
    ///////////////////////////////////////////
    template <class T>
    const T * ST_SPointer<T>::data() const
    {
        // throw if null?
        return mData;
    }
    
    ///////////////////////////////////////////
    template <class T>
    T * ST_SPointer<T>::data()
    {
        // throw if null?
        return mData;
    }
    
    ///////////////////////////////////////////
    template <class T>
    bool ST_SPointer<T>::empty() const
    {
        return (mData == 0);
    }
    
    ///////////////////////////////////////////
    template <class T>
    int ST_SPointer<T>::locks() const
    {
        if( empty() )
        {
            return 0;
        }
        return mData->lockCount();
    }

} // namespace

#endif

