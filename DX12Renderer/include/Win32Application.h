#ifndef VE_WIN32_APP_H
#define VE_WIN32_APP_H

#include "DXWindow.h"
#include <Windows.h>

namespace VE { namespace Platform { class DXWindow; } }

namespace VE 
{
    namespace Platform 
    {
        class Win32Application 
        {
        public :
            static int Run(DXWindow* pSample, HINSTANCE hInstance, int nCmdShow);
            static HWND GetHwnd() { return _hwnd; }

        protected:
            static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        private:
            static HWND _hwnd;
        };

    }
}

#endif