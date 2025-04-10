#include <stdio.h>

#include <Window.xml.h>
#include <SecondWindow.xml.h>

#include <NanoKit.h>

void AppLaunched(void *app)
{
    printf("App launched\n");
}

int main()
{   
    nkWindow* window = Window_Create();
    nkWindow* secondWindow = SecondWindow_Create();

    printf("Hello World\n"); 

    RunApp(NULL);
 
    return 0;

}              