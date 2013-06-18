//#############################
#include "sc_streamtostreamreader.hpp"
#include "si_streamreaderlog.hpp"

#include <si_macros.hpp>
#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>

using rpms::SC_StreamToStreamReader;

///////////////////////////////////////////
SC_StreamToStreamReader::SC_StreamToStreamReader(const std::string &aThreadName, 
        const int aPipe, const bool aToStdOut )
    : SA_StreamReader(aThreadName, aPipe), mToStdOut(aToStdOut)
{
    TRY(SC_StreamToStreamReader::SC_StreamToStreamReader(const std::string &aThreadName, 
                const int aPipe, const bool aToStdOut ));
    CATCH;
}

///////////////////////////////////////////
SC_StreamToStreamReader::~SC_StreamToStreamReader()
{
    TRY(SC_StreamToStreamReader::~SC_StreamToStreamReader());
    CATCH;
}

///////////////////////////////////////////
void SC_StreamToStreamReader::execute()
{
    TRY(SC_StreamToStreamReader::execute());
        int bufsize = SS_Ini::instance()->getInt("streams", "stream.stream.readbuffsize");
        char buf[bufsize+1];
        memset( buf, '\0', sizeof(buf) );
        std::string out = "||";

        bool shouldRead = true;
        ssize_t bytes = 0;
        while( shouldRead && mShouldRun )
        {
            bytes = ::read( mPipe, buf, bufsize );
            if( 0 == bytes )
            {
                shouldRead = false;
            }
            else
            {
                out.append( buf, bytes);
            }
        }
        
        if( SS_Ini::instance()->getBool("streams", "stream.stream.removenewline") )
        {
            // spin through the string and get rid of all the new line chars
            std::string::iterator i = out.begin();
            while( i != out.end() )
            {
                if( *i == 0x0a ) // append any other ones here
                {
                    out.erase(i, i+1);
                    out.insert(i, 2, '|');
                }
                else
                {
                    ++i;
                }
            }
        }
        
        // now we have to do something with the output
        if( mToStdOut )
        {
            RPMS_INFO(SRD_STRM_LG, "Contents of std stream:" + std::string(out));
        }
        else
        {
            RPMS_ERROR(SRD_STRM_LG, "Contents of err stream:" + std::string(out));
        }
    CATCH;
}


///////////////////////////////////////////
