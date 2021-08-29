
#include "ProcessInfo.h"

#include "Console/Print.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <tchar.h>

namespace Windows 
{
    namespace Process 
    {
        bool ListProcessThreads(unsigned long dwOwnderPID)
        {
            HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
            THREADENTRY32 te32;

            // Take a snapshot of all running threads.
            hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
            if (hThreadSnap == INVALID_HANDLE_VALUE)
            {
                return false;
            }

            // Fill in the size of the structure before using it.
            te32.dwSize = sizeof(THREADENTRY32);

            // Retrieve the information about the first thread,
            // and exit if unsuccessful
            if (!Thread32First(hThreadSnap, &te32))
            {
                Console::PrintError("Thread32First"); // Show cause of failure.
                CloseHandle(hThreadSnap); // Clean up snapshot object.
                return false;
            }

            // Now walk the thread list of the system,
            // and display information about each thread
            // associated with the specified process.
            do
            {
                if (te32.th32OwnerProcessID == dwOwnderPID)
                {
                    HANDLE threadHandle = OpenThread(SPECIFIC_RIGHTS_ALL, FALSE, te32.th32ThreadID); // With great power comes great responsibility...
                    if (threadHandle != INVALID_HANDLE_VALUE)
                    {
                        // _tprintf(TEXT("\nTHREAD ID = 0x%08X"), te32.th32ThreadID);
                        _tprintf(TEXT("\nTHREAD ID = %d"), te32.th32ThreadID);

                        FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime;
                        if (GetThreadTimes(threadHandle, &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTime))
                        {
                            SYSTEMTIME sysCreatedTime;
                            if (FileTimeToSystemTime(&lpCreationTime, &sysCreatedTime))
                            {
                                _tprintf(TEXT("\nCreation time = %u:%u:%u:%u"), sysCreatedTime.wHour, sysCreatedTime.wMinute, sysCreatedTime.wSecond, sysCreatedTime.wMilliseconds);
                            }
                            else
                            {
                                Console::PrintError("Performing file time conversion");
                            }
                        }
                        else
                        {
                            Console::PrintError("Acquiring thread times");
                        }

                        _tprintf(TEXT("\nbase priority = %d"), te32.tpBasePri);
                        _tprintf(TEXT("\ndelta priority = %d"), te32.tpDeltaPri);
                    }
                }
            } while (Thread32Next(hThreadSnap, &te32));

            _tprintf(TEXT("\n\n"));

            // Don't forget to clean up the snapshot object.
            CloseHandle(hThreadSnap);
            return true;
        }

        unsigned long FindProcessId(const char* procName)
        {
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);

            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

            if (Process32First(snapshot, &entry) == TRUE)
            {
                while (Process32Next(snapshot, &entry) == TRUE)
                {
                    if (_stricmp(entry.szExeFile, procName) == 0)
                    {
                        DWORD pid = entry.th32ProcessID;
                        CloseHandle(snapshot);

                        return pid;
                    }
                }
            }

            CloseHandle(snapshot);
            return -1;
        }

        int IsProcessRunning(unsigned long pid)
        {
            HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
            DWORD ret = WaitForSingleObject(process, 0);
            CloseHandle(process);
            return ret == WAIT_TIMEOUT;
        }
    }
}