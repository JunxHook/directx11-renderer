#include "font.h"
#include "globals.h"

#include "fw1-fontwrapper/FW1FontWrapper.h"

namespace
{
    IFW1FontWrapper* g_pFontWrapper = nullptr;
}

bool Font::Initialize ( )
{
    IFW1Factory* pFW1Factory = nullptr;
    HRESULT hr = FW1CreateFactory ( FW1_VERSION , &pFW1Factory );
    if ( FAILED ( hr ) ) {
        return false;
    }

    hr = pFW1Factory->CreateFontWrapper ( g_pd3dDevice , L"Arial" , &g_pFontWrapper );
    if ( FAILED ( hr ) ) {
        pFW1Factory->Release ( );
        return false;
    }

    pFW1Factory->Release ( );
    return true;
}

void Font::Shutdown ( )
{
    if ( g_pFontWrapper )
    {
        g_pFontWrapper->Release ( );
        g_pFontWrapper = nullptr;
    }
}

void Font::DrawTextt ( float x , float y , const std::wstring& text , float font_size , FColor color )
{
    if ( !g_pFontWrapper )
        return;

    const float offset = 1.0f;

    FColor black = 0xFF000000;

    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x - offset , y , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x + offset , y , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x , y - offset , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x , y + offset , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x - offset , y - offset , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x + offset , y - offset , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x - offset , y + offset , black , FW1_RESTORESTATE );
    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x + offset , y + offset , black , FW1_RESTORESTATE );

    g_pFontWrapper->DrawString ( g_pd3dDeviceContext , text.c_str ( ) , font_size , x , y , color , FW1_RESTORESTATE );
}
