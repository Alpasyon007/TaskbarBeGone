#include "TaskbarBeGone.h"

TaskbarBeGone::TaskbarBeGone() : taskbar(FindWindow("Shell_TrayWnd", nullptr)) {
	// Create application window
	// ImGui_ImplWin32_EnableDpiAwareness();
	wc = {sizeof(wc), CS_CLASSDC, TaskbarBeGone::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL};
	::RegisterClassExW(&wc);
	hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100, 520, 520, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if(!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	(void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;	 // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge = true;
	// io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if(io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding			  = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
						g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
}

TaskbarBeGone::~TaskbarBeGone() {
	WaitForLastSubmittedFrame();

	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

void TaskbarBeGone::Run() {
	runningApplications.clear();
	EnumWindows(TaskbarBeGone::EnumWindowsProc, reinterpret_cast<LPARAM>(&runningApplications));

	// Main loop
	while(!done) {
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while(::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if(msg.message == WM_QUIT) done = true;
		}
		if(done) break;

		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// IMGUI CODE
		ImGuiID dockspace = ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_NoTabBar);

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		// if(show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

		{
			ImGui::Begin("Hello", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
			// Begin the menu bar
			if(ImGui::BeginMenuBar()) {
				// Add a "File" menu
				if(ImGui::BeginMenu("File")) {
					if(ImGui::MenuItem("Open", "Ctrl+O")) { /* Do something */
					}
					if(ImGui::MenuItem("Save", "Ctrl+S")) { /* Do something */
					}
					if(ImGui::MenuItem("Quit", "Alt+F4")) {}
					ImGui::EndMenu();
				}

				// Add an "Edit" menu
				if(ImGui::BeginMenu("Edit")) {
					if(ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Do something */
					}
					if(ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Do something */
					}
					ImGui::EndMenu();
				}

				// End the menu bar
				ImGui::EndMenuBar();
			}

			// Get the HWND of the currently focused window
			HWND foregroundWindow = GetForegroundWindow();

			for(auto& app : runningApplications) {
				// Check if the application's HWND matches the focused HWND
				app.focused = (app.id == foregroundWindow);

				ImGui::Selectable(app.title.c_str(), &app.selected);

				// Draw a different color for the focused application
				if(app.focused) {
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[focused]");

					ShowWindow(taskbar, (app.selected) ? SW_HIDE : SW_SHOW);
				}
			}

			ImGui::End();
		}

		ImGui::DockBuilderDockWindow("Hello", dockspace);

		// IMGUI CODE

		// Rendering
		ImGui::Render();

		FrameContext* frameCtx		= WaitForNextFrameResources();
		UINT		  backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
		g_pd3dCommandList->ResourceBarrier(1, &barrier);

		// Render Dear ImGui graphics
		const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
		g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
		g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
		g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		g_pd3dCommandList->ResourceBarrier(1, &barrier);
		g_pd3dCommandList->Close();

		g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

		// Update and Render additional Platform Windows
		if(io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)g_pd3dCommandList);
		}

		g_pSwapChain->Present(1, 0); // Present with vsync
		// g_pSwapChain->Present(0, 0); // Present without vsync

		UINT64 fenceValue = g_fenceLastSignaledValue + 1;
		g_pd3dCommandQueue->Signal(g_fence, fenceValue);
		g_fenceLastSignaledValue = fenceValue;
		frameCtx->FenceValue	 = fenceValue;
	}
}

// Helper functions
bool TaskbarBeGone::CreateDeviceD3D(HWND hWnd) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount		  = NUM_BACK_BUFFERS;
		sd.Width			  = 0;
		sd.Height			  = 0;
		sd.Format			  = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags			  = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage		  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count	  = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect		  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode		  = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling			  = DXGI_SCALING_STRETCH;
		sd.Stereo			  = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug* pdx12Debug = NULL;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug)))) pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if(D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK) return false;

		// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if(pdx12Debug != NULL) {
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
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors				= NUM_BACK_BUFFERS;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask					= 1;
		if(g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK) return false;

		SIZE_T						rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle		  = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for(UINT i = 0; i < NUM_BACK_BUFFERS; i++) {
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors				= 1;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if(g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK) return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type					  = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags					  = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask				  = 1;
		if(g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK) return false;
	}

	for(UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if(g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK) return false;

	if(g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
	   g_pd3dCommandList->Close() != S_OK)
		return false;

	if(g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK) return false;

	g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(g_fenceEvent == NULL) return false;

	{
		IDXGIFactory4*	 dxgiFactory = NULL;
		IDXGISwapChain1* swapChain1	 = NULL;
		if(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK) return false;
		if(dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK) return false;
		if(swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK) return false;
		swapChain1->Release();
		dxgiFactory->Release();
		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void TaskbarBeGone::CleanupDeviceD3D() {
	CleanupRenderTarget();
	if(g_pSwapChain) {
		g_pSwapChain->SetFullscreenState(false, NULL);
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if(g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
	for(UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if(g_frameContext[i].CommandAllocator) {
			g_frameContext[i].CommandAllocator->Release();
			g_frameContext[i].CommandAllocator = NULL;
		}
	if(g_pd3dCommandQueue) {
		g_pd3dCommandQueue->Release();
		g_pd3dCommandQueue = NULL;
	}
	if(g_pd3dCommandList) {
		g_pd3dCommandList->Release();
		g_pd3dCommandList = NULL;
	}
	if(g_pd3dRtvDescHeap) {
		g_pd3dRtvDescHeap->Release();
		g_pd3dRtvDescHeap = NULL;
	}
	if(g_pd3dSrvDescHeap) {
		g_pd3dSrvDescHeap->Release();
		g_pd3dSrvDescHeap = NULL;
	}
	if(g_fence) {
		g_fence->Release();
		g_fence = NULL;
	}
	if(g_fenceEvent) {
		CloseHandle(g_fenceEvent);
		g_fenceEvent = NULL;
	}
	if(g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}

#ifdef DX12_ENABLE_DEBUG_LAYER
	IDXGIDebug1* pDebug = NULL;
	if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug)))) {
		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		pDebug->Release();
	}
#endif
}

void TaskbarBeGone::CreateRenderTarget() {
	for(UINT i = 0; i < NUM_BACK_BUFFERS; i++) {
		ID3D12Resource* pBackBuffer = NULL;
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
		g_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void TaskbarBeGone::CleanupRenderTarget() {
	WaitForLastSubmittedFrame();

	for(UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if(g_mainRenderTargetResource[i]) {
			g_mainRenderTargetResource[i]->Release();
			g_mainRenderTargetResource[i] = NULL;
		}
}

void TaskbarBeGone::WaitForLastSubmittedFrame() {
	FrameContext* frameCtx	 = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64		  fenceValue = frameCtx->FenceValue;
	if(fenceValue == 0) return; // No fence was signaled

	frameCtx->FenceValue = 0;
	if(g_fence->GetCompletedValue() >= fenceValue) return;

	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
	WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* TaskbarBeGone::WaitForNextFrameResources() {
	UINT nextFrameIndex				 = g_frameIndex + 1;
	g_frameIndex					 = nextFrameIndex;

	HANDLE		  waitableObjects[]	 = {g_hSwapChainWaitableObject, NULL};
	DWORD		  numWaitableObjects = 1;

	FrameContext* frameCtx			 = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64		  fenceValue		 = frameCtx->FenceValue;
	if(fenceValue != 0) // means no fence was signaled
	{
		frameCtx->FenceValue = 0;
		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
		waitableObjects[1] = g_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtx;
}

// Callback function to receive information about each window
BOOL CALLBACK TaskbarBeGone::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	// Get the process ID for the window
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);

	// Get the window title
	char title[256];
	GetWindowTextA(hwnd, title, sizeof(title));

	// If the window is visible and has a title, add it to the list
	if(IsWindowVisible(hwnd) && strlen(title) > 0) {
		auto&		 apps = *reinterpret_cast<std::vector<SelectedApps>*>(lParam);
		SelectedApps app  = {title, hwnd, false};
		apps.push_back(app);
	}

	return TRUE;
}

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI TaskbarBeGone::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	static NOTIFYICONDATA nid = {sizeof(nid)};

	switch(msg) {
		case WM_SIZE:
			if(g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
				WaitForLastSubmittedFrame();
				CleanupRenderTarget();
				HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN,
															 DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
				assert(SUCCEEDED(result) && "Failed to resize swapchain.");
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if(wParam == SC_MINIMIZE) {
				// Hide the window instead of minimizing it
				ShowWindow(hWnd, SW_HIDE);

				memset(&nid, 0, sizeof(nid));
				nid.cbSize			 = sizeof(nid);
				nid.hWnd			 = hWnd;
				nid.uID				 = 1;
				nid.uFlags			 = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP | NIF_GUID;
				nid.uCallbackMessage = WM_USER + 1;
				nid.hIcon			 = LoadIcon(NULL, IDI_APPLICATION);
				strcpy(nid.szTip, "My Application");
				Shell_NotifyIcon(NIM_ADD, &nid);
				return 0;
			}
			if((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY: ::PostQuitMessage(0); return 0;
		case WM_TRAYICON: {
			// Restore the window when the system tray icon is clicked
			if(lParam == WM_LBUTTONUP) {
				Shell_NotifyIcon(NIM_DELETE, &nid);
				ShowWindow(hWnd, SW_SHOW);
				SetForegroundWindow(hWnd);
			}
		} break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}