#ifndef __sc_daemonapp_hpp
#define __sc_daemonapp_hpp

#include "../base/sa_baseapp.hpp"

namespace rpms
{
    class SC_Daemon;
    /**
     * This is the main daemon application.  
     * This is a simple execute and leave application.  As it instantiates an instance
     *      of the SC_Daemon, it will just leave it running until it dies on its own or 
     *      the killApp methid is called.
     *
     * @author Peter Suggitt (2005)
     * @note Concrete class for the daemon executable
     * */
    class SC_DaemonApp : public SA_BaseApp
    {
        public:
        protected:
            /** The inherited execute method */
            virtual int execute( int aArgC, char *aArgV[] );
        private:
    };
} // namespace

#endif

