#include "globals.h"
#include "window.h"
#include "renderer.h"
#include "draw.h"
#include "input.h"
#include "font.h"
#include "gui.h"

static int window_x = 50 , window_y = 50;

static int active_tab = 1; 

static bool players_box = true;
static bool players_skeleton = false;
static bool resources_stone = true;

static bool misc_crosshair = true;

int WINAPI WinMain ( HINSTANCE hInstance , HINSTANCE hPrevInstance , PSTR lpCmdLine , int nCmdShow )
{
    if ( !Window::Create ( L"DirectX 11 Sandbox" ) )
    {
        Window::Destroy ( );
        return 1;
    }

    if ( !Renderer::Initialize ( Window::GetHwnd ( ) ) )
    {
        Renderer::Cleanup ( );
        Window::Destroy ( );
        return 1;
    }

    if ( !Draw::Initialize ( ) )
    {
        Draw::Shutdown ( );
        Renderer::Cleanup ( );
        Window::Destroy ( );
        return 1;
    }

    if ( !Font::Initialize ( ) )
    {
        Font::Shutdown ( );
        Draw::Shutdown ( );
        Renderer::Cleanup ( );
        Window::Destroy ( );
        return 1; 
    }

    bool done = false;
    while ( !done )
    {
        MSG msg;
        while ( PeekMessage ( &msg , NULL , 0U , 0U , PM_REMOVE ) )
        {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
            if ( msg.message == WM_QUIT )
                done = true;
        }
        if ( done ) break;

        RECT rect;
        GetClientRect ( Window::GetHwnd ( ) , &rect );
        static int width = 0 , height = 0;
        if ( g_pSwapChain && ( width != ( rect.right - rect.left ) || height != ( rect.bottom - rect.top ) ) )
        {
            width = rect.right - rect.left; height = rect.bottom - rect.top;
            Renderer::CleanupRenderTarget ( );
            g_pSwapChain->ResizeBuffers ( 0 , width , height , DXGI_FORMAT_UNKNOWN , 0 );
            Renderer::CreateRenderTarget ( );
        }

        Input::BeginFrame ( );
        Renderer::BeginFrame ( ); 

        GUI::BeginFrame ( );

        Draw::Begin ( );
        {
            GUI::BeginWindow ( L"Vadios" , window_x , window_y , 550 , 400 );

            {
                GUI::TabButton ( L"Aimbot" , 0 , active_tab );
                GUI::TabButton ( L"Visuals" , 1 , active_tab );
                GUI::TabButton ( L"Misc" , 2 , active_tab );
                GUI::TabButton ( L"Exploits" , 3 , active_tab );
            }
            GUI::Separator ( );

            if ( active_tab == 1 ) 
            {
                GUI::Checkbox ( L"Box" , &players_box );
                GUI::Checkbox ( L"Skeleton" , &players_skeleton );
                GUI::Checkbox ( L"Stone" , &resources_stone );
            }
            else if ( active_tab == 2 )
            {
                GUI::Checkbox ( L"Crosshair" , &misc_crosshair );
            }

            GUI::EndWindow ( );
        }
        Draw::End ( );

        GUI::RenderText ( );

        Renderer::EndFrame ( );
        Input::EndFrame ( );
    }

    Font::Shutdown ( );
    Draw::Shutdown ( );
    Renderer::Cleanup ( );
    Window::Destroy ( );

    return 0;
}