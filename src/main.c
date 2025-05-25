#include <stdio.h>

#include <windows.h>

#include <Window.xml.h>
#include <SecondWindow.xml.h>

#include <stdlib.h>


#include "../NanoKit/lib/kit/NanoKit.h"

void AppLaunched()
{
    printf("App launched\n");

}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{   
    nkWindow* window = Window_Create();

    nkDockPanel* dockPanel = nkDockPanel_Create();
    window->RootView = dockPanel->View;

    dockPanel->View->BackgroundColor = COLOR_MAGENTA;

    /* create many dockpanel views */
    for (int i = 0; i < 1000; i++)
    {
        nkView* view = CreateView();

        nkColor randomColors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN};
        view->BackgroundColor = randomColors[rand() % 5];

        view->SizeRequest = (nkSize){1, 1};
        
        switch (i % 4)
        {
            case 0:
                view->DockPosition = DOCK_POSITION_TOP;
                break;
            case 1:
                view->DockPosition = DOCK_POSITION_BOTTOM;
                break;
            case 2:
                view->DockPosition = DOCK_POSITION_LEFT;
                break;
            case 3:
                view->DockPosition = DOCK_POSITION_RIGHT;
                break;
        }


        AddChildView(dockPanel->View, view);
    }

    //nkWindow* secondWindow = SecondWindow_Create();

    printf("Hello World\n"); 

    RunApp(AppLaunched);
 
    return 0;

}              