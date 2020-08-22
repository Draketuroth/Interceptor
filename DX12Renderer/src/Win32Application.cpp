
#include "stdafx.h"
#include "Win32Application.h"

namespace VE 
{
    namespace Platform 
    {
        HWND Win32Application::_hwnd = nullptr;
    }
}

namespace VE 
{
    namespace Platform
    {
        int Win32Application::run(DXWindow* pSample, HINSTANCE hInstance, int nCmdShow)
        {
            // Parse the command line parameters.
            int argc;
            LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
            pSample->parseCommandLineArgs(argv, argc);
            LocalFree(argv);

            // Initialize the window class.
            WNDCLASSEX windowClass = { 0 };
            windowClass.cbSize = sizeof(WNDCLASSEX);
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            windowClass.lpfnWndProc = WindowProc;
            windowClass.hInstance = hInstance;
            windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
            windowClass.lpszClassName = L"DirectX12 Renderer";
            RegisterClassEx(&windowClass);

            RECT windowRect = { 0, 0, static_cast<LONG>(pSample->getWidth()), static_cast<LONG>(pSample->getHeight()) };
            AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

            // Crate the window and store a handle to it.
            _hwnd = CreateWindow(
                windowClass.lpszClassName,
                pSample->getTitle().c_str(),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                windowRect.right - windowRect.left,
                windowRect.bottom - windowRect.top,
                nullptr, // no parent window
                nullptr, // no menus
                hInstance,
                pSample);

            // Initialize the window. OnInit is defined in each child-implementation of DXWindow.
            pSample->onInit();

            ShowWindow(_hwnd, nCmdShow);

            // Main sample loop.
            MSG msg = {};
            while (msg.message != WM_QUIT) 
            {
                // Process any messages in the queue.
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            pSample->onDestroy();

            // Return this part of the WM_QUIT message to Windows.
            return static_cast<char>(msg.wParam);
        }
        LRESULT Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            DXWindow* pSample = reinterpret_cast<DXWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            switch (message) 
            {
            case WM_CREATE:
            {
                // Save the DXWindow passed in to CreateWindow.
                LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            }
            return 0;

            case WM_KEYDOWN:
                if (pSample) 
                {
                    pSample->onKeyDown(static_cast<uint8_t>(wParam));
                }
                return 0;
            
            case WM_KEYUP:
                if (pSample) 
                {
                    pSample->onKeyUp(static_cast<uint8_t>(wParam));
                }
                return 0;
           
            case WM_PAINT:
                if (pSample) 
                {
                    pSample->onUpdate();
                    pSample->onRender();
                }
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            }
            
            // Handle any message the switch statement didn't.
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
}