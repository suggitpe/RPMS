//#############################
#include "sc_streamtonullreader.hpp"
#include "si_streamreaderlog.hpp"

#include <si_macros.hpp>
#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>

using rpms::SC_StreamToNullReader;

///////////////////////////////////////////
SC_StreamToNullReader::SC_StreamToNullReader(const std::string &aThreadName, const int aPipe)
    : SA_StreamReader(aThreadName, aPipe)
{
    TRY(SC_StreamToNullReader::SC_StreamToNullReader(const std::string &aThreadName, const int aPipe));
    CATCH;
}

///////////////////////////////////////////
SC_StreamToNullReader::~SC_StreamToNullReader()
{
    TRY(SC_StreamToNullReader::~SC_StreamToNullReader());
    CATCH;
}

///////////////////////////////////////////
void SC_StreamToNullReader::execute()
{
    TRY(SC_StreamToNullReader::execute());
        int bufsize = SS_Ini::instance()->getInt("streams", "stream.null.readbuffsize");
        char buf[bufsize+1];
        memset( buf, '\0', sizeof(buf) );

        bool shouldRead = true;
        ssize_t bytes = 0;
        while( shouldRead && mShouldRun )
        {
            bytes = ::read( mPipe, buf, bufsize );
            if( 0 == bytes )
            {
                shouldRead = false;
            }
        }
        RPMS_INFO( SRD_FILE_LG, "Stream to null file writing to completed work");
    CATCH;
}




