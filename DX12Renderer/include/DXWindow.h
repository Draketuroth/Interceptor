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

            virtual void onInit() = 0;
            virtual void onUpdate() = 0;
            virtual void onRender() = 0;
            virtual void onDestroy() = 0;

            virtual void onKeyDown(uint8_t key) {};
            virtual void onKeyUp(uint8_t key) {};

            unsigned int getWidth() const { return _width; }
            unsigned int getHeight() const { return _height; }
            const std::wstring getTitle() const { return _title.c_str(); }

            void parseCommandLineArgs(wchar_t* argv[], int argc);

        protected:
            std::wstring getAssetFullPath(const wchar_t* assetName);

            _Use_decl_annotations_
            void getHardwareAdapter(
                _In_ IDXGIFactory1* pFactory,
                _Outptr_result_maybenull_ IDXGIAdapter1** pAdapter,
                bool requestHighPerformanceAdapter = false);
            
            void setCustomWindowText(const wchar_t* text);

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