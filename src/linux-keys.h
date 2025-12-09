#pragma once

#include "linux-virtual-keys.h"
#include <map>
#include <thread>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include "keys.h"

namespace LT {

// Interpret a buffer of bytes from terminal input and convert to a Key enum.
// Returns KEY_NONE if unknown or unsupported.
inline Key decode_key(const char *buf, ssize_t n) {
    if (n <= 0 || buf == NULL)
        return KEY_NONE;

    // Ensure temp buffer is null-terminated for strcmp comparisons
    char tmp[8];
    ssize_t len = (n < (ssize_t)sizeof(tmp) - 1) ? n : (ssize_t)sizeof(tmp) - 1;
    memcpy(tmp, buf, len);
    tmp[len] = '\0';

    // ------------- Multi-byte (escape) keys -------------
    if (strcmp(tmp, VK_UP) == 0) return UP;
    if (strcmp(tmp, VK_DOWN) == 0) return DOWN;
    if (strcmp(tmp, VK_RIGHT) == 0) return RIGHT;
    if (strcmp(tmp, VK_LEFT) == 0) return LEFT;

    if (strcmp(tmp, VK_HOME) == 0) return HOME;
    if (strcmp(tmp, VK_END) == 0) return END;
    if (strcmp(tmp, VK_INSERT) == 0) return INSERT;
    if (strcmp(tmp, VK_DELETE) == 0) return DELETE;
    if (strcmp(tmp, VK_PAGEUP) == 0) return PAGEUP;
    if (strcmp(tmp, VK_PAGEDOWN) == 0) return PAGEDOWN;

    if (strcmp(tmp, VK_F1) == 0) return F1;
    if (strcmp(tmp, VK_F2) == 0) return F2;
    if (strcmp(tmp, VK_F3) == 0) return F3;
    if (strcmp(tmp, VK_F4) == 0) return F4;
    if (strcmp(tmp, VK_F5) == 0) return F5;
    if (strcmp(tmp, VK_F6) == 0) return F6;
    if (strcmp(tmp, VK_F7) == 0) return F7;
    if (strcmp(tmp, VK_F8) == 0) return F8;
    if (strcmp(tmp, VK_F9) == 0) return F9;
    if (strcmp(tmp, VK_F10) == 0) return F10;
    if (strcmp(tmp, VK_F11) == 0) return F11;
    if (strcmp(tmp, VK_F12) == 0) return F12;

    // ------------- Single-byte keys -------------
    if (tmp[0] == '\033') return ESC;
    if (tmp[0] == '\t') return TAB;
    if (tmp[0] == '\n') return ENTER;
    if (tmp[0] == 0x7F) return BACKSPACE;

    // ------------- Letters -------------
    if (n == 1) {
        char c = tmp[0];
        if (c >= 'A' && c <= 'Z')
            return (Key)(A + (c - 'A'));
        if (c >= 'a' && c <= 'z')
            return (Key)(a + (c - 'a'));
    }

    return KEY_NONE;
}
    // Function to set terminal to raw mode
    inline void setRawMode(bool enable) {
#ifdef __linux__
        static struct termios oldt, newt;
        if (enable) {
            tcgetattr(STDIN_FILENO, &oldt);  // Save old settings
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply new settings
        } else {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore old settings
        }
#endif
    }

    class LinuxKeyHandler{
    private:
        std::map<Key, std::chrono::time_point<std::chrono::steady_clock>> pressed_keys;
        bool running = true;
        std::thread key_down_thread;
        void key_down_func();

        const int delay = 50;

    public:
        LinuxKeyHandler();
        ~LinuxKeyHandler();
        bool GetAsyncKeyState(const Key& key);
    };

}
