// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.

#include "Draw/hMain.h"
#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <dwmapi.h>
#include <TlHelp32.h>
#include <string>
#include <Psapi.h>
#include <iostream>
#include "Draw/hDraw.h"
#include <vector>
#include <iomanip>
#include <iostream>
#include <math.h>
#include "Structs/Structs.h"
#include "Hooks/Hooks.h"
#include "Hack/hack.h"

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
using namespace std;

IDirect3D9Ex* dx_Object = NULL;
IDirect3DDevice9Ex* dx_Device = NULL;
D3DPRESENT_PARAMETERS dx_Params;
ID3DXLine* dx_Line;


HWND hWnd, TargetWnd;
MSG Message;
RECT WindowRect, ClientRect;
int clientWidth = 800, clientHeight = 600;
int borderWidth, borderHeight; // Grenz Größen


char lWindowName[256] = "Overlay";
char tWindowName[256] = "Skyrim"; // put Game window name here

const MARGINS pMargin = { 0,0, clientWidth, clientHeight };




void PlaceJMP(BYTE* Address, DWORD jumpTo, DWORD length = 5) {
	DWORD dwOldProtect, dwBkup, dwRelAddr;

	VirtualProtect(Address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	dwRelAddr = (DWORD)(jumpTo - (DWORD)Address) - 5;

	*Address = 0xE9;

	*((DWORD*)(Address + 0x1)) = dwRelAddr;

	for (DWORD x = 0x5; x < length; x++)
	{
		*(Address + x) = 0x90;
	}

	VirtualProtect(Address, length, dwOldProtect, &dwBkup);
}


void MsgBoxAddy(DWORD addy) {
	char szBuffer[1024];
	sprintf(szBuffer, "%02x", addy);
	MessageBox(NULL, szBuffer, "Title", MB_OK);

	//copy addy to clipboard
	const char* output = szBuffer;
	const size_t len = strlen(output) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), output, len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}




DWORD FindPattern(char* module, char* pattern, char* mask) {

	MODULEINFO  mInfo = GetModuleInfo(module);
	DWORD base = (DWORD)mInfo.lpBaseOfDll;
	DWORD size = (DWORD)mInfo.SizeOfImage;

	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		if (found) {
			return base + i;
		}
	}
}

DWORD WINAPI InitiateHooks(LPVOID param) {
	//entityhook

	char* module = _strdup("TESV.exe");
	char* pattern = _strdup("\x8B\x56\x34\x8B\x46\x38\x8B\x4E\x3C\x8D\x7E\x34");
	char* mask = _strdup("xxxxxxxxxxxx");

	DWORD entityAddy = FindPattern(module, pattern, mask);

	EntlistJmpBack = entityAddy + 0x6;
	PlaceJMP((BYTE*)entityAddy, (DWORD)entityhook, 6);

	return NULL;
}



LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{

	case WM_PAINT: // we need to paint? lets paint!
		Render();
		break;
	case WM_CREATE:
		return DwmExtendFrameIntoClientArea(hWnd, &pMargin); // extension of window frame into client area
		break;
	case WM_DESTROY:
		PostQuitMessage(0); // We need to use this to exit a message loop
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam); // Making sure all messages are processed
		break;
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEXA OverlayWnd; // contains window class information
	OverlayWnd.cbSize = sizeof(WNDCLASSEXA); // size of struct, basically checking for version or check
	OverlayWnd.style = CS_HREDRAW | CS_VREDRAW;  // Style, redraw method type
	OverlayWnd.lpfnWndProc = Proc; // Pointer to the window procedure
	OverlayWnd.cbClsExtra = 0; // window class struct extra bytes
	OverlayWnd.cbWndExtra = 0; // window instance extra bytes
	OverlayWnd.hInstance = hInstance; // handle to the instance that contains the window procedure for the class
	OverlayWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // basic window icon set
	OverlayWnd.hCursor = LoadCursor(NULL, IDC_ARROW); // basic window cursor icon set
	OverlayWnd.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)); // handle to the class background brush
	OverlayWnd.lpszMenuName = lWindowName;
	OverlayWnd.lpszClassName = lWindowName;

	// registers a window class for the use in call to this createwindowex func
	if (!RegisterClassExA(&OverlayWnd))
	{
		exit(1);
	}

	TargetWnd = FindWindowA(0, tWindowName);

	if (TargetWnd)
	{
		GetWindowRect(TargetWnd, &WindowRect);
		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;
		hWnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, windowWidth, windowHeight, 0, 0, 0, 0);
	}

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);


	ShowWindow(hWnd, SW_SHOW);
	D3D9Init(hWnd);

	bool panic = false;
	while (panic == false)
	{

		if (GetAsyncKeyState(VK_F12))
			panic = true;

		if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);

			DispatchMessage(&Message);
		}

		TargetWnd = FindWindowA(0, tWindowName);

		if (!TargetWnd)
			exit(0);

		GetWindowRect(TargetWnd, &WindowRect);
		GetClientRect(TargetWnd, &ClientRect);

		windowWidth = WindowRect.right - WindowRect.left;
		windowHeight = WindowRect.bottom - WindowRect.top;


		DWORD dwStyle = GetWindowLong(TargetWnd, GWL_STYLE);

		if (dwStyle & WS_BORDER)
		{
			windowHeight = WindowRect.bottom - WindowRect.top;
			windowWidth = WindowRect.right - WindowRect.left;
			clientHeight = ClientRect.bottom - ClientRect.top;
			clientWidth = ClientRect.right - ClientRect.left;
			borderHeight = (windowHeight - ClientRect.bottom);
			borderWidth = (windowWidth - ClientRect.right) / 2; //only want one side
			borderHeight -= borderWidth; //remove bottom from width, bottom is the same size as either side so we subtract that

			WindowRect.left += borderWidth;
			WindowRect.top += borderHeight;
		}

		MoveWindow(hWnd, WindowRect.left, WindowRect.top, clientWidth, clientHeight, true);

	} 
	exit(0);


}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		CreateThread(0, NULL, (LPTHREAD_START_ROUTINE)WinMain, (LPVOID)L"Window Title", NULL, NULL);
		CreateThread(0, NULL, InitiateHooks ,0 , NULL, NULL);

	}
	return TRUE;
}

int D3D9Init(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &dx_Object)))
	{
		exit(1);
	}

	dx_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	dx_Params.BackBufferWidth = windowWidth;
	dx_Params.BackBufferHeight = windowHeight;
	dx_Params.EnableAutoDepthStencil = TRUE;
	dx_Params.AutoDepthStencilFormat = D3DFMT_D16;
	dx_Params.hDeviceWindow = hWnd;
	dx_Params.MultiSampleQuality = DEFAULT_QUALITY;
	dx_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dx_Params.Windowed = TRUE;
	if (FAILED(dx_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dx_Params, 0, &dx_Device)))
	{
		exit(1);
	}


	if (!dx_Line)
		D3DXCreateLine(dx_Device, &dx_Line);

	D3DXCreateFont(dx_Device, 12, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &dx_Font);

	return 0;

}



int Render()
{
	dx_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	dx_Device->BeginScene();
	InitHack();

	if (TargetWnd == GetForegroundWindow())
	{

		if (GetAsyncKeyState('1'))
		{
			nameesp = !nameesp;
			boneesp = !boneesp;
		}

		if (boneesp) {
			BoneESP();
		
		}
		if (nameesp) {
			DrawEntNames();
		}

		
		Sleep(1);

	}


	dx_Device->EndScene();
	dx_Device->PresentEx(0, 0, 0, 0, 0);
	return 0;
}

