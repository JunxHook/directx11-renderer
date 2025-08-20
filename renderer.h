#pragma once

#include "globals.h"

namespace Renderer
{
    bool Initialize ( HWND hwnd );
    void Cleanup ( );

    void BeginFrame ( );
    void EndFrame ( );

    void CreateRenderTarget ( );
    void CleanupRenderTarget ( );
}