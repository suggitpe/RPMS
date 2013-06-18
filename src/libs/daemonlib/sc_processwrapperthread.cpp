//#############################
#include "sc_processwrapperthread.hpp"
#include "sx_process.hpp"
#include "si_daemonlog.hpp"

#include <si_macros.hpp>
#include <utilities/st_conversion.hpp>

#include <logging/si_logging.hpp>
#include <utilities/sc_datetime.hpp>
#include <utilities/sc_filesystemhelper.hpp>
#include <utilities/sc_processinfo.hpp>
#include <utilities/ss_ini.hpp>
#include <streamreader/sa_streamreader.hpp>
#include <streamreader/ss_streamreaderfactory.hpp>
#include <xmlutil/sc_xmlstatus.hpp>
#include <xmlutil/sc_xmlsysconfig.hpp>
#include <xmlutil/sc_xmlprocstats.hpp>

#include <queue>
#include <sys/wait.h>

using rpms::SC_ProcessWrapperThread;
using rpms::SC_ProcessConfig;
using rpms::SC_ComponentStatus;
using rpms::SC_CompStat;
using rpms::ST_SPointer;
using rpms::convert;

///////////////////////////////////////////
ST_SPointer<SC_ProcessWrapperThread> SC_ProcessWrapperThread::create( const std::string &aThreadName, 
        const SC_ProcessConfig &aProcConfig )
{
    TRY(SC_ProcessWrapperThread::create( const std::string &aThreadName, const SC_ProcessConfig &aProcConfig ));
        return new SC_ProcessWrapperThread( aThreadName, aProcConfig );
    CATCH;
}

///////////////////////////////////////////
SC_ProcessWrapperThread::SC_ProcessWrapperThread( const std::string &aThreadName, 
        const SC_ProcessConfig &aProcConfig ) 
    : SA_ThreadBase(aThreadName), mLock("ProcessWrapperThread_lock"), mRunProcess(true), mFailed(false), 
        mProcTimeStamp("Not set"), mChildPID(0), mExternalStop(false), mStdOutReader(0), mStdErrReader(0),
        mReconfChecked(false)
{
    TRY(SC_ProcessWrapperThread::SC_ProcessWrapperThread( const std::string &aThreadName, 
                const SC_ProcessConfig &aProcConfig ) );
        RPMS_TRACE( DMN_PRC_LOG, "New process wrapper thread created for [" + aProcConfig.getInternalName() + "]" );
        mProcData = new SC_ProcessConfig(aProcConfig);
        mRetryTimes = new std::deque<long>;
        mProcInfo = new SC_ProcessInfo(); // default it to create one looking at the local process to avoid null pointers
    CATCH;
}

