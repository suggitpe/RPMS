#ifndef __sc_systeminfo_hpp
#define __sc_systeminfo_hpp

#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <string>

namespace rpms
{

    /**
     * This class is a collection of static methods and accessors to the underlying system info to get system informtion
     *  
     * @author Peter Suggitt (2005)
     * @note <b>Utility class</b>
     * */
    class SC_SystemInfo
    {
        public:
            /** Main constructor - goes to the underlying system and retrieves varios structs */
            SC_SystemInfo();
            /** Virtual destructor */
            virtual ~SC_SystemInfo();
            /** Static method to get the CPU speed of the machine */
            static float getCpuClockSpeed();
        protected:
        private: // methods
        private: //members
            struct sysinfo mSysInfo;
            struct utsname mUName;

        public: // uname accessors
            const char * getSysName()             const { return mUName.sysname; }
            const char * getNodeName()            const { return mUName.nodename; }
            const char * getRelease()             const { return mUName.release; }
            const char * getVersion()             const { return mUName.version; }
            const char * getMachine()             const { return mUName.machine; }
            const char * getDomainName()          const { return mUName.domainname; }

        public: // sysinfo accessors
            unsigned long getLoad0()        const { return mSysInfo.loads[0];}
            unsigned long getLoad1()        const { return mSysInfo.loads[1];}
            unsigned long getLoad2()        const { return mSysInfo.loads[2];}
            unsigned long getTotalRAM()     const { return mSysInfo.totalram;}
            unsigned long getFreeRAM()      const { return mSysInfo.freeram;}
            unsigned long getSharedRAM()    const { return mSysInfo.sharedram;}
            unsigned long getBufferedRAM()  const { return mSysInfo.bufferram;}
            unsigned long getTotalSwap()    const { return mSysInfo.totalswap;}
            unsigned long getFreeSwap()     const { return mSysInfo.freeswap;}
            unsigned short getNumProcs()    const { return mSysInfo.procs;}
            unsigned short getPad()         const { return mSysInfo.pad;}
            unsigned long getTotalHigh()    const { return mSysInfo.totalhigh;}
            unsigned long getFreeHigh()     const { return mSysInfo.freehigh;}
            unsigned int getMemUnit()       const { return mSysInfo.mem_unit;}
            
    };


}// namespace

#endif
