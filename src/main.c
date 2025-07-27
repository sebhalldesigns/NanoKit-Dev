#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <nanowin.h>

#include <views/nkdockview/nkdockview.h>

static uint8_t atlas_buffer[512 * 512];
static nkFont_t font;

float pointerX = 0.0f;
float pointerY = 0.0f;

#if _WIN32
static LARGE_INTEGER start, end, lastStart, frequency = {0};
#endif 

static float fps = 0.0f;

static char fpsText[64];

extern const uint8_t NKFonts_fonts_Roboto_Regular_ttf[];
extern const size_t NKFonts_fonts_Roboto_Regular_ttf_size;

void PointerMoveCallback(nkWindow_t *window, float x, float y)
{
    pointerX = x;
    pointerY = y;

    nkWindow_RequestRedraw(window);
}   

void WindowResizeCallback(nkWindow_t *window, float width, float height)
{
    //printf("Window '%s' resized to %.2f x %.2f\n", window->title, width, height);

    #if _WIN32
        QueryPerformanceCounter(&start); 
    #endif

     #if __EMSCRIPTEN__
        double startTime = emscripten_get_now();
    #endif

    nkWindow_LayoutViews(window); // Layout the views in the window after resizing

    #if _WIN32
        QueryPerformanceCounter(&end);
        double elapsedMs = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
        printf("Layout took %.2f ms\n", elapsedMs);
    #endif

     #if __EMSCRIPTEN__
        double endTime = emscripten_get_now();
        double elapsedMs = endTime - startTime;
        printf("Layout took %.2f ms\n", elapsedMs);
    #endif
}

