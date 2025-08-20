# Lightweight DirectX 11 Rendering Framework

This project is a lightweight, from-scratch rendering framework built using C++ and DirectX 11. It provides a solid foundation for developing graphical applications, including tools, 2D games, and visualizations. The framework is designed with a modular approach, separating concerns like window management, rendering, input handling, and UI into distinct components.

While its immediate use is demonstrated as an immediate-mode GUI renderer, its underlying systems are versatile and can be extended for a wide range of graphical tasks.

## Features

-   **DirectX 11 Backend:** A clean and modern rendering pipeline leveraging the power of Direct3D 11.
-   **Window Management:** A simple wrapper around the Win32 API for creating, managing, and handling window events.
-   **2D Primitive Drawing:** A custom drawing module with its own vertex/pixel shaders to render basic shapes like filled rectangles and outlines, which form the building blocks for any 2D graphics.
-   **High-Quality Text Rendering:** Integrates the popular **FW1 FontWrapper** library to render crisp, anti-aliased text with a drop-shadow effect for excellent readability.
-   **Stateful Input System:** Captures and processes keyboard and mouse input, distinguishing between key-down and key-press events.
-   **Immediate-Mode GUI:** A simple, dependency-free immediate-mode GUI system built on top of the drawing, font, and input modules. It supports draggable windows, tabs, and checkboxes with smooth animations.

## Core Components

The framework is broken down into several core modules, each with a specific responsibility.

### 1. Window (`window.h`, `window.cpp`)

This module handles the creation and management of the application's main window.
-   `Window::Create()`: Initializes a `WNDCLASSEXW` and calls `CreateWindowW` to create a standard Win32 window.
-   `Window::WndProc()`: The core message procedure for the window. It processes essential messages like `WM_DESTROY` and forwards input events (`WM_KEYDOWN`, etc.) to the `Input` module.
-   `Window::GetHwnd()`: A simple getter to provide the window handle (`HWND`) to other modules, primarily the renderer.

### 2. Renderer (`renderer.h`, `renderer.cpp`)

This is the heart of the graphics engine. It wraps the core DirectX 11 objects.
-   `Renderer::Initialize()`: Sets up the `ID3D11Device`, `ID3D11DeviceContext`, and `IDXGISwapChain`. It configures the swap chain for standard double-buffered rendering.
-   `Renderer::CreateRenderTarget()`: Creates the render target view, which is bound to the swap chain's back buffer. This is what the application draws to.
-   `Renderer::BeginFrame()`: Prepares for a new frame by clearing the render target view with a solid color.
-   `Renderer::EndFrame()`: Presents the final rendered image to the screen by calling `Present()` on the swap chain.

### 3. Input (`input.h`, `input.cpp`)

A stateful input handler that provides a simple interface for polling mouse and keyboard status.
-   `Input::WndProcHandler()`: Called from the main `WndProc` to update the internal key state array based on Windows messages.
-   `Input::BeginFrame()` / `Input::EndFrame()`: These functions work together to track key states across frames. `EndFrame` copies the current state to a "previous state" buffer, allowing for the detection of single key presses.
-   `IsKeyDown()`: Checks if a key is currently held down.
-   `IsKeyPressed()`: Checks if a key was just pressed in the current frame.

### 4. Draw (`draw.h`, `draw.cpp`)

A low-level 2D drawing library that renders vertices directly to the screen. The GUI and other visual elements are built using this module.
-   **Custom Shaders:** A single, hardcoded vertex and pixel shader is compiled at runtime using `D3DCompile`. It's a simple passthrough shader for rendering 2D vertices with color.
-   **Vertex Buffer:** Manages a dynamic `ID3D11Buffer` that stores a list of `Vertex` structs. Primitives are added to a `std::vector` during the frame.
-   `Draw::Initialize()`: Compiles the shaders, creates the input layout, and initializes all necessary D3D11 objects like blend states, rasterizer states, and buffers.
-   `Draw::Begin()`: Clears the vertex buffer for the new frame.
-   `Draw::AddFilledRect()`: Triangulates a rectangle and adds its 6 vertices to the vertex list.
-   `Draw::End()`: Maps the vertex buffer, copies the vertex data from the vector to the GPU, sets the D3D11 pipeline state (shaders, input layout, etc.), and issues a `Draw()` call to render all queued geometry for the frame.

