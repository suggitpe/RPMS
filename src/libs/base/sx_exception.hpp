#ifndef __sx_exception_hpp
#define __sx_exception_hpp

#include <string>
#include <stack>

namespace rpms
{

    /**
     *  Base class for all exceptions.  
     *  As all exceptions derive from this as thrown internally we can 
     *      manage the stack more efficeintly
     *
     *  @author Peter Suggitt (2005)
     *  @note Abstract class
     * */
    class SX_Exception
    {
        public:
            /** Returns the cause of the exception */
            virtual const std::string reason() const;
            /** Dumps a copy of the stack in string form - it is up to the caller to do something with the output */
            virtual const std::string stack() const;
            /** Virtual destructor */
            virtual ~SX_Exception();
            /** Push a new stack entry onto stack trace
             * @param aMeth a String name for the methid where the exception has flowed through */
            void push( const std::string &aMeth);
            /** Copy constructor 
             * @param aRhs an exception to copy from */
            SX_Exception( const SX_Exception &aRhs );
            /** Assignment operator must be visible 
             * @param aRhs an exception to assign from */
            SX_Exception& operator=( const SX_Exception &aRhs );
            /** Exception type - set in the constructor */
            virtual const std::string type() const;

        protected:
            /** String constructor, it is protected so that no-one actualy throws a base exception 
             * @param aReason The reason why the exception was thrown. */
            SX_Exception( const std::string &aReason );

        private:
            /** Simple reason holder */
            std::string mReason;
            /** This stack is used through the TRY/CATCH macros to detail where the exception has derived from */
            std::stack<std::string> * mStack;
    };

} // namespace

#endif
