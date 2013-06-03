#ifndef __si_exception_hpp
#define __si_exception_hpp

/**
 * <b>Exception macros</b>
 * This is used to define the actual content of the exceptions classes
 *
 * @author Peter Suggitt (2005)
 * @note Macro based
 * */

//////////////////////////////////////////////////////////////////
#define EXCEPTION_DECL_STRT( classname, derivedclassname )   \
namespace rpms                                          \
{                                                       \
class classname : public derivedclassname               \
{                                                       \
    public:                                             \
        classname(const std::string &aName);            \
        classname(const classname &aRhs);               \
        classname& operator=(const classname &aRhs);    \
    private:                                            \
        const std::string type() const;

//////////////////////////////////////////////////////////////////
#define EXCEPTION_DECL_END                              \
};                                                      \
}

//////////////////////////////////////////////////////////////////
#define EXCEPTION_IMPL( classname, derivedclassname )   \
rpms::classname::classname( const std::string &aName )        \
   : derivedclassname(aName)                            \
{                                                       \
}                                                       \
                                                        \
const std::string rpms::classname::type() const               \
{                                                       \
    return #classname;                                  \
}                                                       \
                                                        \
rpms::classname::classname(const classname &aRhs)             \
    : derivedclassname(aRhs)                            \
{                                                       \
}                                                       \
                                                        \
rpms::classname& rpms::classname::operator=( const classname &aRhs ) \
{                                                       \
    if( this != &aRhs  )                                \
    {                                                   \
        derivedclassname::operator=(aRhs);              \
    }                                                   \
    return *this;                                       \
}

#endif
