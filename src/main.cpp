#include "linux-keys.h"
#include "oneapi/tbb/parallel_pipeline.h"
#include <chrono>
#include <cstdlib>
#include <iostream>

#ifdef WIN32
#else
#endif

#include "doomASCIIFire.h"
#include <termios.h>
#include <unistd.h>



int main()
{
    KeyHandler keyHandler;
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

        if (keyHandler.detect_key_press(Key::q))
        {
            fire->openConfig();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        if (keyHandler.detect_key_press(Key::UP))
        {
            fire->updateDecayRate(false);
        }
        if (keyHandler.detect_key_press(Key::DOWN))
        {
            fire->updateDecayRate(true);
        }
        if (keyHandler.detect_key_press(Key::d))
        {
            fire->colourBandMultiplier -= 0.02;
        }
        if (keyHandler.detect_key_press(Key::a))
        {
            fire->colourBandMultiplier += 0.02;
        }
        if (keyHandler.detect_key_press(Key::f))
        {
            fire->backgroundMode =  !fire->backgroundMode;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (keyHandler.detect_key_press(Key::s))
        {
            fire->frameDelay++;
            if (fire->frameDelay >= 1000)
            {
                fire->frameDelay = 100;
            }
        }
        if (keyHandler.detect_key_press(Key::w))
        {
            fire->frameDelay--;
            if (fire->frameDelay <= 0)
            {
                fire->frameDelay = 0;
            }
        }
        if (keyHandler.detect_key_press(Key::RIGHT))
        {
            fire->flicker++;
            if (fire->flicker > width / 3)
            {
                fire->flicker = width / 3;
            }
        }
        if (keyHandler.detect_key_press(Key::LEFT))
        {
            fire->flicker--;
            if (fire->flicker <= 0)
            {
                fire->flicker = 0;
            }
        }
        if (keyHandler.detect_key_press(Key::ESC))
        {
            running = false;
        }

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
