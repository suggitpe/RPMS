#include <base/sa_threadbase.hpp>
#include <utilities/ss_ini.hpp>

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// pre decl
void showPids();
void showSystemCall();
void simpleFork();
void simpleForkAndExec();
void testEnv();
void sigPlay();
void makeZombie();
void pipeTest();
void streamTest();
void writer( const char *aMsg, int count, FILE *aStrm );
void reader( FILE *aStrm );
pid_t spawn( char* program, char** arg_list );

using std::cout;
using std::cerr;
using std::endl;

using rpms::SA_ThreadBase;


///// gloabls
sig_atomic_t sigterm_count = 0;

////////////////////////////////////////////// 
void handler( int sig_num )
{
    cout << "sigint" << endl;
    ++sigterm_count;
}

////////////////////////////////////////////// 
class TestThread : public SA_ThreadBase
{//{{{
    public:
        /////////////////////////////////////////////
        TestThread()
            : SA_ThreadBase("testThread"), mRun(true)
        {
            cout << "testthread ctor" << endl;
        }

        ~TestThread()
        {
            cout << "testthread dtor" << endl;
        }

    protected:
        /////////////////////////////////////////////
        virtual void run()
        {
            cout << "in run" << endl;
            while(mRun == true)
            {
                cout << "...." << endl;
                sleep(1);
            }
            cout << "out of run" << endl;
        }

        /////////////////////////////////////////////
        virtual void stop()
        {
            mRun = false;
        }

    private:
        /////////////////////////////////////////////
        int mPid;
        bool mRun;
};//}}}

////////////////////////////////////////////// 
int main()
{{{
    cout << "**************************************************" << endl;
    //testEnv();
    //cout << "**************************************************" << endl;
    //showPids();
    //cout << "**************************************************" << endl;
    //showSystemCall();
    //cout << "**************************************************" << endl;
    //simpleFork();
    //cout << "**************************************************" << endl;
    //simpleForkAndExec();
    //cout << "**************************************************" << endl;
    //sigPlay();
    //cout << "**************************************************" << endl;
    //makeZombie();
    //cout << "**************************************************" << endl;
    //pipeTest();
    //cout << "**************************************************" << endl;
    streamTest();
    cout << "**************************************************" << endl;
    /*
    SS_Ini::instance()->initialise("/home/suggitpe/test/RPMS/config/rpms.ini");
    TestThread *t = new TestThread;
    cout << "Starting thread" << endl;
    t->start();
    cout << "Joining thread" << endl;
    t->join();
    delete t;
    */
    return 0;
}}}

////////////////////////////////////////////// 
void sigPlay()
{
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);

    for( int i = 0; i < 8; ++i)
    {
        sleep(1);
    }

    cout << "SIGINT was called " << sigterm_count << " times" << endl;
}

////////////////////////////////////////////// 
void streamTest()
{{{
    int fds[2];
    pid_t child;
    pipe(fds);

    child = fork();

    if( child != (pid_t)0 )
    {
        // parent
        FILE *stream;
        close(fds[0]);
        stream = fdopen(fds[1], "w"); // open stream for write
        fprintf( stream, "This is a test.\n");
        fprintf( stream, "Hello, world.\n");
        fprintf( stream, "My dog has fleas.\n");
        fprintf( stream, "This program sucks!\n");
        fprintf( stream, "One fish, two fish\n");
        fflush(stream);
        close(fds[1]);
        waitpid(child, NULL, 0);
    }
    else
    {
        // child
        close(fds[1]);
        dup2( fds[0], STDIN_FILENO ); // connect read end to stdin
        execlp( "sort", "sort", 0 ); // replace child with sort
    }
}}}

