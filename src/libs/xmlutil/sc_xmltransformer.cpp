//#############################
#include "sc_xmltransformer.hpp"
#include <si_macros.hpp>

using rpms::SC_XMLTransformer;

///////////////////////////////////////////
SC_XMLTransformer::SC_XMLTransformer( const std::string &aXMLToTransform )
    : SA_XMLDocument(), mXML(aXMLToTransform)
{
    TRY(SC_XMLTransformer( const std::string &aXMLToTransform ));
    CATCH;
}

///////////////////////////////////////////
SC_XMLTransformer::~SC_XMLTransformer()
{
    TRY(SC_XMLTransformer::~SC_XMLTransformer());
    CATCH;
}

///////////////////////////////////////////
void SC_XMLTransformer::load()
{
    TRY(SC_XMLTransformer::load());
        createTreeFromString(mXML);
    CATCH;
}

///////////////////////////////////////////
void SC_XMLTransformer::debug() const
{
}

bool SC_XMLTransformer::transform( const std::string &aXSLPath  )
{
    TRY(SC_XMLTransformer::transform(const std::string &aXSLPath ));
        return transformDoc(aXSLPath);
    CATCH;
}


///////////////////////////////////////////
