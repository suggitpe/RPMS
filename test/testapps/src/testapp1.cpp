#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <getopt.h>
#include <signal.h>
#include <unistd.h>

////////////////////////////////////////////////
static int run = 1;
////////////////////////////////////////////////
static struct option long_opts[] = 
{
    {"play",            0, 0, 'p' },  // start the system
    {"test",            0, 0, 't' },  // stop the system
    {0,0,0,0}
};
////////////////////////////////////////////////
void handler( int sig_num )
{
    cout << "~~~~~~~~ caught signal [" << sig_num << "] ~~~~~~~~~" << endl;
    run = 0;
}
////////////////////////////////////////////////
void usage()
{
    cerr << "Invalid usage:" << endl;
    for(int i = 0; long_opts[i].name; ++i )
    {
        cerr << "\t --" << long_opts[i].name << endl;
    }
    exit(1);
}
////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    cout << "####################################" << endl;
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    for( int i = 0; i < argc; ++i )
    {
        cout << "arg " << i << " [" << argv[i] << "]\n";
    }
    int c;
    int opts = 0;
    bool play = false;
    while( 1 )
    {
        int opt_ind = 0;
        c = getopt_long_only(argc, argv, "", long_opts, &opt_ind );
        if( -1 == c )
        {
            break;
        }
        switch( c )
        {
            case 'p':
                ++opts;
                play = true;
                break;
            case 't':
                ++opts;
                break;
            default:
                usage();
                break;
        }
    }
    if( opts != 1 )
    {
        usage();
    }
    // ****************************************
    if(play)
    {
        cout << "play" << endl;
        while(run)
        {
            cout << "*************" << endl;
            sleep(1);
        }
    }
    else
    {
        cout << "test" << endl;
    }
    cout << "####################################" << endl;
    
    return 0;
}
////////////////////////////////////////////////
