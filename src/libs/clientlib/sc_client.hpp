#ifndef __sc_client_hpp
#define __sc_client_hpp

#include <base/sa_application.hpp>
#include <base/st_spointer.hpp>

namespace rpms
{

    /**
     *  This class is to be used by a client application to communicate with
     *  the system as a whole.  This class will invoke a connection with the 
     *  lead node and allow the user to issue commands to the 
     *  system (eg, start/stop/status)
     *
     *  @author Peter Suggitt (2005)
     *  @note <b>Concrete class containing the client side functionality</b>
     * */
    class SC_Client : public SA_Application
    {
        public:
            /** Standard constructor */
            SC_Client();
            /** destructor */
            ~SC_Client();
            /** shutdown method so we can actually stop the application nicely */
            virtual void shutdown();

            /** List of actions that can be performed by the client */
            enum EAction {NOTHING, START, KILL, STATUS, RECONFIGURE, RESTART_FAILURE, RESTART_APP_TYPE, STOP_DIRECTOR};
            /** Perfoms the actual task on the system 
             * @param aAction The action to take .. must be of type EAction
             * @param aParam the parameter to go with the action (can be null) */
            bool performTask( const EAction &aAction, const std::string &aParam);
            /** Wrapper method to deal with the processing of any returned XML
             * @param aXmlStatus Some XML to transform, this will be status XML that the client must transofrm to be easily readable */
            bool transformXml( const std::string &aXmlStatus );

        protected:
        private:
        
    };

} // namespace

#endif
