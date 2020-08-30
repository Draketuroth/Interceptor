
#include "stdafx.h"
#include "Win32Window.h"
#include "Win32Application.h"

namespace VE 
{
    namespace Platform 
    {
        Win32Window::Win32Window(unsigned int width, unsigned int height, std::wstring name) : 
            DXWindow(width, height, name), 
            _frameIndex(0),
            _viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
            _scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
            _rtvDescriptorSize(0)
        {

        }

        void Win32Window::OnInit()
        {
            LoadPipeline();
            LoadAssets();
        }

        void Win32Window::OnUpdate()
        {

        }

        void Win32Window::OnRender()
        {
            // Record all the commands we need to render the scene into the command list.
            PopulateCommandList();

            // Execute the command list.
            ID3D12CommandList* ppCommandList[] = { _commandList.Get() };
            _commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

            // Present the frame.
            ThrowIfFailed(_swapChain->Present(1, 0));

            WaitForPreviousFrame();
        }

        void Win32Window::OnDestroy()
        {
            // Ensure that the GPU is no longer referencing resources that are about to be
            // cleaned up by the destructor.
            WaitForPreviousFrame();

            CloseHandle(_fenceEvent);
        }

        void Win32Window::LoadPipeline()
        {
            UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
            // Enable the debug layer (requires the Graphics Tools "optional feature").
            // NOTE: Enabling the debug layer after device creation will invalidate the active device.
            {
                Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
                if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) 
                {
                    debugController->EnableDebugLayer();

                    // Enable additional debug layers.
                    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
                }
            }
#endif

            Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
            ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

            if (_useWarpDevice) 
            {
                Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
                ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

                ThrowIfFailed(D3D12CreateDevice(
                    warpAdapter.Get(),
                    D3D_FEATURE_LEVEL_11_0,
                    IID_PPV_ARGS(&_device)
                    ));
            }
            else 
            {
                Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
                GetHardwareAdapter(factory.Get(), &hardwareAdapter);

                ThrowIfFailed(D3D12CreateDevice(
                    hardwareAdapter.Get(),
                    D3D_FEATURE_LEVEL_11_0,
                    IID_PPV_ARGS(&_device)
                ));
            }

            // Describe and create the command queue.
            D3D12_COMMAND_QUEUE_DESC queueDesc = {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            ThrowIfFailed(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)));

            // Describe and create the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.BufferCount = _frameCount;
            swapChainDesc.Width = _width;
            swapChainDesc.Height = _height;
            swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.SampleDesc.Count = 1;

            Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
            ThrowIfFailed(factory->CreateSwapChainForHwnd
            (
                _commandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
                Win32Application::GetHwnd(),
                &swapChainDesc,
                nullptr,
                nullptr,
                &swapChain
            ));

            // This sample does not support fullscreen transitions.
            ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

            ThrowIfFailed(swapChain.As(&_swapChain));
            _frameIndex = _swapChain->GetCurrentBackBufferIndex();

