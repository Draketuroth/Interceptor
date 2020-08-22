
#include "stdafx.h"
#include "Win32Window.h"
#include "Win32Application.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    VE::Platform::Win32Window sample(1280, 720, L"D3D12 Hello Window");
    return VE::Platform::Win32Application::run(&sample, hInstance, nCmdShow);
}