// Based on Ben Watson's CPU measurement method

#ifndef CPU_SAMPLER_H
#define CPU_SAMPLER_H

#include <Windows.h>

namespace VE 
{
    namespace Core 
    {
        namespace Util
        {
            class CpuSampler
            {
            public:
                CpuSampler(void);

                bool GetProcessUsage(double &cpuOutput);
                void SetProcessHandle(HANDLE handle);
                void SetCPUSamplingFrequency(unsigned int ms);
            private:
                ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
                inline bool isFirstRun() const { return (_previousTimeStamp == 0); }

                // System total times.
                FILETIME _ftPrevSysKernel;
                FILETIME _ftPrevSysUser;

                // Process times.
                FILETIME _ftPrevProcKernel;
                FILETIME _ftPrevProcUser;

                // Process handle.
                HANDLE _processHandle;

                // double _cpuUsage;
                unsigned int  _minElapsedMS;
                double _previousTimeStamp;

                volatile LONG _runCounter;
            };
        }
    }
}

#endif