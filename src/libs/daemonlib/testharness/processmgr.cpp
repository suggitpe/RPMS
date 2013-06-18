#include "processmgr.hpp"
#include <sys/wait.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

///////////////////////////////////////////////
ProcessMgr::ProcessMgr( const std::string &aCmd )
    : mCmd(aCmd)
{
    //cout << "ProcessMgr ctor" << endl;
}

///////////////////////////////////////////////
ProcessMgr::~ProcessMgr()
{
    //cout << "ProcessMgr dtor" << endl;
}

///////////////////////////////////////////////
int ProcessMgr::executeProcess()
{
    int procReturn;
    cout << "Executing process" << endl;
    pid_t x = spawn();
    cout << "New process started" << endl;
    waitpid( x, &procReturn, 0 );
    if( WIFEXITED(procReturn) )
    {
        cerr << "Good end to proc" << endl;
    }
    else
    {
        cerr << "Bad end to proc" << endl;
    }
    return procReturn;
}

///////////////////////////////////////////////
pid_t ProcessMgr::spawn()
{
    int stdin_pipe[2];
    int stdout_pipe[2];
    int stderr_pipe[2];

    if( (pipe(stdin_pipe) == 0) && (pipe(stdout_pipe) == 0) && (pipe(stderr_pipe) == 0) )
    {
        pid_t child = fork();
        if( child != (pid_t)0 )
        {
            close(stdin_pipe[0]); // close read
			close(stdin_pipe[1]); // close write
			close(stderr_pipe[1]); // close write
            close(stdout_pipe[1]); // close write
            // we are parent
            return child;
        }
        // we are child
        cout << "Child execution of [" << mCmd << "]" << endl;
        // close stdin
        close(STDIN_FILENO);
        // attach to the recently closed filedescriptor
        dup(stdin_pipe[0]);
        // close out the old pipes
        close(stdin_pipe[0]);
	    close(stdin_pipe[1]);

        // close stdout
        close(STDOUT_FILENO);
        dup(stdout_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        // close stderr
        close(STDERR_FILENO);
        dup(stderr_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        
        execvp( mCmd.c_str(), NULL );
        cerr << "Child error occurred in execvp" << endl;
        abort();
    }
    return 0;
}

///////////////////////////////////////////////
int main()
{
    cout << "******************************************" << endl;
    ProcessMgr *proc0 = new ProcessMgr("./testscripts/testscript1.pl");
    int ret = proc0->executeProcess();
    delete proc0;
    cout << ">>>>>>> ret = " << ret << endl;
    cout << "--------------------" << endl;
    ProcessMgr *proc1 = new ProcessMgr("/home/suggitpe/codebase/RPMS/trunk/test/testscripts/testscript2.pl");
    ret = proc1->executeProcess();
    delete proc1;
    cout << ">>>>>>> ret = " << ret << endl;

    cout << "******************************************" << endl;
    return 0;
}
