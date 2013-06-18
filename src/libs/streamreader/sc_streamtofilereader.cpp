//#############################
#include "sc_streamtofilereader.hpp"
#include "si_streamreaderlog.hpp"

#include <si_macros.hpp>
#include <logging/si_logging.hpp>
#include <utilities/ss_ini.hpp>

#include <stdio.h>

using rpms::SC_StreamToFileReader;

///////////////////////////////////////////
SC_StreamToFileReader::SC_StreamToFileReader(const std::string &aThreadName, const int aPipe, const std::string &aFilePostFix, const std::string &aLogFileName)
    : SA_StreamReader(aThreadName, aPipe), mFilePostFix(aFilePostFix), mLogFileName(aLogFileName)
{
    TRY(SC_StreamToFileReader::SC_StreamToFileReader(const std::string &aThreadName, const int aPipe, const std::string &aFilePostFix, const std::string &aLogFileName));
        
    CATCH;
}

///////////////////////////////////////////
SC_StreamToFileReader::~SC_StreamToFileReader()
{
    TRY(SC_StreamToFileReader::~SC_StreamToFileReader());
    CATCH;
}

///////////////////////////////////////////
void SC_StreamToFileReader::execute()
{
    TRY(SC_StreamToFileReader::execute());
        int bufsize = SS_Ini::instance()->getInt("streams", "stream.file.readbuffsize");
        int writeBuffer = SS_Ini::instance()->getInt("streams", "stream.file.writebuffsize");
        //double check to make sure that the main buffer is not smaller than the write buffer
        if( bufsize > writeBuffer )
        {
            writeBuffer = bufsize;
        }
        // prepare the buffers
        char buf[bufsize+1];
        memset( buf, '\0', sizeof(buf) );
        char writebuf[writeBuffer+1];
        memset( writebuf, '\0', sizeof(writebuf) );
        char * writeBuffPtr = writebuf;
        int pointer = 0;

        std::string fileName = mLogFileName + "." + mFilePostFix;
        RPMS_INFO( SRD_FILE_LG, "Stream to log file writing to [" + fileName + "]");

        bool shouldRead = true;
        ssize_t bytes = 0;
        // create the file access and open it for write
        FILE *file;
        file = fopen(fileName.c_str(), "w");
        if( !file )
        {
            shouldRead = false;
        }
        
        while( shouldRead && mShouldRun )
        {
            // read the data in the pipe into the read buffer
            bytes = ::read( mPipe, buf, bufsize );
            if( 0 == bytes )
            {
                shouldRead = false;
            }
            else
            {
                // here we should be populating a buff buffer.  as soon as the 
                //   buffer reaches a certain size we the write that to the actual 
                //   file itself.  This allows us to make sure that there is minimal I/O
                if( pointer+bytes > writeBuffer )
                {
                    writebuf[pointer] == '\0';
                    fwrite(writebuf, sizeof(char), pointer, file);
                    writeBuffPtr = writebuf;
                    pointer = 0;
                }
                // move the read in buffer contents to the write buffer
                if( bytes >= 0 )
                {
                    memcpy( writeBuffPtr, buf, bytes );
                    writeBuffPtr += bytes;
                    pointer += bytes;
                }
            }
        }
        // close up the file after purging the write buffer
        fwrite(writebuf, sizeof(char), pointer, file);
        fflush(file);
        fclose(file);
        RPMS_INFO( SRD_FILE_LG, "Stream to log file writing to completed work");
    CATCH;
}

///////////////////////////////////////////
