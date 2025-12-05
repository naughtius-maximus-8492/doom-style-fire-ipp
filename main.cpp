#include <chrono>
#include <iostream>
#include <thread>

#include "doomASCIIFire.h"

int main()
{
    int R = 254;
    int G = 128;
    int B = 0;
    std::cout << "\033[38;2;" << R << ";" << G << ";" << B << "m0\033[0m\n";


    doomASCIIFire fire{};

    while (true)
    {
        fire.printFrame();
        fire.decayStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(42));
    }
    return 0;
}