#ifndef __sc_datetime_hpp
#define __sc_datetime_hpp

#include <string>

namespace rpms
{

    /**
     * @author Peter Suggitt (2005)
     * @note <b> class</b>
     *      This class is a 
     * */
    
    /** Simple class to wrap up the manaagement of dates and times */
    class SC_DateTime
    {
        public:
            /** Main constructor */
            SC_DateTime(const int aYear, 
                    const int aMonth, 
                    const int aDay, 
                    const int aHour, 
                    const int aMin, 
                    const int aSec);
            /** Virtual destructor */
            virtual ~SC_DateTime();
            /** static method to get the date as of now (uses localtime) */
            static SC_DateTime now();
            /** converts the date object to a string */
            std::string toString();
            /** gets the current time in seconds since Epoch (0 hours jan 1 1970) */
            static long getCurrentTimeInSecs();
            /**  */
            static long long int getCurrentTimeInMillis();
            
        protected:
                  
        private: // members
            int mYear;
            int mMonth;
            int mDay;
            int mHour;
            int mMin;
            int mSec;
    };

} // namespace

#endif