            // Create descriptor heaps.
            {
                // Describe and create a render target view (RTV) descriptor head.
                D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
                rtvHeapDesc.NumDescriptors = _frameCount;
                rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                ThrowIfFailed(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap)));

                _rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            }

            // Create frame resources.
            {
                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

                // Create a RTV for each frame.
                for (unsigned int n = 0; n < _frameCount; n++) 
                {
                    ThrowIfFailed(_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n])));
                    _device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, rtvHandle);
                    rtvHandle.Offset(1, _rtvDescriptorSize);
                }
            }

            ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)));
        }

        void Win32Window::LoadAssets()
        {
            // Create an empty root signature.
            {
                CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
                rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

                Microsoft::WRL::ComPtr<ID3DBlob> signature;
                Microsoft::WRL::ComPtr<ID3DBlob> error;
                ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
                ThrowIfFailed(_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)));
            }

            // Create the pipeline state, which includes compiling and loading shaders.
            {
                Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
                Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
                // Enable better shader debugging with the graphics debugging tools.
                UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
                UINT compileFlags = 0;
#endif
                ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
                ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

                // Define the vertex input layout.
                D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
                {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
                };

                // Describe and create the graphics pipeline state object (PSO).
                D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
                psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
                psoDesc.pRootSignature = _rootSignature.Get();
                psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
                psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
                psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
                psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
                psoDesc.DepthStencilState.DepthEnable = FALSE;
                psoDesc.DepthStencilState.StencilEnable = FALSE;
                psoDesc.SampleMask = UINT_MAX;
                psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                psoDesc.NumRenderTargets = 1;
                psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
                psoDesc.SampleDesc.Count = 1;
                ThrowIfFailed(_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState)));
            }

            // Create the command list.
            ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(&_commandList)));

            // Command lists are created in the recording state, but there is nothing
            // to record yet. The main loop expects it to be closed, so close it now.
            ThrowIfFailed(_commandList->Close());

            // Create the vertex buffer.
            {
                Vertex triangleVertices[] =
                {
                    { {0.0f, 0.25f * _aspectRatio, 0.0f } , {1.0f, 0.0f, 0.0f, 1.0f} },
                    { {0.25f, -0.25f * _aspectRatio, 0.0f } , {0.0f, 1.0f, 0.0f, 1.0f} },
                    { {-0.25f, -0.25f * _aspectRatio, 0.0f } , {1.0f, 0.0f, 1.0f, 1.0f} }
                };

                const UINT vertexBufferSize = sizeof(triangleVertices);

                // Note: using upload heaps to transfer static data is not recommended. Read up on Default Heap usage.
                ThrowIfFailed(_device->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                    D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&_vertexBuffer)
                ));

                // Copy the triangle data to the vertex buffer.
                UINT8* vertexDataBegin;
                CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
                ThrowIfFailed(_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataBegin)));
                memcpy(vertexDataBegin, triangleVertices, sizeof(triangleVertices));
                _vertexBuffer->Unmap(0, nullptr);

                // Initialize the vertex buffer view.
                _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
                _vertexBufferView.StrideInBytes = sizeof(Vertex);
                _vertexBufferView.SizeInBytes = vertexBufferSize;
            }

            // Create synchronization objects and wait until assets have been uploaded to the GPU.
            {
                ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
                _fenceValue = 1;

                // Create an event handle to use for frame synchronization.
                _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                if (_fenceEvent == nullptr) 
                {
                    ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
                }

                WaitForPreviousFrame();
            }
        }

        void Win32Window::PopulateCommandList()
        {
            // Command list allocators can only be reset when the associated
            // command lists have finished execution on the GPU; apps should
            // fences to determine GPU progress.
            ThrowIfFailed(_commandAllocator->Reset());

            // However, when ExecuteCommandList() is called on a particular command
            // list, that command list can then be reset at any time and must be before
            // re-recording.
            ThrowIfFailed(_commandList->Reset(_commandAllocator.Get(), _pipelineState.Get()));

            // Set necessary state.
            _commandList->SetGraphicsRootSignature(_rootSignature.Get());
            _commandList->RSSetViewports(1, &_viewport);
            _commandList->RSSetScissorRects(1, &_scissorRect);

            // Indicate that the back buffer will be used as a render target.
            _commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex, _rtvDescriptorSize);
            _commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

            // Record commands.
            const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            _commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            _commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
            _commandList->DrawInstanced(3, 1, 0, 0);

            // Indicate that the back buffer will now be used to present.
            _commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

            ThrowIfFailed(_commandList->Close());
        }

        void Win32Window::WaitForPreviousFrame()
        {
            // Signal and increment the fence value.
            const uint64_t fence = _fenceValue;
            ThrowIfFailed(_commandQueue->Signal(_fence.Get(), fence));
            _fenceValue++;

            // Wait until the previous frame is finished.
            if (_fence->GetCompletedValue() < fence) 
            {
                ThrowIfFailed(_fence->SetEventOnCompletion(fence, _fenceEvent));
                WaitForSingleObject(_fenceEvent, INFINITE);
            }

            _frameIndex = _swapChain->GetCurrentBackBufferIndex();
        }
    }
}