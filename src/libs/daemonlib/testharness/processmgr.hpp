#ifndef __processmgr_hpp_test
#define __processmgr_hpp_test

#include <string>

class ProcessMgr
{
    public:
        ProcessMgr( const std::string &aCmd );
        ~ProcessMgr();
        int executeProcess();
    protected:
    private:
        pid_t spawn();
        std::string mCmd;
};

#endif
