#ifndef __sa_baseapp_hpp
#define __sa_baseapp_hpp

#include <base/sa_application.hpp>

namespace rpms
{
    /**
     * This class is the base class for all applications.
     * It will contain the references to the signal handlers and also manage the 
     *      loading of the ini file and any other prerequisits for the application.
     * 
     * @author Peter Suggitt (2005)
     * @note Base class for all executables
     * */
    class SA_BaseApp
    {
        public:
            /** This is where all applications must initially call so that the 
             * base of the application is set up correctly
             * @param aArgC The number of arguments passed in
             * @param aArgV An array of arguments */
            int baseMain( int aArgC, char *aArgV[] );
            static void killApp();

        protected:
            /** Pure virtual method that is called from baseMain 
             * @param aArgC The number of arguments passed in
             * @param aArgV An array of arguments */
            virtual int execute( int aArgC, char *aArgV[] ) = 0;

            static SA_Application *mApp;

        private:
    };
}

#endif
