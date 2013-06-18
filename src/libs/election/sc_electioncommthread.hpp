#ifndef __sc_electioncommthread_hpp
#define __sc_electioncommthread_hpp

#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>

namespace rpms
{

    // pre decl
    class SA_Election;
    class SA_Socket;
    
    /** 
     * This thread class is used by all the elections to manage the communications happening in an election
     *
     * @author Peter Suggitt (2005)
     * @note <b>Concrete class</b>
     * */
    class SC_ElectionCommThread : public SA_ThreadBase
    {
        public:
            /** create a new instance of the thread */
            static ST_SPointer<SC_ElectionCommThread> create( 
                    const ST_SPointer<SA_Election> &aElector );
            /** stop thread execution */
            virtual void stop();
            /** virtual dtor */
            virtual ~SC_ElectionCommThread();
            /** reset the thread and its contents */
            virtual void reset();
    
        protected:
            /** hidden ctor */
            SC_ElectionCommThread( 
                    const std::string &aThreadName, const ST_SPointer<SA_Election> &aElector );
            /** main run method */
            virtual void run();

        private:
            /** initialise */
            void init();
    
        private: // members
            /** run flag */
            bool mShouldRun;
            /** socket to recveinf election messages */
            ST_SPointer<SA_Socket> mRecvSocket;
            /** access to the main election object */
            ST_SPointer<SA_Election> mOwner;
    
    };

} // namespace

#endif
