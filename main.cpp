#include <chrono>
#include <iostream>
#include <thread>
#define NOMINMAX
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
    float decayRate = 9;
    float colourBandMultiplier = 1.0F;

    while (true)
    {
        fire.printFrame();
        fire.decayStep();

        if (GetAsyncKeyState('Q') & 0x8000)
        {
            fire.openConfig();
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 && decayRate < 1000)
        {
            decayRate -= 0.25;
            fire.updateDecayRate(decayRate);
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && decayRate > 0)
        {
            decayRate += 0.25;
            fire.updateDecayRate(decayRate);
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && colourBandMultiplier >= 0)
        {
            colourBandMultiplier -= 0.02;
            fire.colour_band_multiplier = colourBandMultiplier;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && colourBandMultiplier <= 1.5)
        {
            colourBandMultiplier += 0.02;
            fire.colour_band_multiplier = colourBandMultiplier;
        }
        if (GetAsyncKeyState('F') & 0x8000)
        {
            fire.backgroundMode =  !fire.backgroundMode;
            Sleep(20);
        }
        fire.wait();
    }
    return 0;
}