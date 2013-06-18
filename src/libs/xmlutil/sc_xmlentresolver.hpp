#ifndef __sc_xmlentresolver_hpp
#define __sc_xmlentresolver_hpp

#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>

namespace rpms
{

    /**
     * This is a derived class from the Xerces Entity Resolver class.  This is used 
     * to isolate and load up the relevant DTDs for the XML that is to be parsed.  This 
     * class is registered with the XML parser before it is 
     * used (parser->setEntityResolver(this)) 
     *
     * @author Peter Suggitt (2005)
     * @note <b>Entity Resolver class</b>
     * */
    class SC_XMLEntResolver : public XERCES_CPP_NAMESPACE::EntityResolver
    {
        public:
            /** This is the main derived method (pure virtual from base class) that is used 
             * to identify the actual DTD. Both params are taken from the raw XML file to be used 
             *      (see DOCTYPE definition in the XML file)
             * @param publicId eg "-//System/Type//lang"
             * @param systemId eg "some_file_name.dtd"
             * */
            XERCES_CPP_NAMESPACE::InputSource * resolveEntity (const XMLCh* const publicId, const XMLCh* const systemId);
    };

} // namespace

#endif
