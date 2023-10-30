#ifndef HMAIN_H
#define HMAIN_H


#include <iostream>
#include <Windows.h>

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


extern int windowWidth; // Spielfenster Breite
extern int windowHeight; // Spielfenster Höhe
extern char lWindowName[256]; // Overlay Name
extern HWND hWnd; // Overlay Handle
extern char tWindowName[256]; // Ziel Name
extern HWND TargetWnd; // Ziel Handle
extern RECT WindowRect, ClientRect; // Overlay, Spiel RECT
extern MSG Message; // Queue Nachricht

LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

extern IDirect3D9Ex* dx_Object;
extern IDirect3DDevice9Ex* dx_Device;
extern D3DPRESENT_PARAMETERS dx_Params;
extern ID3DXLine* dx_Line;
extern ID3DXFont* dx_Font;


int D3D9Init(HWND hWnd);

int Render();


#endif