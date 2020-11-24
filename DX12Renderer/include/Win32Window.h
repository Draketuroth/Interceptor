#ifndef VE_WIN32WINDOW_H
#define VE_WIN32WINDOW_H

#include "DXWindow.h"

namespace VE 
{
    namespace Platform 
    {
        class Win32Window : public DXWindow
        {
        public:
            Win32Window(unsigned int width, unsigned int height, std::wstring name);

            virtual void OnInit();
            virtual void OnUpdate();
            virtual void OnRender();
            virtual void OnDestroy();

        private:

            double _appCounter = 0.0;

            static const unsigned int _frameCount = 2;

            struct Vertex 
            {
                DirectX::XMFLOAT3 positon;
                DirectX::XMFLOAT4 color;
            };

            struct TransformOffsetBuffer 
            {
                DirectX::XMFLOAT4 offset;
                DirectX::XMFLOAT4X4 matrixModel;
                DirectX::XMFLOAT4X4 matrixView;
                DirectX::XMFLOAT4X4 matrixProjection;
                float padding[12]; 
            };
            static_assert((sizeof(TransformOffsetBuffer) % 256) == 0, "Constant buffer size must be 256-byte aligned");

            // Pipeline objects.
            CD3DX12_VIEWPORT _viewport;
            CD3DX12_RECT _scissorRect;

            Microsoft::WRL::ComPtr<IDXGISwapChain3> _swapChain;
            Microsoft::WRL::ComPtr<ID3D12Device> _device;
            Microsoft::WRL::ComPtr<ID3D12Resource> _renderTargets[_frameCount];
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _commandAllocator;
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> _commandQueue;
            Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap;
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _cbvHeap;
            Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList;
            unsigned int _rtvDescriptorSize;

            // App resources.
            Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBuffer;
            D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
            Microsoft::WRL::ComPtr<ID3D12Resource> _constantBuffer;
            TransformOffsetBuffer _constantBufferData;
            UINT8* _pCbvDataBegin;

            // Synchronization objects
            unsigned int _frameIndex;
            HANDLE _fenceEvent;
            Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
            std::uint64_t _fenceValue;

            void LoadPipeline(); // Load the rendering pipeline dependencies.
            void LoadAssets();
            void PopulateCommandList();
            void WaitForPreviousFrame();
        };
    }
}

#endif