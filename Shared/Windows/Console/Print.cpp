
#include <Windows.h>

#include <stdio.h>
#include <tchar.h>

namespace Windows
{
    namespace Console
    {
        void PrintError(const char* msg)
        {
            DWORD eNum;
            TCHAR sysMsg[256];
            TCHAR* p;

            eNum = GetLastError();
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, eNum,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                sysMsg, 256, NULL);

            // Trim the end of the line and terminate it with a null
            p = sysMsg;
            while ((*p > 31) || (*p == 9))
                ++p;
            do { *p-- = 0; } while ((p >= sysMsg) &&
                ((*p == '.') || (*p < 33)));

            // Display the message
            _tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
        }

        void PrintExecutedPath()
        {
            char execProcessName[MAX_PATH + 1];
            execProcessName[MAX_PATH] = '\0';
            GetModuleFileName(NULL, execProcessName, MAX_PATH);

            _tprintf(TEXT("\nLoaded from: %s"), execProcessName);
        }
    }
}