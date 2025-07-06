#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <nanowin.h>

#include <views/nkdockview/nkdockview.h>

static uint8_t atlas_buffer[512 * 512];
static nkFont_t font;

float pointerX = 0.0f;
float pointerY = 0.0f;

static LARGE_INTEGER start, end, lastStart, frequency = {0};
static float fps = 0.0f;

static char fpsText[64];

extern const uint8_t NKFonts_fonts_Roboto_Regular_ttf[];
extern const size_t NKFonts_fonts_Roboto_Regular_ttf_size;

void PointerMoveCallback(nkWindow_t *window, float x, float y)
{
    pointerX = x;
    pointerY = y;

    //nkWindow_RequestRedraw(window);
}   

void WindowResizeCallback(nkWindow_t *window, float width, float height)
{
    //printf("Window '%s' resized to %.2f x %.2f\n", window->Title, width, height);

    QueryPerformanceCounter(&start);

    nkWindow_LayoutViews(window); // Layout the views in the window after resizing

    QueryPerformanceCounter(&end);

    double elapsedMs = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Layout took %.3f ms\n", elapsedMs);
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

    QueryPerformanceCounter(&start);

    nkDraw_Begin(&window->drawContext, window->width, window->height);

    nkWindow_RedrawViews(window); // Redraw the views in the window


    #if 0
    nkDraw_SetColor(&window->drawContext, (nkVector4_t){0.0f, 0.0f, 0.0f, 1.0f});

    float width = 100.0f;
    float height = 20.0f;

    size_t renderCount = 0;

    for (float x = 0.0f; x < window->width; x += width)
    {
        for (float y = 0.0f; y < window->height; y += height)
        {
            // Draw a rectangle
            nkDraw_SetColor(&window->drawContext, NK_COLOR_MAGENTA); // Light grey
            nkDraw_Rect(&window->drawContext, x + 2.5f, y + 2.5f, width - 5.0f, height - 5.0f);
            renderCount++;

            nkDraw_SetColor(&window->drawContext, NK_COLOR_WHITE);
            nkDraw_Text(&window->drawContext, &font, title, x, y + 10.0f);
            renderCount++;
        }
    }
    #endif

    nkDraw_End(&window->drawContext);

    QueryPerformanceCounter(&end);

    double elapsedMs = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Drawing took %.2f ms\n", elapsedMs);

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


}   

nkWindow_t window = {0};

nkView_t views[10000] = {0}; // Array to hold views


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

    nkDockView_t dockView  = {0};
    nkDockView_Create(&dockView);

    window.rootView = (nkView_t *)&dockView.view; // Set the root view to the dock view
    
    dockView.view.backgroundColor = NK_COLOR_LIGHT_GRAY; // Set the background color of the dock view

     /* create many dockpanel views */
    for (int i = 0; i < 10000; i++)
    {
        nkView_t* view = &views[i];
        nkView_Create(view, "DockPanel View");

        nkColor_t randomColors[] = {NK_COLOR_RED, NK_COLOR_GREEN, NK_COLOR_BLUE, NK_COLOR_YELLOW, NK_COLOR_CYAN};
        view->backgroundColor = randomColors[rand() % 5];

        view->sizeRequest = (nkSize_t){0.1, 0.1};
        
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

        nkView_AddChildView(&dockView.view, view);
    }



    //nkFont_Load(&font, "build/Roboto-Regular.ttf", 16.0f, atlas_buffer, 512, 512);
    nkFont_LoadFromMemory(&font, NKFonts_fonts_Roboto_Regular_ttf, NKFonts_fonts_Roboto_Regular_ttf_size, 16.0f, atlas_buffer, 512, 512);

    QueryPerformanceFrequency(&frequency);


    while (nkWindow_PollEvents())
    {
        //nkWindow_RequestRedraw(&window); // Request a redraw for the window
    }

    return 0;

}