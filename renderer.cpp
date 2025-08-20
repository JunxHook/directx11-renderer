#include "renderer.h"

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;


bool Renderer::Initialize ( HWND hwnd )
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray [ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT res = D3D11CreateDeviceAndSwapChain ( NULL , D3D_DRIVER_TYPE_HARDWARE , NULL , createDeviceFlags , featureLevelArray , 2 , D3D11_SDK_VERSION , &sd , &g_pSwapChain , &g_pd3dDevice , &featureLevel , &g_pd3dDeviceContext );
    if ( res != S_OK )
        return false;

    CreateRenderTarget ( );

    return true;
}

void Renderer::Cleanup ( )
{
    CleanupRenderTarget ( );
    if ( g_pSwapChain ) { g_pSwapChain->Release ( ); g_pSwapChain = nullptr; }
    if ( g_pd3dDeviceContext ) { g_pd3dDeviceContext->Release ( ); g_pd3dDeviceContext = nullptr; }
    if ( g_pd3dDevice ) { g_pd3dDevice->Release ( ); g_pd3dDevice = nullptr; }
}

void Renderer::BeginFrame ( )
{
    const float clear_color_rgba [ 4 ] = { 0.1f, 0.12f, 0.13f, 1.0f };
    g_pd3dDeviceContext->OMSetRenderTargets ( 1 , &g_mainRenderTargetView , NULL );
    g_pd3dDeviceContext->ClearRenderTargetView ( g_mainRenderTargetView , clear_color_rgba );
}

void Renderer::EndFrame ( )
{
    g_pSwapChain->Present ( 1 , 0 );
}

void Renderer::CreateRenderTarget ( )
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer ( 0 , IID_PPV_ARGS ( &pBackBuffer ) );
    if ( pBackBuffer )
    {
        g_pd3dDevice->CreateRenderTargetView ( pBackBuffer , NULL , &g_mainRenderTargetView );
        pBackBuffer->Release ( );
    }
}

void Renderer::CleanupRenderTarget ( )
{
    if ( g_mainRenderTargetView )
    {
        g_mainRenderTargetView->Release ( );
        g_mainRenderTargetView = nullptr;
    }
}