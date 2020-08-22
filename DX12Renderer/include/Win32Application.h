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
            static int run(DXWindow* pSample, HINSTANCE hInstance, int nCmdShow);
            static HWND getHwnd() { return _hwnd; }

        protected:
            static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        private:
            static HWND _hwnd;
        };

    }
}

#endif