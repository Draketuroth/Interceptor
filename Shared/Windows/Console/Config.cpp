
#include <Windows.h>
#include "Config.h"

#include <stdio.h>

namespace Windows
{
    namespace Console
    {
        void SetupConsole()
        {
            AllocConsole();
            SetConsoleTitle("DirectDraw Hooker");
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
            freopen("CONIN$", "r", stdin);
        }
    }
}