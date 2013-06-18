//#############################

#include <deque>
#include <pthread.h>
#include <iostream>
#include <base/sa_baseobject.hpp>
#include <base/sa_threadbase.hpp>
#include <base/st_spointer.hpp>
#include <utilities/st_conversion.hpp>

#include <utilities/ss_ini.hpp>
#include <logging/si_logging.hpp>

#define LOGNAME "testharness"
#define THREAD "thread"
#define MAX 7

void executeAll();

void executeOne();
///////////////////////////////////////////////////////////
class Thread : public SA_ThreadBase
{
    public:
        static ST_SPointer<Thread> create(const std::string &aName );
        void stop() { mRun = false;}
        virtual ~Thread();
        Thread( const std::string &aName);
    protected:
        void run();
    private:
         bool mRun;
};

////////////////////////////
Thread::Thread(const std::string &aName)
    : SA_ThreadBase(aName)
{
    mRun = true;
}

////////////////////////////
Thread::~Thread()
{
}

////////////////////////////
ST_SPointer<Thread> Thread::create(const std::string &aName )
{
    return new Thread(aName);
}


////////////////////////////
void Thread::run()
{
    while (mRun)
    {
        RPMS_TRACE(THREAD, "Doing something");
        sleep(1);
        RPMS_TRACE(THREAD, "Done  something");
    }
}

///////////////////////////////////////////////////////////
int main()
{
    std::cout << "------------------------ START ---------------------" << std::endl;
    SS_Ini::instance()->initialise("/home/suggitpe/test/RPMS/config/rpms.ini");
    //executeOne();
    executeAll();
    sleep (1);
    std::cout << "------------------------- END ----------------------" << std::endl;
}

//////////////////////////////////////////////////
void executeOne()
{
    {
        ST_SPointer<Thread> t1 = new Thread("Test Thread");
        t1->start();
        t1->join();
    }
}

//////////////////////////////////////////////////
void executeAll()
{
    RPMS_DEBUG(LOGNAME, "Thread size = [" + rpms::convert<std::string>(sizeof(Thread)) + "]");
    //sleep(2);
    std::cout << "----------------------------------------------------" << std::endl;
    std::deque<ST_SPointer<Thread> > q;
    for(int i = 0; i < MAX; ++i)
    {
        RPMS_INFO(LOGNAME, "Creating thread [" + rpms::convert<std::string>(i) + "]");
        q.push_back( new Thread("TestThread_" + rpms::convert<std::string>(i) ) );
    }

    //sleep(3);

    
    std::cout << "----------------------------------------------------" << std::endl;
    for(int j = 0; j < MAX; ++j)
    {
        RPMS_INFO(LOGNAME, "Starting thread [" + rpms::convert<std::string>(j) + "]");
        q.at(j)->start();
        sleep(1);
    }
    //sleep(1);
    
    std::cout << "----------------------------------------------------" << std::endl;
    for(int j = 0; j < MAX; ++j)
    {
        RPMS_INFO(LOGNAME, "Stopping thread [" + rpms::convert<std::string>(j) + "]");
        q.at(j)->stop();
        //sleep(1);
    }

    std::cout << "----------------------------------------------------" << std::endl;
    bool running = true;
    while( q.size() > 0 )
    {
        for( int i = 0; i < q.size(); ++i )
        {
            if( ! q.at(i)->isRunning() )
            {
                RPMS_DEBUG( LOGNAME, "...erase");
                std::deque<ST_SPointer<Thread> >::iterator x = q.begin();
                x += i;
                q.erase(x);
                continue;
            }
        }
        RPMS_DEBUG( LOGNAME, "size [" + rpms::convert<std::string>(q.size()) + "]");
        sleep(1);
    }
    q.clear();
}