////////////////////////////////////////////// 
void pipeTest()
{{{
    int fds[2];
    pid_t child;
    pipe(fds);

    // new process
    child = fork();

    if( child != (pid_t)0 )
    {
        // parent
        FILE *stream;
        close(fds[0]); // close read end
        stream = fdopen( fds[1], "w");
        writer("Hello world.", 5, stream);
        close(fds[1]);
    }
    else
    {
        // child
        FILE *stream;
        close(fds[1]); // close write end
        stream = fdopen( fds[0], "r" ); // open stream for read
        reader(stream);
        close(fds[0]); // clean up
    }
}}}

////////////////////////////////////////////// 
void writer( const char *aMsg, int count, FILE *aStrm )
{{{
    for( ; count > 0; --count )
    {
        // write and flush immediately
        fprintf( aStrm, "%s\n", aMsg );
        fflush(aStrm);
        sleep(1);
    }
}}}

////////////////////////////////////////////// 
void reader( FILE *aStrm )
{{{
    char buff[1024];
    while( !feof(aStrm) && !ferror(aStrm) && (fgets( buff, sizeof(buff), aStrm) != NULL) )
    {
        fputs( buff, stdout );
    }
}}}

////////////////////////////////////////////// 
void simpleForkAndExec()
{{{
    // fisrt of all you have to create the pipes
    cout << "Creating pipes" << endl;
    // create the pipes
    int stdin_pipe[2];
    int stdout_pipe[2];
    int stderr_pipe[2];

    pipe(stdin_pipe);
    pipe(stdout_pipe);
    pipe(stderr_pipe);
        
    int child_status;
    cout << "I am " << getpid() << endl;
    char * arg[] = {"ls", "-l", "/", NULL};
    
    pid_t x = spawn("ls", arg);
    cout << "done and created " << x << endl;
    
    waitpid( x, &child_status, 0 );
    if( WIFEXITED(child_status) )
    {
        cout << "Normal exit from child - exit code [" << WEXITSTATUS(child_status) << "]" << endl;
    }
    else
    {
        cerr << "Process exited abnormally" << endl;
    }
}}} 

////////////////////////////////////////////// 
pid_t spawn( char* program, char** arg_list )
{{{
    pid_t child_pid;
    child_pid = fork();

    if( child_pid != 0 )
    {
        return child_pid;
    }
    else
    {
        cout << "calling execvp" << endl;
        execvp(program, arg_list);
        cerr << "error occured in execvp" << endl;
        abort();
    }
}}}

////////////////////////////////////////////// 
void simpleFork()
{{{
    pid_t child_pid;
    cout << "Main process id is " << getpid() << endl;
    child_pid = fork();
    if( child_pid != 0 )
    {
        cout << "This is the parent process with id " << getpid() << " and parent of " 
            << getppid() << endl;
        cout << "The child process id is " << child_pid << endl;
    }
    else
    {
        cout << "this is the child process with id " << getpid() << " and parent of " 
            << getppid() << endl;
    }
    sleep(1);
    
}}}

////////////////////////////////////////////// 
void showSystemCall()
{{{
    unsigned int ret = system("ls -l /home/suggitpe");
    cout << "----------" << endl;
    cout << "System call return is " << ret << endl;
}}}

////////////////////////////////////////////// 
void showPids()
{{{
    cout << "The process id is " << getpid() << endl;
    cout << "The parent process id is " << getppid() << endl;
}}}

////////////////////////////////////////////// 
void testEnv()
{{{
    cout << "LD_LIBRARY_PATH [" << getenv("LD_LIBRARY_PATH") << "]" << endl;
    putenv("PETETEST=hello");
    cout << "PETETEST [" << getenv("PETETEST") << "]" << endl;
    setenv("PETETEST", "hello again", false);
    cout << "PETETEST [" << getenv("PETETEST") << "]" << endl;
    setenv("PETETEST", "hello again again", true);
    cout << "PETETEST [" << getenv("PETETEST") << "]" << endl;
    unsetenv("PETETEST");
}}}

////////////////////////////////////////////// 
void makeZombie()
{{{
    pid_t child_pid;
    child_pid = fork();
    if( child_pid > 0 ) 
    {
        sleep(60);
    }
    else
    {
        exit(0);
    }
}}}
