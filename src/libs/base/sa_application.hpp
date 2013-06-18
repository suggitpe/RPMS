#ifndef __sa_application_hpp
#define __sa_application_hpp

#include <base/sa_baseobject.hpp>

namespace rpms
{
    /**
     * This class a simple base class (no implementation) to enforce 
     *      a pure virtual method called by the signal handler in SA_BaseApp
     *
     *  @author Peter Suggitt (2005)
     *  @note Abstract class
     * */
    class SA_Application : public SA_BaseObject
    {
        public:
            /** virtual methid for application instances to take up */
            virtual void shutdown() = 0;
    };
} // namepsace

#endif
