#ifndef __sc_xmltransformer_hpp
#define __sc_xmltransformer_hpp

#include <xmlutil/sa_xmldocument.hpp>

namespace rpms
{

    /** Class to transform a piece of XML through an XSL transformer and output the results */
    class SC_XMLTransformer : public SA_XMLDocument
    {
        public:
            SC_XMLTransformer( const std::string &aXMLToTransform );
            virtual ~SC_XMLTransformer();
            virtual void load();
            virtual void debug() const;
            bool transform( const std::string &aXSLPath );
        protected:
        private:
            std::string mXML;
    };

} // namespace

#endif