///////////////////////////////////////////
SC_ProcessWrapperThread::~SC_ProcessWrapperThread()
{
    TRY(SC_ProcessWrapperThread::~SC_ProcessWrapperThread());
        delete mRetryTimes;
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_ProcessConfig> SC_ProcessWrapperThread::getProcData() const
{
    return mProcData;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::stop()
{
    TRY(SC_ProcessWrapperThread::stop());
        // in here we could use lots of logic to make sure that the process actually dies
        //      but we should not be thinking in that way.  As this is a process management 
        //      tool, there is no way that we can tell what the 
        mLock.lock();
        mExternalStop = true;
        if( mChildPID != 0 )
        {
            ::kill( mChildPID, SIGTERM );
            RPMS_TRACE(DMN_PRC_LOG, "Sent SIGTERM to process ID [" + convert<std::string>(mChildPID) + "] (" + mProcData->getCompName() + ") .. stopping readers");
            if( !mStdOutReader.empty() )
            {
                mStdOutReader->stop();
            }

            if( !mStdErrReader.empty() )
            {
                mStdErrReader->stop();
            }

            if( !mStdOutReader.empty() )
            {
                mStdOutReader->join();
            }

            if( !mStdErrReader.empty() )
            {
                mStdErrReader->join();
            }

        }
        mLock.unlock();
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::initialise()
{
    TRY(SC_ProcessWrapperThread::initialise());
        mLoggingDir = SS_Ini::instance()->getString("directories", "log");
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_ComponentStatus> SC_ProcessWrapperThread::getCompStatus() const
{
    TRY(SC_ProcessWrapperThread::getCompStatus() const);
        if( mChildPID != 0  )
        {
            mProcInfo->load();
            return new SC_ComponentStatus( 
                    getProcData(), 
                    isRunning(), 
                    mProcTimeStamp, 
                    mRetryTimes->size(), 
                    mChildPID, 
                    mProcInfo->getLatestRss(),
                    mProcInfo->getRssAvg(),
                    mProcInfo->getCpuTime(),
                    mProcInfo->getCpuAvgPerSec() );
        }
        return new SC_ComponentStatus( getProcData(), false, mProcTimeStamp, 0, 0, 0, 0, 0, 0);
    CATCH;
}

///////////////////////////////////////////
ST_SPointer<SC_CompStat> SC_ProcessWrapperThread::getProcStats() const
{
    TRY(SC_ProcessWrapperThread::getProcStats() const);
        if(mChildPID != 0)
        {
            mProcInfo->load();
            return new SC_CompStat(
                        getProcData(),
                        mProcInfo->getCpuAvgPerMin(),
                        mProcInfo->getLatestRss());
        }
        return new SC_CompStat(getProcData(), 0, 0);
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::run()
{
    TRY(SC_ProcessWrapperThread::run());
        mLock.lock();
        initialise();
        mLock.unlock();

        // test to make sure that the cmd line exe exists and is executable by us
        if( ! SC_FileSystemHelper::testForExecutable(mProcData->getCompName()) )
        {
            mProcTimeStamp = "Process not executable";
            return;
        }
        
        // here we take one of two routes .. either to run once or many times
        try
        {
            if( SC_ProcessConfig::_CONTINUOUS == mProcData->getRunType() )
            {
                runProcessContinuously();
            }
            else if( SC_ProcessConfig::_ONCE == mProcData->getRunType() )
            {
                runProcessOnce();
            }
            else
            {
                RPMS_ERROR( DMN_PRC_LOG, "Process run type not specified for [" + mProcData->getCompName() 
                        + "], should be either \"continuous\" or \"once\"");
            }
        }
        catch( SX_Exception &ex )
        {
            RPMS_ERROR( DMN_PRC_LOG, "Problem in execution of process: " + ex.reason() );
        }
        mStdOutReader->join();
        mStdErrReader->join();
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::runProcessContinuously()
{
    TRY(SC_ProcessWrapperThread::runProcessContinuously());
        bool run = mRunProcess;
        while( run && !mFailed && !mExternalStop )
        {
            // FIRST: check whether we should actually start the process
            if( 0 != mRetryTimes->size() )
            {
                if( checkFailureRate() )
                {
                    RPMS_DEBUG( DMN_PRC_LOG, "Restarting instance of [" + mProcData->getCompName() 
                            + "] .. retry rate now [" + convert<std::string>(mRetryTimes->size()) 
                            + "/" + convert<std::string>(mProcData->getMaxRetries()) + "]" );
                }
                else
                {
                    RPMS_ERROR( DMN_PRC_LOG, "Process [" + mProcData->getCompName() 
                            + "] has exceeded its retry limit");
                    mFailed = true;
                    mRetryTimes->clear(); // clean up
                    return;
                }
            }
            
            // SECONDLY: we set up the relevant items that the running process will need
            mRetryTimes->push_back(SC_DateTime::getCurrentTimeInSecs());

            // this is where we actually do the process execution
            int procReturnVal = forkProcess();
            if( !evaluateProcessReturn( procReturnVal ) )
            {
                RPMS_WARN( DMN_PRC_LOG, "Process ended badly so will not restart it" );
                RPMS_INFO( DMN_PRC_LOG, "Please try and run [" + mProcData->getCompName() + " " 
                        + mProcData->getArgs() + "] manually" );
                mFailed = true;
            }

            // to test whether we should enter the loop again
            run = mRunProcess;
        }
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::runProcessOnce()
{
    TRY(SC_ProcessWrapperThread::runProcessOnce());
        if( !mExternalStop )
        {
            // this is where we actually do the process execution
            int procReturnVal = forkProcess();
        }
    CATCH;
}

///////////////////////////////////////////
int SC_ProcessWrapperThread::forkProcess()
{
    TRY(SC_ProcessWrapperThread::forkProcess());
        mProcTimeStamp = SC_DateTime::now().toString();
        RPMS_DEBUG(DMN_PRC_LOG, "Starting process [" + mProcData->getCompName() + "] at [" + mProcTimeStamp + "]");
       
        int ret;
        if( 0 != pipe(mStdIn) || 0 != pipe(mStdOut) || 0 != pipe(mStdErr) )
        {
            RPMS_ERROR( DMN_PRC_LOG, "Failed to create pipes for procesess" );
            mFailed = true;
        }

        // set this up here so we do not keep regening it
        std::string commandLine = mProcData->getCompName();
        if( !mProcData->getArgs().empty() )
        {
            commandLine += " " + mProcData->getArgs();
        }
        
        std::string logName;
        if( mProcData->getLoggingName().empty() ) 
        {
            logName = mProcData->getCompName();
        }
        else
        {
            logName = mProcData->getLoggingName();
        }

        // create the stdout log file if needed
        if( mProcData->getStdOut() == "file")
        {
            SS_StreamReaderFactory::instance()->createSpaceForLogFile( logName + "_" + convert<std::string>(mProcData->getSysInstance()) + ".log" );
        }

        // create the stderr log file if needed
        if( mProcData->getStdErr() == "file" )
        {
            SS_StreamReaderFactory::instance()->createSpaceForLogFile( logName + "_" + convert<std::string>(mProcData->getSysInstance()) + ".err" );
        }


        mChildPID = fork();
        int err = 0;
        int numErr = 0;
        int wret = 0;
        switch( mChildPID )
        {
            case -1: // fork error
                RPMS_FATAL(DMN_PRC_LOG, "Failed to fork for process execution");
                return -1;
                break;

            case 0: // child
                err = execProcess( commandLine );
                break;

            default: // parent
                // manage the reading of the pipes from the executed process
                readProcPipes( logName );
                // waitpid( pid, status, options )
                wret = waitpid( mChildPID, &ret, 0);
                mChildPID = 0; /// reset to 0 so we can see if the process is dead
                numErr = errno;
                mStdOutReader->stop();
                mStdErrReader->stop();
                close( mStdOut[0] );
                close( mStdErr[0] );
                mStdErrReader->join();
                mStdOutReader->join();
                break;
        }

        // if the errno has been set from wait pid then we must do something with it
        if( wret != 0 )
        {
            char errBuff[SS_Ini::instance()->getInt("daemon", "daemon.error.buff")];
            char * errTxt = strerror_r(numErr, errBuff, sizeof(errBuff));
            std::string exc = "Failure in waitpid.  Error [" + convert<std::string>(numErr) + "]: " + errTxt;
            RPMS_WARN( DMN_PRC_LOG, exc );
        }
                

        return ret;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::readProcPipes( const std::string &aLogName )
{
    TRY(SC_ProcessWrapperThread::readProcPieps( const std::string &aLogName ));
        close(mStdIn[0]);
        close(mStdIn[1]);

        close(mStdOut[1] ); // close write end of stdout
        close(mStdErr[1]); // close write end of stderr
        mStdOutReader = SS_StreamReaderFactory::instance()->getReader( 
                mStdOut[0], 
                mProcData->getStdOut(), 
                SS_StreamReaderFactory::_STDOUT,
                aLogName,
                convert<std::string>(mProcData->getSysInstance()));
        mStdErrReader = SS_StreamReaderFactory::instance()->getReader( 
                mStdErr[0], 
                mProcData->getStdErr(), 
                SS_StreamReaderFactory::_STDERR,
                aLogName,
                convert<std::string>(mProcData->getSysInstance()));

        // now we need to set up the process monitor
        std::string procName = mProcData->getCompName();
        mProcInfo = new SC_ProcessInfo(mChildPID);
        while( (mProcInfo->getProcName().substr(0, 10) != procName.substr(0,10)) || !mProcInfo->isLatestValid() )
        {
            if( !mProcInfo->isLatestValid() )
            {
                // the proc is no longer running
                this->stopDead();
                return;
            }
            RPMS_TRACE( DMN_PRC_LOG, "Reloading process stats [" + mProcInfo->getProcName().substr(0, 10) + "] vrs [" + procName.substr(0,10) + "]");
            mProcInfo->load(true);
            // this will allow the other processes to continue
            sched_yield();
        }
        // this is required in case the stop was set after it had got passed the process route 
        if( mExternalStop )
        {
            stop();
        }
        else
        {
            // start the reader threads
            mStdOutReader->start();
            mStdErrReader->start();
        }
    CATCH;
}

///////////////////////////////////////////
int SC_ProcessWrapperThread::execProcess( const std::string &aCmdLine )
{
    TRY(SC_ProcessWrapperThread::execProcess( const std::string &aCmdLine ));
        // firstly we need to create a char *argv[], so we parse the command line 
        //      to get all the args that we want to execute
        std::queue<std::string> args;
        std::string copy = aCmdLine;
        std::string::iterator iter = copy.begin();
        int start = 0;
        int end = 0;
        int count = 0;
        while( iter != copy.end() )
        {
            if( *iter == ' ' )
            {
                end = count - start;
                args.push( copy.substr(start, end) );
                start = count+1;
            }
            ++iter;
            ++count;
        }
        // remember to get the last arg
        end = count - start;
        args.push( copy.substr(start, end) );

        int sz = args.size(); // we need this to be kept for later
        char *cmdArgs[sz+1];
        count = 0;
        while( ! args.empty() )
        {
            cmdArgs[count] = const_cast<char*>(args.front().c_str());
            args.pop();
            ++count;
        }
        cmdArgs[sz] = '\0';

        // now to sort out the plumbing .. pun!
        // FIRSTLY deal with the childs pipes
        // close stdin pipes after we have taken control
        close(STDIN_FILENO);
        dup(mStdIn[0]);
        close(mStdIn[0]);
	    close(mStdIn[1]);

        // close stdout pipes after we have taken control
        close(STDOUT_FILENO);
        dup(mStdOut[1]);
        close(mStdOut[0]);
        close(mStdOut[1]);

        // close stderr pipes after we have taken control
        close(STDERR_FILENO);
        dup(mStdErr[1]);
        close(mStdErr[0]);
        close(mStdErr[1]);

        int ret;
        std::cout << "Cmd line [" << aCmdLine.c_str() << "] executed from RPMS" << std::endl;
        // execvp takes the following args:
        //      const char *file --> the file to execute
        //      char *const argv[] --> an arg list (first item must be the executable)
        //  if we have a argv[] we can execute with execvp( *argv, argv );
        // this will only return if there is an execution error
        ret = ::execvp( *cmdArgs, cmdArgs );
        // this is so we have something to report on in the logging
        std::cerr << "**** Failed to execute process [" << *cmdArgs << "] with execvp from RPMS ****" << std::endl;
        _exit(EXIT_FAILURE);
        return ret;
    CATCH;
}

///////////////////////////////////////////
void SC_ProcessWrapperThread::getEnvironmentVarArray()
{
    //TRY(SC_ProcessWrapperThread::getEnvironmentVarArray());
        // we set 5 hardcoded variables for use in the system
        //      RPMS_LOG_DIR
        //      RPMS_HOST_NAME
        //      RPMS_APP_NAME
        //      RPMS_APP_INSTANCE
        //      RPMS_DATE_STAMP
        // These can be used by the logging mechanism to manage the creation of any 
        //      application generated logfiles, such as is the case for log4cplus enabled
        //      applications
        
    //CATCH;
}

///////////////////////////////////////////
bool SC_ProcessWrapperThread::evaluateProcessReturn( const int aRetVal )
{
    TRY(SC_ProcessWrapperThread::evaluateProcessReturn( const int aRetVal ));
        bool ret = false;
        switch( aRetVal )
        {
            case 0:
                ret = true;
                RPMS_DEBUG( DMN_PRC_LOG, "Process terminated normally [0]" );
                break;
            case -1:
                RPMS_ERROR( DMN_PRC_LOG, "Process terminated abnormally" );
                ret = false;
                break;
            case 6:
                RPMS_ERROR( DMN_PRC_LOG, "**** Process terminated with abort [" + mProcData->getInternalName() + "] ****" );
                ret = false;
                break;
            case 15:
                RPMS_DEBUG( DMN_PRC_LOG, "Process terminated after user interaction [15] (SIGTERM)" );
                ret = true;
                break;
            default:
                RPMS_WARN( DMN_PRC_LOG, "Process return value [" + convert<std::string>(aRetVal) 
                        + "], allowing application to manage it" );
                ret = true;
        }
        return ret;
    CATCH;
}

///////////////////////////////////////////
bool SC_ProcessWrapperThread::checkFailureRate()
{
    TRY(checkFailureRate());
        // FIRSTLY we have to remove all of the previous restarts that are too old
        long now = SC_DateTime::getCurrentTimeInSecs();
        long timeout = mProcData->getRetryWindow();
        size_t num = mRetryTimes->size(); // take a static count
        long diff = 0;

        std::deque<long>::iterator iter = mRetryTimes->begin();
        while( iter != mRetryTimes->end() )
        {
            diff = now - *iter;
            if( diff > timeout )
            {
                RPMS_DEBUG(DMN_PRC_LOG, "Retry instance invalid as beyond retry window");
                mRetryTimes->erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        // SECONLY we need to calculate how may are left over and compare to the maximum
        //      restart rate specified in the config xml
        size_t retries = mRetryTimes->size(); // need a fresh count as it may have changed above
        if( retries > mProcData->getMaxRetries() )
        {
            return false;
        }
        return true;
    CATCH;
}

///////////////////////////////////////////
SC_ProcessWrapperThread::SC_ProcessWrapperThread( const SC_ProcessWrapperThread &aRhs )
    : SA_ThreadBase("dummy"), mLock("dummy")
{
    TRY(SC_ProcessWrapperThread::SC_ProcessWrapperThread( const SC_ProcessWrapperThread &aRhs ));
        throw SX_Process("Not allowed to copy a thread class");
    CATCH;
}

///////////////////////////////////////////
SC_ProcessWrapperThread& SC_ProcessWrapperThread::operator=( const SC_ProcessWrapperThread &aRhs )
{
    TRY(SC_ProcessWrapperThread::operator=( const SC_ProcessWrapperThread &aRhs ));
        throw SX_Process("Not allowed to assign a thread class");
        return *this;
    CATCH;
}

///////////////////////////////////////////
