#ifndef WINDOWS_PROCESS_INFO
#define WINDOWS_PROCESS_INFO

namespace Windows 
{
    namespace Process 
    {
        bool ListProcessThreads(unsigned long dwOwnderPID);

        unsigned long FindProcessId(const char* procName);

        int IsProcessRunning(unsigned long pid);
    }
}

#endif