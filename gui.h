// gui.h
#pragma once
#include <string>
#include <vector>

namespace GUI
{
    void BeginFrame ( );
    void BeginWindow ( const wchar_t* title , int& x , int& y , int w , int h );
    void EndWindow ( );
    void RenderText ( );
    bool TabButton ( const wchar_t* label , int index , int& active_tab );
    void Separator ( );
    void Checkbox ( const wchar_t* label , bool* value );
}