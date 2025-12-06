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
    float defaultDecayRate = 9;
    float defaultColourBandMultiplier = 1.0F;

    while (true)
    {
        fire.printFrame();
        fire.decayStep();

        if (GetAsyncKeyState('Q') & 0x8000)
        {
            fire.openConfig();
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 && defaultDecayRate < 1000)
        {
            defaultDecayRate -= 0.1;
            fire.updateDecayRate(defaultDecayRate);
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && defaultDecayRate > 0)
        {
            defaultDecayRate += 0.1;
            fire.updateDecayRate(defaultDecayRate);
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && defaultColourBandMultiplier >= 0)
        {
            defaultColourBandMultiplier -= 0.01;
            fire.colour_band_multiplier = defaultColourBandMultiplier;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && defaultColourBandMultiplier <= 1.5)
        {
            defaultColourBandMultiplier += 0.01;
            fire.colour_band_multiplier = defaultColourBandMultiplier;
        }

        fire.wait();

    }
    return 0;
}