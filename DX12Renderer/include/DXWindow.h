#ifndef VE_DXWINDOW_H
#define VE_DXWINDOW_H

#include "DXHelper.h"
#include <wchar.h>

namespace VE 
{
    namespace Platform 
    {
        class DXWindow
        {
        public:
            DXWindow(unsigned int width, unsigned int height, std::wstring name);
            virtual ~DXWindow();

            virtual void OnInit() = 0;
            virtual void OnUpdate() = 0;
            virtual void OnRender() = 0;
            virtual void OnDestroy() = 0;

            virtual void OnKeyDown(uint8_t key) {};
            virtual void OnKeyUp(uint8_t key) {};

            unsigned int GetWidth() const { return _width; }
            unsigned int GetHeight() const { return _height; }
            const std::wstring GetTitle() const { return _title.c_str(); }

            void ParseCommandLineArgs(wchar_t* argv[], int argc);

        protected:
            std::wstring GetAssetFullPath(const wchar_t* assetName);

            _Use_decl_annotations_
            void GetHardwareAdapter(
                _In_ IDXGIFactory1* pFactory,
                _Outptr_result_maybenull_ IDXGIAdapter1** pAdapter,
                bool requestHighPerformanceAdapter = false);
            
            void SetCustomWindowText(const wchar_t* text);

            unsigned int _width;
            unsigned int _height;
            float _aspectRatio;

            bool _useWarpDevice;

        private:
            std::wstring _assetsPath;
            std::wstring _title;
        };
    }
}


#endif