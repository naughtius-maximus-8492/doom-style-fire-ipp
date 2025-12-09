#include <chrono>

#ifdef WIN32
#else
#endif

#include "doomASCIIFire.h"


int main()
{
    toggle_cursor(true);

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
        fire->updateFrame();

        std::string frame {};

        while (std::chrono::steady_clock::now() - last < std::chrono::milliseconds(fire->frameDelay))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        last = std::chrono::steady_clock::now();

        printFrameFast(fire->startCharFrameBuffer, fire->charFrameBufferSize);

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
        if (detect_key_press('D'))
        {
            fire->colourBandMultiplier -= 0.02;
        }
        if (detect_key_press('A'))
        {
            fire->colourBandMultiplier += 0.02;
        }
        if (detect_key_press('F'))
        {
            fire->backgroundMode =  !fire->backgroundMode;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (detect_key_press('S'))
        {
            fire->frameDelay++;
            if (fire->frameDelay >= 1000)
            {
                fire->frameDelay = 100;
            }
        }
        if (detect_key_press('W'))
        {
            fire->frameDelay--;
            if (fire->frameDelay <= 0)
            {
                fire->frameDelay = 0;
            }
        }
        if (detect_key_press(VK_RIGHT))
        {
            fire->flicker++;
            if (fire->flicker > width / 3)
            {
                fire->flicker = width / 3;
            }
        }
        if (detect_key_press(VK_LEFT))
        {
            fire->flicker--;
            if (fire->flicker <= 0)
            {
                fire->flicker = 0;
            }
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
    toggle_cursor(false);

    return 0;
}
