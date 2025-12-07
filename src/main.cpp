#include <chrono>
#include <iostream>
#include <thread>
#include <chrono>

#ifdef WIN32
#else
#endif

#include "doomASCIIFire.h"

int main()
{
    int width {};
    int height {};

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

    short decayRate = 75;
    float colourBandMultiplier = 1.0F;
    auto last = std::chrono::steady_clock::now();

    while (fire.running)
    {
        printFrameFast(fire.getFrame());
        fire.decayStep();

#ifdef WIN32

        if (detect_key_press('Q'))
        {
            fire.openConfig();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        if (detect_key_press('K'))
        {
            decayRate--;
            fire.updateDecayRate(decayRate);
        }
        if (detect_key_press('J'))
        {
            decayRate++;
            fire.updateDecayRate(decayRate);
        }
        if (detect_key_press('L'))
        {
            colourBandMultiplier -= 0.02;
        }
        if (detect_key_press('H'))
        {
            colourBandMultiplier += 0.02;
        }
        if (detect_key_press('F'))
        {
            fire.backgroundMode =  !fire.backgroundMode;
            std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
        if (detect_key_press(VK_ESCAPE))
        {
            fire.running = false;
        }

        fire.colour_band_multiplier = colourBandMultiplier;
#endif

        while (std::chrono::steady_clock::now() - last < std::chrono::milliseconds(fire.frameDelay))
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }

        last = std::chrono::steady_clock::now();
    }
    return 0;
}