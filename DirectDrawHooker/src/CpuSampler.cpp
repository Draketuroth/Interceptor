
#include "CpuSampler.h"

namespace VE 
{
    namespace Core 
    {
        namespace Util 
        {
            double GetQueryPerformanceMs() {
                static LARGE_INTEGER queryFrequency;
                static BOOL supportsQueryFrequency = QueryPerformanceFrequency(&queryFrequency);

                // Check if installed hardware supports high-resolution counter.
                if (supportsQueryFrequency) {
                    LARGE_INTEGER queryCounter;
                    QueryPerformanceCounter(&queryCounter);
                    return (1000.0 * queryCounter.QuadPart) / queryFrequency.QuadPart;
                }
                else {
                #if _WIN64 
                    return static_cast<double>(GetTickCount64());
                #else
                    return static_cast<double>(GetTickCount());
                #endif
                }
            }

            CpuSampler::CpuSampler(void) :
            _minElapsedMS(100),
            _previousTimeStamp(0),
            _runCounter(0)
            {
                ZeroMemory(&_ftPrevSysKernel, sizeof(FILETIME));
                ZeroMemory(&_ftPrevSysUser, sizeof(FILETIME));

                ZeroMemory(&_ftPrevProcKernel, sizeof(FILETIME));
                ZeroMemory(&_ftPrevProcUser, sizeof(FILETIME));
            }

            bool CpuSampler::GetProcessUsage(double& cpuOutput)
            {
                // Create a copy to protect against race conditions.
                double percentage = 0.0;

                if (::InterlockedIncrement(&_runCounter) == 1) 
                {
                    double currentTimestamp = GetQueryPerformanceMs();

                    // Check if enough time has passed. 
                    if ( !((currentTimestamp - _previousTimeStamp) > _minElapsedMS) ) 
                    {
                        ::InterlockedDecrement(&_runCounter);
                        cpuOutput = -1.0;
                        return false;
                    }

                    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
                    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

                    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
                        !GetProcessTimes(_processHandle, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser))
                    {
                        ::InterlockedDecrement(&_runCounter);
                        cpuOutput = -1.0;
                        return false;
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

                        percentage = static_cast<double>((100.0 * totalProc) / totalSys);
                    }

                    _ftPrevSysKernel = ftSysKernel;
                    _ftPrevSysUser = ftSysUser;
                    _ftPrevProcKernel = ftProcKernel;
                    _ftPrevProcUser = ftProcUser;

                    _previousTimeStamp = GetQueryPerformanceMs();
                }

                ::InterlockedDecrement(&_runCounter);

                cpuOutput = percentage;

                return true;
            }

            void CpuSampler::SetProcessHandle(HANDLE handle)
            {
                _processHandle = handle;
            }

            void CpuSampler::SetCPUSamplingFrequency(unsigned int ms)
            {
                _minElapsedMS = ms;
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
        }
    }
}