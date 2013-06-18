#ifndef __sc_statequeue_hpp
#define __sc_statequeue_hpp

#include <base/sa_baseobject.hpp>

#include <queue>

namespace rpms
{

    /**
     * This class is a simple object that is meant to hold the information for the state queue.
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SC_StateQueueObject
    {
        public:
            /** enum for the different states */
            enum EState {_UNKNOWN, _START, _STOP, _CONFIGURE, _RESTART, _FAILSTART };
            /** Main constructor */
            SC_StateQueueObject( const EState aState, const std::string &aArg );
            /** Destructor */
            ~SC_StateQueueObject();
            /** copy constructor */
            SC_StateQueueObject(const SC_StateQueueObject &aRhs );
            /** assignment operator */
            SC_StateQueueObject& operator=(const SC_StateQueueObject &aRhs );
            /** Getter method for the state */
            EState getState() const { return mState; }
            /** Getter method for the Arg */
            std::string getArg() const { return mArg; }
    
        protected:
        private: // mmebers
            /** State member */
            EState mState;
            /** Arg in string form */
            std::string mArg;
    };
    
    /**
     * This class is a collection class responsible for manageing state transitions
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SC_StateQueue : public SA_BaseObject
    {
        public:
            /** Main constructor */
            SC_StateQueue();
            /** Virtual destructor */
            virtual ~SC_StateQueue();
            /** Add a new state to the statck */
            void push( const SC_StateQueueObject::EState aState, const std::string &aArg );
            /** Get the top most object on the queue */
            SC_StateQueueObject front() const;
            /** Pop the top item from the queue */
            void pop();
            /** Simple size getter on the underlying object */
            size_t size() const;
            /** Simple empty getter on the underlying queue */
            bool empty() const;
            
        protected:
        private:
            /** The underlying queue */
            std::queue<SC_StateQueueObject> * mQueue;
    };

} // namespace


#endif
