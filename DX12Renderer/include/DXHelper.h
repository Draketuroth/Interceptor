#ifndef VE_DXHELPER_H
#define VE_DXHELPER_H

#include <stdexcept>
#include <string>
#include <winerror.h>
#include <stdio.h>
#include <wchar.h>

namespace VE 
{
    inline std::string HrToString(HRESULT hr) 
    {
        char buf[64] = {};
        sprintf_s(buf, "HRESULT of 0x%08X", static_cast<unsigned int>(hr));
        return std::string(buf);
    }

    class HrException : public std::runtime_error 
    {
    public:
        HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), _hr(hr) {}
        HRESULT error() const { return _hr; }
    private:
        const HRESULT _hr;
    };

    #define SAFE_RELEASE(x) if (x) (x)->Release()

    inline void ThrowIfFailed(HRESULT hr) 
    {
        if (FAILED(hr)) 
        {
            throw HrException(hr);
        }
    }

    inline void GetAssetsPath(_Out_writes_(pathSize) wchar_t* path, unsigned int pathSize)
    {
        if (path == nullptr) 
        {
            throw std::exception();
        }

        DWORD size = GetModuleFileName(nullptr, path, pathSize);
        if (size == 0 || size == pathSize) 
        {
            throw std::exception();
        }

        wchar_t* lastSlash = wcsrchr(path, L'\\');
        if (lastSlash) 
        {
            *(lastSlash + 1) = L'\0';
        }
    }
}


#endif