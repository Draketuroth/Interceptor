
// Define INITGUID before ddraw.h
#define INITGUID

// DirectDraw includes
#include <ddraw.h>

// Standard includes
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>
#include <string>
#include <mutex>

// Detours include
#include "detours.h"

// TypeDefs

// DirectDraw Hook Definitions
IDirectDraw* lpDD = NULL;
IDirectDrawSurface* lpDDSurface = NULL;

HANDLE hMainThread;

DWORD pDirectDrawDevice = 0x539ED0;
DWORD pDirectDrawSurface = 0x539ED8;

typedef HRESULT(__stdcall* func_Blt)(LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);

func_Blt origSurfaceBlt = NULL;

void SetupConsole() 
{
    AllocConsole();
    SetConsoleTitle("DirectDraw Hooker");
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
}

HRESULT __stdcall mySurfaceBlt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
    MessageBox(NULL, "Blt", "Bleep", MB_OK);
    return origSurfaceBlt(lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}

DWORD HookVTable(PDWORD pointer, DWORD function, int index) 
{
    DWORD pFunction = (*(PDWORD*)pointer)[index];
    (*(PDWORD*)pointer)[index] = function;
    
    return pFunction;
}

void main() 
{
    SetupConsole();

    // Get path of executable file of the current process.
    char execProcessName[MAX_PATH + 1];
    execProcessName[MAX_PATH] = '\0';
    GetModuleFileName(NULL, execProcessName, MAX_PATH);

    std::cout << "Loaded from: " << execProcessName << std::endl;

    // Get the id of the process loading the dll.
    uint32_t pid = GetCurrentProcessId();
    std::cout << "Process ID: " << std::to_string(pid).c_str() << std::endl;

    HMODULE ddrawModule = GetModuleHandle("ddraw.dll");
    if (ddrawModule != NULL)
    {
        std::cout << "Successfully loaded ddraw.dll handle!" << std::endl;
    }

    /*
    lpDDSurface = *(IDirectDrawSurface**)pDirectDrawSurface;
    lpDD = *(IDirectDraw **)pDirectDrawDevice;

    if (lpDDSurface) 
    {
        // IDirectDrawSurface::Blt
        origSurfaceBlt = (func_Blt)HookVTable((PDWORD)lpDDSurface, (DWORD)mySurfaceBlt, 5);
    }
    */
}

    __declspec(dllexport) BOOL __stdcall DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReseved
)
{
    switch (ul_reason_for_call) 
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        hMainThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
    	break;
    }
    return TRUE;
}