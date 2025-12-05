#include <chrono>
#include <iostream>
#include <thread>

#include "doomASCIIFire.h"

int main()
{
    std::cout << "\033[38;5;128m0Oo.\033[0m\n";

    doomASCIIFire fire{};

    while (true)
    {
        fire.printFrame();
        fire.decayStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(42));
    }
    return 0;
}