#pragma once
#include "globals.h"
#include <vector>

struct Vertex
{
    float x , y , z;    
	DWORD color;    // format -> DXGI_FORMAT_R8G8B8A8_UNORM
};

struct Color
{
    float r , g , b , a;
    Color ( float r , float g , float b , float a = 1.0f ) : r ( r ) , g ( g ) , b ( b ) , a ( a ) { }

    operator DWORD( ) const
    {
        DWORD dwR = static_cast< DWORD >( r * 255.0f );
        DWORD dwG = static_cast< DWORD >( g * 255.0f );
        DWORD dwB = static_cast< DWORD >( b * 255.0f );
        DWORD dwA = static_cast< DWORD >( a * 255.0f );
        return ( dwA << 24 ) | ( dwR << 16 ) | ( dwG << 8 ) | dwB;
    }
};

namespace Draw
{
    bool Initialize ( );
    void Shutdown ( );

    void Begin ( );
    void End ( );

    void AddFilledRect ( float x , float y , float w , float h , Color color );
    void AddRect ( float x , float y , float w , float h , Color color , float thickness = 1.0f );
}