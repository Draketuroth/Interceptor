
#include <windows.h>

#include "Windows/Console/Config.h"
#include "Windows/Console/Print.h"

BOOL __stdcall DllEntryPoint(HINSTANCE hDll, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}