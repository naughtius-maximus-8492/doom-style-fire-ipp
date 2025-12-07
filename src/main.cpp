#include <chrono>

#ifdef WIN32
#else
#endif

#include "doomASCIIFire.h"

int main()
{
    int width {};
    int height {};

    calculateHeightWidth(&height, &width);

    // For running in IDE debug
    if (width <= 0 || height <= 0)
    {
        width = 256;
        height = 256;
    }

    doomASCIIFire* fire = new doomASCIIFire(width, height);

    auto last = std::chrono::steady_clock::now();

    int oldHeight = height;
    int oldWidth = width;
    bool running = true;
    while (running)
    {
        fire->decayStep();
        std::string frame = "\033[H" + fire->getFrame();

        while (std::chrono::steady_clock::now() - last < std::chrono::milliseconds(fire->frameDelay) || fire->frameDelay > defaultDelay)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        last = std::chrono::steady_clock::now();

        printFrameFast(frame);
        fire->decayStep();

#ifdef WIN32

        if (detect_key_press('Q'))
        {
            fire->openConfig();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        if (detect_key_press(VK_UP))
        {
            fire->updateDecayRate(false);
        }
        if (detect_key_press(VK_DOWN))
        {
            fire->updateDecayRate(true);
        }
        if (detect_key_press(VK_RIGHT))
        {
            fire->colour_band_multiplier -= 0.02;
        }
        if (detect_key_press(VK_LEFT))
        {
            fire->colour_band_multiplier += 0.02;
        }
        if (detect_key_press('F'))
        {
            fire->backgroundMode =  !fire->backgroundMode;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (detect_key_press('R'))
        {
            delete fire;
            fire = new doomASCIIFire(width, height);
        }
        if (detect_key_press(VK_ESCAPE))
        {
            running = false;
        }

#endif

        // Check if window size has changed

        calculateHeightWidth(&height, &width);
        if (oldHeight != height || oldWidth != width)
        {
            oldHeight = height;
            oldWidth = width;
            delete fire;
            fire = new doomASCIIFire(width, height);
        }

    }

    delete fire;

    clearScreen();

    return 0;
}