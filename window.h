// window.h
#pragma once

#include "globals.h"

namespace Window
{
    bool Create ( const wchar_t* window_name );
    void Destroy ( );
    HWND GetHwnd ( );

    LRESULT WINAPI WndProc ( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );
}