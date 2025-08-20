// input.h
#pragma once
#include <Windows.h>

struct Point { int x , y; };

namespace Input
{
    void BeginFrame ( );
    void EndFrame ( );

    void WndProcHandler ( UINT msg , WPARAM wParam , LPARAM lParam );

    Point GetMousePos ( );
    bool IsKeyDown ( int key );
    bool IsKeyPressed ( int key ); 
}