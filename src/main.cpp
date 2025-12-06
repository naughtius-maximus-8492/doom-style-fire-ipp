#include <chrono>
#include <iostream>
#include <thread>
#include <chrono>
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

    if (width <= 0 || height <= 0)
    {
        width = 256;
        height = 256;
    }
    doomASCIIFire fire(width, height);
    float decayRate = 90;
    float colourBandMultiplier = 1.0F;
    auto last = std::chrono::steady_clock::now();

    while (true)
    {
        std::cout << fire.getFrame();
        fire.decayStep();

        if (GetAsyncKeyState('Q') & 0x8000)
        {
            fire.openConfig();
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            decayRate -= 0.75;
            fire.updateDecayRate(decayRate);
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            decayRate += 0.75;
            fire.updateDecayRate(decayRate);
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && colourBandMultiplier)
        {
            colourBandMultiplier += 0.02;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && colourBandMultiplier)
        {
            colourBandMultiplier -= 0.02;
        }
        if (GetAsyncKeyState('F') & 0x8000)
        {
            fire.backgroundMode =  !fire.backgroundMode;
            Sleep(100);
        }

        fire.colour_band_multiplier = colourBandMultiplier;

        while (std::chrono::steady_clock::now() - last < std::chrono::milliseconds(fire.frameDelay))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        last = std::chrono::steady_clock::now();

        std::cout << "\033[H";
    }
    return 0;
}