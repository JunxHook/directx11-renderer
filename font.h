#pragma once
#include <string>

using FColor = unsigned int;

namespace Font
{
    bool Initialize ( );
    void Shutdown ( );

    void DrawTextt ( float x , float y , const std::wstring& text , float font_size , FColor color );
}