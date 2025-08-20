// window.cpp
#include "window.h"
#include "input.h"

namespace
{
    HWND g_hwnd = nullptr;
    WNDCLASSEXW g_wc = {};
}

bool Window::Create ( const wchar_t* window_name )
{
    g_wc.cbSize = sizeof ( g_wc );
    g_wc.style = CS_CLASSDC;
    g_wc.lpfnWndProc = WndProc;
    g_wc.hInstance = GetModuleHandle ( NULL );
    g_wc.lpszClassName = L"DX11 Sandbox Class";

    if ( !RegisterClassExW ( &g_wc ) )
        return false;

    g_hwnd = CreateWindowW ( g_wc.lpszClassName , window_name , WS_OVERLAPPEDWINDOW , 100 , 100 , WINDOW_WIDTH , WINDOW_HEIGHT , NULL , NULL , g_wc.hInstance , NULL );

    if ( !g_hwnd )
    {
        Destroy ( );
        return false;
    }

    ShowWindow ( g_hwnd , SW_SHOWDEFAULT );
    UpdateWindow ( g_hwnd );

    return true;
}

void Window::Destroy ( )
{
    if ( g_hwnd )
    {
        DestroyWindow ( g_hwnd );
        g_hwnd = nullptr;
    }

    if ( g_wc.lpszClassName )
    {
        UnregisterClassW ( g_wc.lpszClassName , g_wc.hInstance );
        g_wc.lpszClassName = nullptr;
    }
}

HWND Window::GetHwnd ( )
{
    return g_hwnd;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler ( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

LRESULT WINAPI Window::WndProc ( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam )
{
    Input::WndProcHandler ( msg , wParam , lParam );

    switch ( msg )
    {
    case WM_SIZE:
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) 
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage ( 0 );
        return 0;
    }
    return DefWindowProcW ( hWnd , msg , wParam , lParam );
}