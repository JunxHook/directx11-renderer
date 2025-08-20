#pragma once

#include <Windows.h>
#include <d3d11.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;