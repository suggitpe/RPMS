#ifndef __sa_baseobject_hpp
#define __sa_baseobject_hpp

#include <iostream>

namespace rpms
{

    /**
     *  Base class used for all objects that want to make 
     *      use of a ST_SPointer<T>.
     *  Context for abstract base object type that will allow smart 
     *      pointer refcounting delegation
     *
     *  @author Peter Suggitt (2005)
     *  @note Abstract class
     * */
    class SA_BaseObject
    {
        public:
            /** Default constructor */
            SA_BaseObject();
            /** Copy constructor @param aRhs an object to copy from */
            SA_BaseObject(const SA_BaseObject &aRhs);
            /** Virtual destructor */
            virtual ~SA_BaseObject();
            /** Assignment operator @param aRhs an opject to assign from  */
            SA_BaseObject & operator=(const SA_BaseObject& aRhs);
            /** Returns the number of pointers with references to the object */
    	    virtual unsigned int lockCount() const { return mRefCount; }
            /** Increments the internal lock count */
    	    virtual unsigned int incRefCount() const { return ++mRefCount; }
            /** Decrements the internal lock count */
    	    virtual unsigned int decRefCount() const { return --mRefCount; }

        protected: // members
            /** Object type */
            unsigned int mTypeFlag;

        private: // members
            /** Internal lock */
            mutable unsigned int mRefCount;
    };

} // namespace

#endif

