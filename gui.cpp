#include "gui.h"
#include "draw.h"
#include "font.h"
#include "input.h"
#include <map>
namespace
{
    struct DeferredText
    {
        Point position;
        std::wstring text;
        FColor color;
        float size;
    };

    struct GuiContext
    {
        Point cursor_pos;
        int window_x , window_y , window_w , window_h;
        const int padding = 10;
        const int item_spacing = 8;
        const int title_bar_height = 25;

        bool is_dragging = false;
        Point drag_offset;        

        std::vector<DeferredText> text_buffer;

    } static g_ctx;
}

void GUI::BeginFrame ( )
{
    g_ctx.text_buffer.clear ( );
}

bool GUI::TabButton ( const wchar_t* label , int index , int& active_tab )
{
    Point mousePos = Input::GetMousePos ( );
    int x = g_ctx.cursor_pos.x;
    int y = g_ctx.cursor_pos.y;

    const int w = 80;
    const int h = 25;

    bool is_active = ( index == active_tab );
    bool is_hovered = ( mousePos.x >= x && mousePos.x <= x + w &&
        mousePos.y >= y && mousePos.y <= y + h );

    bool clicked = false;
    if ( is_hovered && Input::IsKeyPressed ( VK_LBUTTON ) )
    {
        active_tab = index;
        clicked = true;
    }

    Color button_color = is_active ? Color ( 0.17f , 0.17f , 0.17f ) : 
        is_hovered ? Color ( 0.2f , 0.2f , 0.2f ) : 
        Color ( 0.15f , 0.15f , 0.15f );            

    Draw::AddFilledRect ( ( float ) x , ( float ) y , ( float ) w , ( float ) h , button_color );

    if ( is_active )
    {
        Draw::AddFilledRect ( ( float ) x , ( float ) y , ( float ) w , 2.0f , { 0.9f, 0.5f, 0.1f } );
    }

    g_ctx.text_buffer.push_back ( {
        { x + w / 2 - 15, y + 5 }, 
        label,
        0xFFDDDDDD,
        16.0f
        } );

    g_ctx.cursor_pos.x += w + g_ctx.item_spacing / 2;

    return clicked;
}

void GUI::Separator ( )
{
    int x = g_ctx.window_x;
    int y = g_ctx.cursor_pos.y;
    int w = g_ctx.window_w;

    Draw::AddFilledRect ( ( float ) x , ( float ) y , ( float ) w , 1.0f , { 0.2f, 0.2f, 0.2f } );

    g_ctx.cursor_pos.y += g_ctx.item_spacing;
}

void GUI::BeginWindow ( const wchar_t* title , int& x , int& y , int w , int h )
{
    Point mousePos = Input::GetMousePos ( );


    bool is_hovering_title_bar = ( mousePos.x >= x && mousePos.x <= x + w &&
        mousePos.y >= y && mousePos.y <= y + g_ctx.title_bar_height );

    if ( is_hovering_title_bar && Input::IsKeyPressed ( VK_LBUTTON ) )
    {
        g_ctx.is_dragging = true;
        g_ctx.drag_offset.x = mousePos.x - x;
        g_ctx.drag_offset.y = mousePos.y - y;
    }

    if ( !Input::IsKeyDown ( VK_LBUTTON ) )
    {
        g_ctx.is_dragging = false;
    }

    if ( g_ctx.is_dragging )
    {
        x = mousePos.x - g_ctx.drag_offset.x;
        y = mousePos.y - g_ctx.drag_offset.y;
    }

    g_ctx.window_x = x;
    g_ctx.window_y = y;
    g_ctx.window_w = w;
    g_ctx.window_h = h;

    Draw::AddFilledRect ( ( float ) x , ( float ) y , ( float ) w , ( float ) h , { 0.1f, 0.1f, 0.1f, 0.95f } );
    Draw::AddFilledRect ( ( float ) x , ( float ) y , ( float ) w , ( float ) g_ctx.title_bar_height , { 0.15f, 0.15f, 0.15f, 0.95f } );

    g_ctx.text_buffer.push_back ( {
        { x + g_ctx.padding, y + 5 },
        title,
        0xFFDDDDDD,
        16.0f
        } );

    g_ctx.cursor_pos.x = x + g_ctx.padding;
    g_ctx.cursor_pos.y = y + g_ctx.title_bar_height + g_ctx.padding;
}

void GUI::EndWindow ( )
{
}

void GUI::RenderText ( )
{
    for ( const auto& text_item : g_ctx.text_buffer )
    {
        Font::DrawTextt (
            ( float ) text_item.position.x ,
            ( float ) text_item.position.y ,
            text_item.text ,
            text_item.size ,
            text_item.color
        );
    }
}


void GUI::Checkbox ( const wchar_t* label , bool* value )
{
    int x = g_ctx.cursor_pos.x;
    int y = g_ctx.cursor_pos.y;

    const int size = 16;
    const int spacing = 5;
    Point mousePos = Input::GetMousePos ( );

    bool isHovered = ( mousePos.x >= x && mousePos.x <= x + size + 100 &&
        mousePos.y >= y && mousePos.y <= y + size );

    if ( isHovered && Input::IsKeyPressed ( VK_LBUTTON ) )
        *value = !( *value );

    static std::map<const wchar_t* , float> animProgress;
    if ( !animProgress.count ( label ) )
        animProgress [ label ] = *value ? 1.0f : 0.0f;

    float speed = 0.15f;
    if ( *value )
        animProgress [ label ] += ( 1.0f - animProgress [ label ] ) * speed;
    else
        animProgress [ label ] -= animProgress [ label ] * speed;

    Draw::AddRect ( ( float ) x , ( float ) y , ( float ) size , ( float ) size ,
        isHovered ? Color { 1,1,1 } : Color { 0.8f,0.8f,0.8f } );

    if ( animProgress [ label ] > 0.01f )
    {
        float pad = 3.0f * ( 1.0f - animProgress [ label ] );
        Draw::AddFilledRect (
            ( float ) x + pad ,
            ( float ) y + pad ,
            ( float ) size - 2 * pad ,
            ( float ) size - 2 * pad ,
            { 0.2f, 0.7f, 0.3f }
        );
    }

    g_ctx.text_buffer.push_back ( {
        { x + size + spacing, y + 1 },
        label,
        0xFFDDDDDD,
        16.0f
        } );

    g_ctx.cursor_pos.y += size + g_ctx.item_spacing;
}
