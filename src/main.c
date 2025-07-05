#include <stdio.h>
#include <math.h>
#include <nanowin.h>

static uint8_t atlas_buffer[512 * 512];
static nkFont_t font;

float pointerX = 0.0f;
float pointerY = 0.0f;

void PointerMoveCallback(nkWindow_t *window, float x, float y)
{
    pointerX = x;
    pointerY = y;
}   

void WindowResizeCallback(nkWindow_t *window, float width, float height)
{
    printf("Window '%s' resized to %.2f x %.2f\n", window->Title, width, height);
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

    printf("Performance counter done\n");

    LARGE_INTEGER start, end, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    nkDraw_Begin(&window->DrawContext, window->Width, window->Height);
    nkDraw_SetColor(&window->DrawContext, (nkVector4_t){0.0f, 0.0f, 0.0f, 1.0f});

    
    float width = 100.0f;
    float height = 20.0f;

    size_t renderCount = 0;

    for (float x = 0.0f; x < window->Width; x += width)
    {
        for (float y = 0.0f; y < window->Height; y += height)
        {
            // Draw a rectangle
            nkDraw_SetColor(&window->DrawContext, (nkVector4_t){0.9f, 0.9f, 0.9f, 1.0f}); // Light grey
            nkDraw_Rect(&window->DrawContext, x + 2.5f, y + 2.5f, width - 5.0f, height - 5.0f);
            renderCount++;

            nkDraw_SetColor(&window->DrawContext, (nkVector4_t){0.0f, 0.0f, 0.0f, 1.0f});
            nkDraw_Text(&window->DrawContext, &font, title, x, y + 10.0f);
            renderCount++;
        }
    }

    QueryPerformanceCounter(&end);
    printf("Performance counter done\n");

    nkDraw_End(&window->DrawContext);

    double elapsedMicroseconds = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;
    printf("Drawing took %.2f microseconds for %zu labels\n", elapsedMicroseconds, renderCount);

}   

nkWindow_t window = {0};


int main()
{
    // Print a message to indicate the program is running
    printf("Hello, NanoKit!\n");


    window.ResizeCallback = WindowResizeCallback;
    window.CodepointInputCallback = WindowCodepointInputCallback;
    window.DrawCallback = WindowDrawCallback;
    window.PointerMoveCallback = PointerMoveCallback;
    nkWindow_Create(&window, "My Window", 800, 600);

    nkFont_Load(&font, "build/Roboto-Regular.ttf", 16.0f, atlas_buffer, 512, 512);

    while (nkWindow_PollEvents())
    {
        
        nkWindow_RequestRedraw(&window); // Request a redraw for the window
    }

    return 0;

}