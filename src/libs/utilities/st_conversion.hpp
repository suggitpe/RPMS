#ifndef __st_conversion_hpp
#define __st_conversion_hpp

#include <string>
#include <sstream>

namespace rpms
{
    /** 
     * This very simple method will create an object of type O from
     * an input of class type I. 
     * \n Uses a string stream to do the conversion, <b>Warning</b> uses the ostream 
     * operators (>>) in implementation.
     *
     * @author  Peter Suggitt (2005) 
     * @note template method
     * @param i the object to convert 
     * @return The object converted to the new type. */
    template <class O, class I>
    O convert(const I & i)
    {
        std::stringstream stream;
        stream << i;
        O result;
        stream >> result;
        return result;
    }
}

#endif
