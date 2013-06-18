#ifndef __sc_clientapp_hpp
#define __sc_clientapp_hpp

#include <getopt.h>
#include "../base/sa_baseapp.hpp"

namespace rpms
{
    class SC_Client;
    
    /** This is used by the application to validate what the user passes in on the command line */
    static struct option long_opts[] = 
    {
        {"start",           0, 0, 's' },  // start the system
        {"kill",            0, 0, 'k' },  // stop the system
        {"verify",          0, 0, 'v' },  // get the system status
        {"reconfigure",     0, 0, 'r' },  // reconfigure the system
        {"bounceapp",       1, 0, 'b' },  // restart app type
        {"failure",         1, 0, 'f' },  // restart failed components
        //{"director",        0, 0, 'd' },  // stop director .. force election
        {"all",             0, 0, 'a' },  // stop director .. force election
        {0,0,0,0}
    };

    /**
     * This is the main client application.  
     * This will form the link between the actual clients and the system as 
     *      a whole enabling full communitation with the remote system
     *
     * @author Peter Suggitt (2005)
     * @note Concrete class for the client executable
     * */
    class SC_ClientApp : public SA_BaseApp
    {
        public:
        protected:
            /** The inherited execute method */
            virtual int execute( int aArgC, char *aArgV[] );
            /** To let the user know how to use the application when they 
             *      have done something wrong */
            static void usage();
        private:
    };
} // namespace

#endif

