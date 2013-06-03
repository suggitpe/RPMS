#ifndef __si_macros_hpp
#define __si_macros_hpp

#include <assert.h>
#include <base/sx_exception.hpp>
#include <base/sx_unknown.hpp>
#include <iostream>

/**
 * <b>Macros header</b> \n
 * All macros go in here
 *
 * @author Peter Suggitt (2005)
 * @note Macro based
 * */

#define ASSERT( test, out )                     \
if( (test) == 0 )                               \
{                                               \
    std::cerr << out << std::endl;              \
    assert(test);                               \
}

#define TRY(method)                             \
static const char * methName = #method;         \
try {


#define CATCH                                   \
} \
catch( SX_Exception &ex )                       \
{                                               \
    ex.push(methName);                          \
    throw;                                      \
} \
catch(...)                                      \
{                                               \
    throw SX_Unknown(methName);                 \
}
    
#endif

