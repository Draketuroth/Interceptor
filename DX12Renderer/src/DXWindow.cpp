
#include "stdafx.h"
#include "DXWindow.h"

namespace VE 
{
    namespace Platform 
    {
        DXWindow::DXWindow(unsigned int width, unsigned height, std::wstring name) :
            _width(width),
            _height(height),
            _title(name),
            _useWarpDevice(false) 
        {
            wchar_t assetsPath[512];
            GetAssetsPath(assetsPath, _countof(assetsPath));
            _assetsPath = assetsPath;

            _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        }
        DXWindow::~DXWindow()
        {

        }
        void DXWindow::ParseCommandLineArgs(wchar_t* argv[], int argc)
        {
            for (int i = 1; i < argc; ++i) 
            {
                if(_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
                   _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
                {
                    _useWarpDevice = true;
                    _title = _title + L" (WARP)";
                }
            }
        }
        std::wstring DXWindow::GetAssetFullPath(const wchar_t* assetName)
        {
            return _assetsPath + assetName;
        }

        _Use_decl_annotations_
        void DXWindow::GetHardwareAdapter(
            IDXGIFactory1* pFactory, 
            IDXGIAdapter1** pAdapter, 
            bool requestHighPerformanceAdapter)
        {
            *pAdapter = nullptr;
            
            Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

            Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
            if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) 
            {
                for (UINT adapterIndex = 0;
                    DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                        adapterIndex,
                        requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                        IID_PPV_ARGS(&adapter));
                    ++adapterIndex) 
                {
                    DXGI_ADAPTER_DESC1 desc;
                    adapter->GetDesc1(&desc);
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) 
                    {
                        // Deon't select the Basic Render Driver adapter.
                        // If you want a software adapter, pass in "/warp" on the command line.
                        continue;
                    }

                    // Check to see whether the adapter supports Direct3D 12, but don't create
                    // the actual device yet.
                    if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
                    {
                        break;
                    }
                }
            
            }
            else
            {
                for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) 
                {
                    DXGI_ADAPTER_DESC1 desc;
                    adapter->GetDesc1(&desc);
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    {
                        // Deon't select the Basic Render Driver adapter.
                        // If you want a software adapter, pass in "/warp" on the command line.
                        continue;
                    }

                    // Check to see whether the adapter supports Direct3D 12, but don't create
                    // the actual device yet.
                    if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
                    {
                        break;
                    }
                }
            }
        }
        void DXWindow::SetCustomWindowText(const wchar_t* text)
        {
            std::wstring windowText = _title.empty() ? text : _title + L": " + text;
        }
    }
}