#ifndef TASKBAR_BE_GONE_H
#define TASKBAR_BE_GONE_H

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#include <algorithm>
#include <string>
#include <vector>

#ifdef _DEBUG
	#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
	#include <dxgidebug.h>
	#pragma comment(lib, "dxguid.lib")
#endif

#define WM_TRAYICON (WM_USER + 1)

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct FrameContext {
	ID3D12CommandAllocator* CommandAllocator;
	UINT64					FenceValue;
};

class SelectedApps {
private:
	template <class Archive> void serialize(Archive& ar, const unsigned int version) {
		ar& title;
		ar& id;
		ar& selected;
		ar& focused;
	}
public:
	std::string title;
	int			id;
	bool		selected;
	bool		focused;
};

class TaskbarBeGone {
public:
	TaskbarBeGone();
	~TaskbarBeGone();

	void Run();
private:
	void				  MainWindow();

	void				  addToStartup();
	void				  removeFromStartup();

	bool				  CreateDeviceD3D(HWND hWnd);
	void				  CleanupDeviceD3D();
	static void			  CreateRenderTarget();
	static void			  CleanupRenderTarget();
	static void			  WaitForLastSubmittedFrame();
	FrameContext*		  WaitForNextFrameResources();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK  EnumWindowsProc(HWND hwnd, LPARAM lParam);
private:
	static int const						  NUM_FRAMES_IN_FLIGHT							 = 3;
	inline static FrameContext				  g_frameContext[NUM_FRAMES_IN_FLIGHT]			 = {};
	inline static UINT						  g_frameIndex									 = 0;

	static int const						  NUM_BACK_BUFFERS								 = 3;
	inline static ID3D12Device*				  g_pd3dDevice									 = NULL;
	ID3D12DescriptorHeap*					  g_pd3dRtvDescHeap								 = NULL;
	ID3D12DescriptorHeap*					  g_pd3dSrvDescHeap								 = NULL;
	ID3D12CommandQueue*						  g_pd3dCommandQueue							 = NULL;
	ID3D12GraphicsCommandList*				  g_pd3dCommandList								 = NULL;
	inline static ID3D12Fence*				  g_fence										 = NULL;
	inline static HANDLE					  g_fenceEvent									 = NULL;
	UINT64									  g_fenceLastSignaledValue						 = 0;
	inline static IDXGISwapChain3*			  g_pSwapChain									 = NULL;
	HANDLE									  g_hSwapChainWaitableObject					 = NULL;
	inline static ID3D12Resource*			  g_mainRenderTargetResource[NUM_BACK_BUFFERS]	 = {};
	inline static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

	const wchar_t							  windowTitle[14]								 = L"TaskbarBeGone";

	HWND									  hwnd;
	WNDCLASSEXW								  wc;
	ImGuiIO*								  io;

	bool									  done				  = false;
	bool									  runOnStartup		  = false;

	inline static bool						  minimizeToTray	  = true;
	bool									  show_demo_window	  = true;
	bool									  show_another_window = false;
	const ImVec4							  clear_color		  = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	HWND									  taskbar;
	std::vector<SelectedApps>				  runningApplications;
};

#endif /* TASKBAR_BE_GONE_H */
