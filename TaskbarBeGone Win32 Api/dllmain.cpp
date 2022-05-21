// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

HINSTANCE localInstance;
static HWND hShellWnd = ::FindWindow((L"Shell_TrayWnd"), NULL);
std::atomic_bool stop_thread = false;
bool toggleTaskbar = false;
static NOTIFYICONDATA nid;

LRESULT CALLBACK DllProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TrayDrawIcon(HWND hWnd);
void MessageLoop(HWND hwnd, int  id, UINT fsModifiers, char k);

// External Functions for C#
extern "C" {
	DLLEXPORT void RegisterInstance(HINSTANCE hInstance) {
		localInstance = hInstance;
		std::thread TrayIcon(TrayDrawIcon, (HWND)hInstance);
		TrayIcon.detach();
	}

	DLLEXPORT void RegisterHotkey(HWND hwnd, int  id, UINT fsModifiers, char k) {
		stop_thread = false;
		std::thread thread(MessageLoop, hwnd, id, fsModifiers, k);
		thread.detach();
	}

	DLLEXPORT void UnregisterHotkey() {
		stop_thread = true;
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	localInstance = hModule;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

void MessageLoop(HWND hwnd, int  id, UINT fsModifiers, char k) {
	UINT vk = VkKeyScanA(k);
	RegisterHotKey(NULL, id, fsModifiers, vk);

	// Message Loop
	MSG msg = { };
	while (true) {
		// Unregister Hotkey and exit thread
		if (stop_thread) {
			UnregisterHotKey(hwnd, id);
			return;
		}

		// Peek at messages to proccess hotkey
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			DllProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		}
	}
}

LRESULT CALLBACK DllProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
		case 1000:
			OutputDebugString(L"ICON\n");
		case WM_HOTKEY:
			OutputDebugString(L"HOTKEY PRESSED\n");
			//ShowWindow(hShellWnd, toggleTaskbar ? SW_SHOW : SW_HIDE);
			toggleTaskbar = !toggleTaskbar;
			break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void TrayDrawIcon(HWND hWnd) {
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = 1000;
	nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcscpy_s(nid.szTip, L"Tray Icon");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	Shell_NotifyIcon(NIM_ADD, &nid);

	while (true) {
		
	}
}