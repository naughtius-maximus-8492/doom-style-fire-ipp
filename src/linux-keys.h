#pragma once

#include <map>
#include <thread>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <termios.h>
#include <unistd.h>

namespace LT {
    // Function to set terminal to raw mode
    inline void setRawMode(bool enable) {
        static struct termios oldt, newt;
        if (enable) {
            tcgetattr(STDIN_FILENO, &oldt);  // Save old settings
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply new settings
        } else {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore old settings
        }
    }

    class LinuxKeyHandler{
    private:
        std::map<char, std::chrono::time_point<std::chrono::steady_clock>> pressed_keys;
        bool running = false;
        std::thread key_down_thread;
        void key_down_func();

        const int delay = 100;

    public:
        LinuxKeyHandler();
        ~LinuxKeyHandler();
        bool GetAsyncKeyState(const char& key);
    };

}
