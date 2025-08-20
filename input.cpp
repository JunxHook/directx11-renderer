#include "input.h"
#include "globals.h" 

namespace
{
    Point g_mousePos;
    bool g_keyState [ 256 ] = {};
    bool g_prevKeyState [ 256 ] = {};
}

void Input::BeginFrame ( )
{
    POINT p;
    if ( GetCursorPos ( &p ) )
    {
        HWND hwnd = FindWindowW ( L"DX11 Sandbox Class" , nullptr );
        if ( hwnd )
        {
            if ( ScreenToClient ( hwnd , &p ) )
            {
                g_mousePos = { p.x, p.y };
            }
        }
    }
}

void Input::EndFrame ( )
{
    memcpy ( g_prevKeyState , g_keyState , sizeof ( g_keyState ) );
}

void Input::WndProcHandler ( UINT msg , WPARAM wParam , LPARAM lParam )
{
    switch ( msg )
    {
    case WM_KEYDOWN:
        if ( wParam < 256 ) g_keyState [ wParam ] = true;
        break;
    case WM_KEYUP:
        if ( wParam < 256 ) g_keyState [ wParam ] = false;
        break;
    case WM_LBUTTONDOWN:
        g_keyState [ VK_LBUTTON ] = true;
        break;
    case WM_LBUTTONUP:
        g_keyState [ VK_LBUTTON ] = false;
        break;
    case WM_RBUTTONDOWN:
        g_keyState [ VK_RBUTTON ] = true;
        break;
    case WM_RBUTTONUP:
        g_keyState [ VK_RBUTTON ] = false;
        break;
    }
}

Point Input::GetMousePos ( )
{
    return g_mousePos;
}

bool Input::IsKeyDown ( int key )
{
    return g_keyState [ key ];
}

bool Input::IsKeyPressed ( int key )
{
    return !g_prevKeyState [ key ] && g_keyState [ key ];
}
