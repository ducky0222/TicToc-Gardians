#include "../TickTock_Guardians/pch.h"
#include "LauncherTTG.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    LauncherTTG ttg{ hInstance };

    ttg.Loop();
}