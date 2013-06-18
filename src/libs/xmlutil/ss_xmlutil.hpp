#ifndef __ss_xmlutil_hpp
#define __ss_xmlutil_hpp

#include <base/st_singleton.hpp>
#include <tools/sc_recursivemutex.hpp>
#include <string>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>

namespace rpms
{

    /**
     * This class wraps up the Xerces functional API (parsers/writers/etc)
     *
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     * */
    class SS_XMLUtil : public ST_Singleton<SS_XMLUtil>
    {
        friend class ST_Singleton<SS_XMLUtil>;
        public:
            /** Virtual destructor */
            virtual ~SS_XMLUtil();
            /** Static method that deals with conversion from XMLCh* to a std string
             * @param aXMLString XMLCh* that you want to be converted
             * @return string that can then be used in normal operations */
            static std::string convertToString(  const XMLCh* aXMLString );
            /** Static method to take a document and create an XML representation of the tree
             * @param aDoc node to parse
             * @return string the end XML result */
            std::string serializeXML( const XERCES_CPP_NAMESPACE::DOMDocument * aDoc );
    
    
            XERCES_CPP_NAMESPACE::DOMDocument * parseXMLString( const std::string &aXML );
    
            XERCES_CPP_NAMESPACE::DOMDocument * parseXMLFile( const std::string &aFile );
    
    
        protected:
        private:
            /** load all the settings into the system  */
            void initialise();
            /** Main constructor */
            SS_XMLUtil();
            /** hidden copy ctor */
            SS_XMLUtil( const SS_XMLUtil &aRhs );
            /** hidden assignment operator */
            SS_XMLUtil& operator=( const SS_XMLUtil &aRhs );
            /** separate method to set up the parser options */
            void setParserOptions();
            /** frees up the memory and objects created by the parser */
            void resetParser();
            /** separet method to manage the writer options */
            void setWriterOptions();
            
            // MEMBERS
            /** lock object */
            mutable SC_RecursiveMutex mLock;
            /** initialisation flag */
            bool mInitialised;
    
            /** pointer to the parser */
            XERCES_CPP_NAMESPACE::XercesDOMParser   *mParser;
            /** poiner to an error handler */
            XERCES_CPP_NAMESPACE::ErrorHandler      *mErrHandler;
            /** pointer to the entity resolver */
            XERCES_CPP_NAMESPACE::EntityResolver    *mEntResolver;
            /** pointer to a writer object */
            XERCES_CPP_NAMESPACE::DOMWriter         *mWriter;
            
    };

} // namespace

#endif
