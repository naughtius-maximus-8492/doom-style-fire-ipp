#include <chrono>
#include <iostream>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

#include "doomASCIIFire.h"

int main()
{
    int width;
    int height;

#ifdef _WIN32
    // Work out size of console in windows
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    width = csbi.dwSize.X;
    height = csbi.dwSize.Y;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
    width = w.ws_col;
    height = w.ws_row;
#endif

    doomASCIIFire fire(width, height);

    while (true)
    {
        fire.printFrame();
        fire.decayStep();

        if (GetAsyncKeyState('Q') & 0x8000)
        {
            fire.openConfig();
        }

        fire.wait();

    }
    return 0;
}