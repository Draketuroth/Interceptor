
#include "CpuSampler.h"

namespace VE 
{
    namespace Core 
    {
        namespace Util 
        {
            CpuSampler::CpuSampler(void) :
            _cpuUsage(-1),
            _dwLastRun(0),
            _lRunCount(0)
            {
                ZeroMemory(&_ftPrevSysKernel, sizeof(FILETIME));
                ZeroMemory(&_ftPrevSysUser, sizeof(FILETIME));

                ZeroMemory(&_ftPrevProcKernel, sizeof(FILETIME));
                ZeroMemory(&_ftPrevProcUser, sizeof(FILETIME));
            }

            short CpuSampler::GetProcessUsage()
            {
                // Create a copy to protect against race conditions in setting
                // the member variable.
                short cpuCopy = _cpuUsage;

                if (::InterlockedIncrement(&_lRunCount) == 1) 
                {
                    if (!EnoughTimePassed()) 
                    {
                        ::InterlockedDecrement(&_lRunCount);
                        return cpuCopy;
                    }

                    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
                    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

                    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
                        !GetProcessTimes(_processHandle, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser))
                    {
                        ::InterlockedDecrement(&_lRunCount);
                        return cpuCopy;
                    }

                    if (!isFirstRun()) 
                    {
                        // Cpu usage is calculated by getting the total amount
                        // of time the system has operated since the last
                        // measurement (made up of kernel + user) and the total
                        // amount of time the process has run (kernel + user).

                        ULONGLONG ftSysKernelDiff = SubtractTimes(ftSysKernel, _ftPrevSysKernel);
                        ULONGLONG ftSysUserDiff = SubtractTimes(ftSysUser, _ftPrevSysUser);

                        ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, _ftPrevProcKernel);
                        ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, _ftPrevProcUser);

                        ULONGLONG totalSys = ftSysKernelDiff + ftSysUserDiff;
                        ULONGLONG totalProc = ftProcKernelDiff + ftProcUserDiff;

                        if (totalSys > 0) 
                        {
                            _cpuUsage = static_cast<short>((100.0 * totalProc) / totalSys);
                        }
                    }

                    _ftPrevSysKernel = ftSysKernel;
                    _ftPrevSysUser = ftSysUser;
                    _ftPrevProcKernel = ftProcKernel;
                    _ftPrevProcUser = ftProcUser;

                    _dwLastRun = GetTickCount64();

                    cpuCopy = _cpuUsage;
                }

                ::InterlockedDecrement(&_lRunCount);

                return cpuCopy;
            }

            void CpuSampler::SetProcessHandle(HANDLE handle)
            {
                _processHandle = handle;
            }

            ULONGLONG CpuSampler::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB)
            {
                LARGE_INTEGER a, b;
                a.LowPart = ftA.dwLowDateTime;
                a.HighPart = ftA.dwHighDateTime;

                b.LowPart = ftB.dwLowDateTime;
                b.HighPart = ftB.dwHighDateTime;

                return a.QuadPart - b.QuadPart;
            }

            bool CpuSampler::EnoughTimePassed()
            {
                const int minElapsedMS = 250;

                ULONGLONG dwCurrentTickCount = GetTickCount64();
                return (dwCurrentTickCount - _dwLastRun) > minElapsedMS;
            }
        }
    }
}