#ifndef __sa_xmldocument_hpp
#define __sa_xmldocument_hpp

#include <base/sa_baseobject.hpp>
#include <xercesc/dom/DOM.hpp>

namespace rpms
{

    /** 
     * Base class for all XML classes - wraps up document construction
     *
     * @author  Peter Suggitt (2005) 
     * @note    <b>Abstract XML Class</b> \n
     * */
    
    class SA_XMLDocument : public SA_BaseObject
    {
        public:
            /** Main dtor. Cleans up the Xerces env */
            virtual ~SA_XMLDocument();
            /** This is the main method that is used to do all the work with the XML
             * file.  It can be used as many times as you like and each call will 
             * clean up the previous load. */
            virtual void load() = 0;
            /** All derived objects should have a debug method */
            virtual void debug() const = 0;
            /** Helper method to get an attribute from a DOMNamedNodeMap */
            static std::string getAttribute( const XERCES_CPP_NAMESPACE::DOMNamedNodeMap * aMap, const std::string aAttName );
        
        protected:
            /** main string ctor */
            SA_XMLDocument();
            /** Accessor to the parsed document.
             * @return DOMDocument* 
             * @throw SX_XMLUtil if the document has not been populated */
            const XERCES_CPP_NAMESPACE::DOMDocument * getDoc() const;
            /** Initialises the parser, based on the configuration settings, parses the xml 
             *      document set up in the ctor and populates the internal DOMDocument.
             * @throw SX_XMLUtil */
            void createTreeFromFile(const std::string &aDocumentPath);
            /**  */
            void createTreeFromString(const std::string &aXMLString);
            /**  */
            void createTreeFromNewDocument(const std::string &aRootName, 
                                           const std::string &aPubId, 
                                           const std::string &aSysId);
            /** Creates a new document based on the root node name, public id and the system id
             * @param aRootName name of the root elem to create
             * @param aPubId publid id string
             * @param aSysId system id string */
            static XERCES_CPP_NAMESPACE::DOMDocument *buildNewDOMDocument( const std::string &aRootName, 
                                                                           const std::string &aPubId, 
                                                                           const std::string &aSysId );
            /** Used to debug the contents of the the document object */
            void debugDoc() const;
            /** Used to append child nodes to the main document */
            bool addChild( XERCES_CPP_NAMESPACE::DOMNode * aNode, XERCES_CPP_NAMESPACE::DOMNode * aParentNode = NULL );
            /** Creates a new element for this document */
            XERCES_CPP_NAMESPACE::DOMElement * createElementForDoc( const std::string &aName );
            /** transforms the underlying document and sends the result to stdout */
            bool transformDoc( const std::string &aXSLPath );
            /** This will remove all elements that have the passed in tag name, 
             *      returning the number of elements removed */
            int removeAllElementsForName( const std::string &aTagName );
    
        private:
            SA_XMLDocument( const SA_XMLDocument &aRhs );
            SA_XMLDocument& operator=( const SA_XMLDocument &aRhs );
            
        private: // members
            /** separate method to set up the parser options */
            XERCES_CPP_NAMESPACE::DOMDocument *mDoc;
            /** flag to show if the class has been initialised .. needed if we reinitialise it */
            bool mInitialised;
    };
    
} // namespace

#endif
