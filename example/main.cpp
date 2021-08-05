#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif
#include <ctime>
#include <fstream>

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device* g_pd3dDevice = NULL;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
static ID3D12Fence* g_fence = NULL;
static HANDLE                       g_fenceEvent = NULL;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3* g_pSwapChain = NULL;
static HANDLE                       g_hSwapChainWaitableObject = NULL;
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
void ResizeSwapChain(HWND hWnd, int width, int height);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "src/imgui_notify.h"
#include "tahoma.h"

void init()
{
	ImGuiIO* io = &ImGui::GetIO();

	// We must load a font before loading notify, because we cannot merge font-awesome with default font
	// FontDataOwnedByAtlas = false is required (also in ImGui::MergeIconsWithLatestFont())
	// because otherwise ImGui will call free() while freeing resources which will lead into a crash
	// since tahoma is defined as readonly and wasn't allocated with malloc()
	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	io->Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

	// Initialize notify
	ImGui::MergeIconsWithLatestFont(16.f, false);
}

void render()
{
	ImGui::SetNextWindowPos({ 50, 50 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 550, 550 }, ImGuiCond_Once);
	ImGui::Begin("Hello World!", NULL, NULL);

	if (ImGui::CollapsingHeader("Examples without title", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Success"))
		{
			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Hello World! This is a success! %s", "We can also format here:)" });
		}

		ImGui::SameLine();
		if (ImGui::Button("Warning"))
		{
			ImGui::InsertNotification({ ImGuiToastType_Warning, 3000, "Hello World! This is a warning!" });
		}

		ImGui::SameLine();
		if (ImGui::Button("Error"))
		{
			ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Hello World! This is an error!" });
		}

		ImGui::SameLine();
		if (ImGui::Button("Info"))
		{
			ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Hello World! This is an info!" });
		}

		ImGui::SameLine();
		if (ImGui::Button("Info long"))
		{
			ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation" });
		}
	}

	if (ImGui::CollapsingHeader("Do it yourself", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static char title[4096] = "A wonderful quote!";
		ImGui::InputTextMultiline("Title", title, sizeof(title));

		static char content[4096] = "Ours is a friendship forged once in this life, and again in the next. Goodbye, my brother. \n- Dembe Zuma";
		ImGui::InputTextMultiline("Content", content, sizeof(content));

		static int duration = 5000; // 5 seconds
		ImGui::InputInt("Duration (ms)", &duration, 100);
		if (duration < 0) duration = 0; // Shouldn't be negative

		static char* type_str[] = { "None", "Success", "Warning", "Error", "Info" };
		static ImGuiToastType type = ImGuiToastType_Success;
		IM_ASSERT(type < ImGuiToastType_COUNT);

		if (ImGui::BeginCombo("Type", type_str[type]))
		{
			for (auto n = 0; n < IM_ARRAYSIZE(type_str); n++)
			{
				const bool is_selected = (type == n);

				if (ImGui::Selectable(type_str[n], is_selected))
					type = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		static bool enable_title = true, enable_content = true;
		ImGui::Checkbox("Enable title", &enable_title);
		ImGui::SameLine();
		ImGui::Checkbox("Enable content", &enable_content);

		if (ImGui::Button("Show"))
		{
			ImGuiToast toast(type, duration);

			if (enable_title)
				toast.set_title(title);

			if (enable_content)
				toast.set_content(content);

			ImGui::InsertNotification(toast);
		}
	}

	ImGui::End();

	// Render toasts on top of everything, at the end of your code!
	// You should push style vars here
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
	ImGui::RenderNotifications();
	ImGui::PopStyleVar(1); // Don't forget to Pop()
	ImGui::PopStyleColor(1);
}

// Main code
int main(int, char**)
{
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, _T("Dear ImGui DirectX12 Example"), WS_OVERLAPPEDWINDOW, 0, 0, 1150, 700, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO* io = &ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap, g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	init();

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(54.f / 255.f, 111.f / 255.f, 217.f / 255.f, 255.f / 255.f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		{
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}

		// Rendering
		render();

		// End scene
		{
			ImGui::Render();

			FrameContext* frameCtx = WaitForNextFrameResources();
			UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
			frameCtx->CommandAllocator->Reset();

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
			g_pd3dCommandList->ResourceBarrier(1, &barrier);

			// Render Dear ImGui graphics
			g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], (float*)&clear_color, 0, NULL);
			g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
			g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			g_pd3dCommandList->ResourceBarrier(1, &barrier);
			g_pd3dCommandList->Close();

			g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

			g_pSwapChain->Present(1, 0); // Present with vsync
			//g_pSwapChain->Present(0, 0); // Present without vsync

			UINT64 fenceValue = g_fenceLastSignaledValue + 1;
			g_pd3dCommandQueue->Signal(g_fence, fenceValue);
			g_fenceLastSignaledValue = fenceValue;
			frameCtx->FenceValue = fenceValue;
		}
	}

	WaitForLastSubmittedFrame();

	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug* pdx12Debug = NULL;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
		return false;

	// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != NULL)
	{
		ID3D12InfoQueue* pInfoQueue = NULL;
		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		pInfoQueue->Release();
		pdx12Debug->Release();
	}
#endif

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
			return false;
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 2; // <-- Set this value to 2 (the first descriptor is used for the built-in font texture, the second for our new texture)
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
			return false;

	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
		g_pd3dCommandList->Close() != S_OK)
		return false;

	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
		return false;

	g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_fenceEvent == NULL)
		return false;

	{
		IDXGIFactory4* dxgiFactory = NULL;
		IDXGISwapChain1* swapChain1 = NULL;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
			dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK ||
			swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
			return false;
		swapChain1->Release();
		dxgiFactory->Release();
		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
	if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
	if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
	if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
	if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
	if (g_fence) { g_fence->Release(); g_fence = NULL; }
	if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }

	//#ifdef DX12_ENABLE_DEBUG_LAYER
	//	IDXGIDebug1* pDebug = NULL;
	//	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
	//	{
	//		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
	//		//pDebug->Release();
	//	}
	//#endif
}

void CreateRenderTarget()
{
	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
	{
		ID3D12Resource* pBackBuffer = NULL;
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
		g_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

void WaitForLastSubmittedFrame()
{
	FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtx->FenceValue = 0;
	if (g_fence->GetCompletedValue() >= fenceValue)
		return;

	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
	WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
	UINT nextFrameIndex = g_frameIndex + 1;
	g_frameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
	DWORD numWaitableObjects = 1;

	FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue != 0) // means no fence was signaled
	{
		frameCtx->FenceValue = 0;
		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
		waitableObjects[1] = g_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtx;
}

void ResizeSwapChain(HWND hWnd, int width, int height)
{
	DXGI_SWAP_CHAIN_DESC1 sd;
	g_pSwapChain->GetDesc1(&sd);
	sd.Width = width;
	sd.Height = height;

	IDXGIFactory4* dxgiFactory = NULL;
	g_pSwapChain->GetParent(IID_PPV_ARGS(&dxgiFactory));

	g_pSwapChain->Release();
	CloseHandle(g_hSwapChainWaitableObject);

	IDXGISwapChain1* swapChain1 = NULL;
	dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1);
	swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain));
	swapChain1->Release();
	dxgiFactory->Release();

	g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);

	g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	assert(g_hSwapChainWaitableObject != NULL);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			WaitForLastSubmittedFrame();
			ImGui_ImplDX12_InvalidateDeviceObjects();
			CleanupRenderTarget();
			ResizeSwapChain(hWnd, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			CreateRenderTarget();
			ImGui_ImplDX12_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