void WindowCodepointInputCallback(nkWindow_t *window, uint32_t codepoint)
{
    char unicodeChar[5] = {0}; // Buffer for UTF-8 character

    // Convert codepoint to UTF-8
    if (codepoint < 0x80) 
    {
        unicodeChar[0] = (char)codepoint;
    } 
    else if (codepoint < 0x800) 
    {
        unicodeChar[0] = (char)((codepoint >> 6) | 0xC0);
        unicodeChar[1] = (char)((codepoint & 0x3F) | 0x80);
    } 
    else if (codepoint < 0x10000) 
    {
        unicodeChar[0] = (char)((codepoint >> 12) | 0xE0);
        unicodeChar[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        unicodeChar[2] = (char)((codepoint & 0x3F) | 0x80);
    } 
    else if (codepoint < 0x110000) 
    {
        unicodeChar[0] = (char)((codepoint >> 18) | 0xF0);
        unicodeChar[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        unicodeChar[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        unicodeChar[3] = (char)((codepoint & 0x3F) | 0x80);
    }

    printf("Codepoint input: 0x%x %s\n", codepoint, unicodeChar);

    nkWindow_RequestRedraw(window); // Request a redraw to update the window with the new input
}

void WindowDrawCallback(nkWindow_t *window)
{

    char title[256];
    snprintf(title, sizeof(title), "(%.1f, %.1f)", pointerX, pointerY);

    #if _WIN32
        QueryPerformanceCounter(&start);
    #endif

    #if __EMSCRIPTEN__
        double startTime = emscripten_get_now();
    #endif

    nkDraw_Begin(&window->drawContext, window->width, window->height);

    nkWindow_RedrawViews(window); // Redraw the views in the window

    nkDraw_End(&window->drawContext);

    #if _WIN32
        QueryPerformanceCounter(&end);

        double elapsedMs = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
        //printf("Drawing took %.2f ms\n", elapsedMs);

        /* calculate FPS */
        if (lastStart.QuadPart == 0)
        {
            lastStart = start;
        }

        double elapsedTime = (double)(start.QuadPart - lastStart.QuadPart) * 1000.0 / frequency.QuadPart; // in milliseconds
        if (elapsedTime > 0.0)
        {
            fps = 1000.0 / elapsedTime; // Calculate FPS
        }

        lastStart = start; // Update lastStart for the next frame

        static double timeAccumulator = 0.0;
        timeAccumulator += elapsedTime;
        

        if (timeAccumulator >= 200.0) // Update title at 5 Hz (200 ms intervals)
        {
            snprintf(fpsText, sizeof(fpsText), "My Window FPS: %.2f", fps);

            nkWindow_SetTitle(window, fpsText);
            timeAccumulator = 0.0; // Reset accumulator
        }

    #endif

    #if __EMSCRIPTEN__
        double endTime = emscripten_get_now();
        double elapsedMs = endTime - startTime;
        printf("Drawing took %.2f ms\n", elapsedMs);
    #endif

}   

void PointerMove(nkView_t *window, float x, float y)
{
    //printf("Pointer moved to (%.2f, %.2f)\n", x, y);
}

void HoverCallback(nkView_t *view, nkPointerHover_t hover)
{
    static nkColor_t prevColor;

    if (hover == HOVER_END)
    {
        view->backgroundColor = prevColor;
    }
    else if (hover == HOVER_BEGIN)
    {
        prevColor = view->backgroundColor; // Store previous color
        view->backgroundColor = NK_COLOR_ORANGE; // Change background color on hover
    }
}

void PointerActionCallback(nkView_t *view, nkPointerAction_t action, nkPointerEvent_t event, float x, float y)
{
    printf("Pointer action: %d, event: %d at (%.2f, %.2f)\n", action, event, x, y);

    static nkPoint_t actionOrigin = {0.0f, 0.0f};
    static nkPoint_t viewOrigin = {0.0f, 0.0f}; // Store the origin of the action

    switch (event)
    {
        case POINTER_EVENT_BEGIN:
        {
            actionOrigin = (nkPoint_t){x, y}; // Store the origin of the action
            viewOrigin = (nkPoint_t){view->frame.x, view->frame.y}; // Store the origin of the view
        } break;

        case POINTER_EVENT_DRAG:
        {
            // Calculate the new position based on the action origin and view origin
            float newX = x - actionOrigin.x + viewOrigin.x;
            float newY = y - actionOrigin.y + viewOrigin.y;

            // Update the view's frame position
            view->frame.x = newX;
            view->frame.y = newY;

            printf("Dragging view to (%.2f, %.2f)\n", newX, newY);
        } break;
    }
}

nkWindow_t window = {0};

nkView_t views[10000] = {0}; // Array to hold views

nkDockView_t dockView  = {0};

nkView_t newRootView = {0}; // New root view for testing
nkView_t secondView = {0}; // Second view for testing


/* IMPORTANT: ON WEB, MAKE SURE NOTHING IS LOCAL TO THE MAIN FUNCTION AS IT IS TEMPORARY */
//DWORD WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main()
{
    // Print a message to indicate the program is running
    printf("Hello, NanoKit!\n");


    window.resizeCallback = (nkWindowResizeCallback_t)WindowResizeCallback;
    window.codepointInputCallback = (nkWindowCodepointInputCallback_t)WindowCodepointInputCallback;
    window.drawCallback = (nkWindowDrawCallback_t)WindowDrawCallback;
    window.pointerMoveCallback = (nkWindowPointerMoveCallback_t)PointerMoveCallback;
    nkWindow_Create(&window, "My Window", 800, 600);

    window.backgroundColor = NK_COLOR_GREEN;

    
    nkDockView_Create(&dockView);

    window.rootView = (nkView_t *)&dockView.view; // Set the root view to the dock view
    
    dockView.view.backgroundColor = NK_COLOR_LIGHT_GRAY; // Set the background color of the dock view

    #if 0

     /* create many dockpanel views */
    for (int i = 0; i < 10000; i++)
    {
        nkView_t* view = &views[i];
        nkView_Create(view, "DockPanel View");

        nkColor_t randomColors[] = {NK_COLOR_RED, NK_COLOR_GREEN, NK_COLOR_BLUE, NK_COLOR_YELLOW, NK_COLOR_CYAN};
        view->backgroundColor = NK_COLOR_GREEN;//randomColors[rand() % 5];

        view->sizeRequest = (nkSize_t){1.0, 1.0};
        view->arrangeCallback = NULL;
        
        switch (i % 4)
        {
            case 0:
                view->dockPosition = DOCK_POSITION_TOP;
                break;
            case 1:
                view->dockPosition = DOCK_POSITION_BOTTOM;
                break;
            case 2:
                view->dockPosition = DOCK_POSITION_LEFT;
                break;
            case 3:
                view->dockPosition = DOCK_POSITION_RIGHT;
                break;
        }

        view->capturePointerHover = true; // Enable pointer hover capture
        view->pointerHoverCallback = HoverCallback; // Set the hover callback

        nkView_AddChildView(&dockView.view, view);
    }

    size_t lastView = sizeof views / sizeof(views[0]) - 1;

    views[lastView].pointerMovementCallback = PointerMove; // Set the pointer movement callback for the last view
    views[lastView].capturePointerMovement = true; // Enable pointer movement capture for the last view

    #endif

    nkView_Create(&newRootView, "New Root View");
    newRootView.backgroundColor = NK_COLOR_GREEN;
    window.rootView = &newRootView; // Set the root view to the second view



    nkView_Create(&secondView, "Second View");
    secondView.backgroundColor = NK_COLOR_BLUE;
    secondView.frame = (nkRect_t){10.0f, 10.0f, 200.0f, 200.0f}; // Set the frame of the second view
    secondView.capturePointerHover = true; // Enable pointer hover capture for the second view
    secondView.pointerHoverCallback = HoverCallback; // Set the hover callback for the second view
    secondView.capturePointerAction = true; // Enable pointer action capture for the second view
    secondView.pointerActionCallback = (PointerActionCallback_t)PointerActionCallback; // Set the pointer action callback for the

    printf("Pointer action callback: %p\n", secondView.pointerActionCallback);

    printf("SECOND VIEW: %p\n", &secondView);

    nkView_AddChildView(&newRootView, &secondView); // Add the second view to the new root view


    //views[lastView].pointerHoverCallback = HoverCallback; // Set the hover callback for the last view
    //views[lastView].capturePointerHover = true; // Enable pointer hover capture for the last


    //nkFont_Load(&font, "build/Roboto-Regular.ttf", 16.0f, atlas_buffer, 512, 512);
    nkFont_LoadFromMemory(&font, NKFonts_fonts_Roboto_Regular_ttf, NKFonts_fonts_Roboto_Regular_ttf_size, 16.0f, atlas_buffer, 512, 512);

    #if _WIN32
        QueryPerformanceFrequency(&frequency);
    #endif

    while (nkWindow_PollEvents())
    {
        //nkWindow_RequestRedraw(&window); // Request a redraw for the window
    }

    return 0;
}