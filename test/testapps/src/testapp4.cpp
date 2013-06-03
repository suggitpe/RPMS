#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

int main( int argc, char *argv[] )
{
    for( int i = 0; i < argc; ++i )
    {
        cout << "arg " << i << " [" << argv[i] << "]\n";
    }
    
    return 0;
}
