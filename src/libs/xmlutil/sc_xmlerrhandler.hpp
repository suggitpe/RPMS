#ifndef __sc_xmlerrhandler_hpp
#define __sc_xmlerrhandler_hpp

#include <xercesc/sax/ErrorHandler.hpp>

namespace rpms
{

    /**
     * @author Peter Suggitt (2005)
     * @note <b>Concrete DOM Parser Error Handler class</b>\n
     * Error handlers are passed into a parser so that there is 
     * a defined way for the parser to manage problems.
     * */
    
    /** XML Error handler - allows a more flexible approach to errors in xml files */
    class SC_XMLErrHandler : public XERCES_CPP_NAMESPACE::ErrorHandler
    {
        public:
            /** ctor */
            SC_XMLErrHandler();
            /** dtor */
            virtual ~SC_XMLErrHandler();
            /** derived pure virtual, used for parser warings */
            virtual void warning(const XERCES_CPP_NAMESPACE::SAXParseException& exc);
            /** derived pure virtual, used for parser errors */
            virtual void error(const XERCES_CPP_NAMESPACE::SAXParseException& ex);
            /** derived pure virtual, used for parser fatal errors */
            virtual void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& ex);
            /** derived pure virtual, used for resetting the err handler */
            virtual void resetErrors();
            /** determines if there are errors */
            bool hasErrors() const { return mErrsFound; }
    
        private:
            /** this comes from the config file .. allows the application to continue after errors */
            bool mFailOnErr;
            /** flag to the errors found */
            bool mErrsFound;
    };
    
} // namespace

#endif