### 5. Font (`font.h`, `font.cpp`)

This module abstracts text rendering by integrating the **FW1 FontWrapper** library.
-   **FW1 FontWrapper Integration:** This library is a powerful wrapper around `FW1CreateFactory` that simplifies high-quality text rendering in D3D11. It handles all the complexities of creating font atlases and rendering glyphs efficiently.
-   `Font::Initialize()`: Creates an `IFW1Factory` and an `IFW1FontWrapper` object for a specified font (e.g., "Arial").
-   `Font::DrawTextt()`: Renders a given string at a specified position and size. This implementation adds a simple but effective drop-shadow by drawing the text multiple times in black with slight offsets before drawing the final colored text on top.

### 6. GUI (`gui.h`, `gui.cpp`)

A simple immediate-mode GUI system that demonstrates the capabilities of the underlying modules.
-   **Immediate Mode:** The GUI is drawn every frame from scratch based on function calls. There is no retained state for UI elements between frames, which simplifies the design.
-   `GUI::BeginWindow()` / `GUI::EndWindow()`: Defines a window container. The `BeginWindow` function handles dragging logic by checking the mouse position and input state.
-   **Widgets:** Provides functions for common UI elements like `Checkbox` and `TabButton`. These functions handle their own logic for input, state changes, and drawing. For example, `Checkbox` includes a smooth animation for its checked state.
-   **Deferred Text Rendering:** To ensure text is always drawn on top of other UI elements, text rendering commands are buffered into a `std::vector` during the widget calls. The `GUI::RenderText()` function is called at the end of the UI composition to submit all text draw calls.

## Project Architecture & Flow

The application follows a standard game loop structure.

1.  **Initialization:**
    -   `Window::Create()` is called to get a valid `HWND`.
    -   `Renderer::Initialize()`, `Draw::Initialize()`, and `Font::Initialize()` are called to set up the DirectX 11 pipeline and drawing utilities.

2.  **Main Loop:**
    -   The standard Win32 message loop (`PeekMessage`, `TranslateMessage`, `DispatchMessage`) processes events. The loop exits when `WM_QUIT` is received.
    -   **Input Update:** `Input::BeginFrame()` polls the current mouse position.
    -   **Start Rendering:** `Renderer::BeginFrame()` clears the screen. `Draw::Begin()` and `GUI::BeginFrame()` clear their respective buffers.
    -   **UI & Scene Composition:** The application calls `GUI` functions to define the user interface. These functions internally call `Draw` functions to add vertices to a list for rendering.
    -   **Render Primitives:** `Draw::End()` sends all the batched 2D geometry to the GPU for rendering.
    -   **Render Text:** `GUI::RenderText()` is called to draw all the text on top of the geometry.
    -   **Present Frame:** `Renderer::EndFrame()` presents the finished frame to the screen.
    -   **End Input Frame:** `Input::EndFrame()` saves the current input state to prepare for the next frame's "key pressed" detection.

3.  **Shutdown:**
    -   After the loop terminates, the `Shutdown()` and `Cleanup()` functions for each module are called in reverse order of initialization to release all COM objects and allocated resources.

## Beyond a GUI: Potential Applications

While this project is presented as a GUI renderer, its core components are highly versatile and provide a strong foundation for many other types of graphical applications:

-   **2D Game Development:** The `Draw` and `Input` modules are all you need to start building a 2D game. The renderer is efficient enough to handle thousands of sprites.
-   **Data Visualization & Tools:** Create custom tools for debugging, data analysis, or content creation where a fast and simple graphical interface is required.
-   **Educational Tool:** The codebase serves as an excellent, minimalistic example of how to set up a DirectX 11 renderer from scratch, making it a great resource for learning graphics programming.
-   **Base for a 3D Renderer:** The `Window` and `Renderer` setup is the first step for any 3D engine. The existing framework can be extended to handle 3D models, lighting, and camera systems.
