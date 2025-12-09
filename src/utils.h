#pragma once
#include <iostream>
#ifdef WIN32
#include "Windows.h"
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include "linux-keys.h"
#endif

struct KeyHandler{
        
#ifdef WIN32
        inline bool detect_key_press(const Key& key)
        {
            const char code = keyToVkCode(key);
            return (GetAsyncKeyState(code) & 0x8000) != 0;
        }
#else
        LT::LinuxKeyHandler handler;

        inline bool detect_key_press(const Key& key)
        {
            return handler.GetAsyncKeyState(key);
        }
#endif
};

#ifdef WIN32
inline void printFrameFast(const Ipp8u* frame, const int length)
{
    DWORD written;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), frame, length, &written, nullptr);
}

inline void calculateHeightWidth(int* height, int* width)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    *width = csbi.dwSize.X;
    *height = csbi.dwSize.Y;
}
#else


inline void printFrameFast(char* frame, int length)
{
    write(STDOUT_FILENO, frame, length);
}

inline void calculateHeightWidth(int* height, int* width)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        *width = w.ws_col;
    *height = w.ws_row;
}
#endif

// Universal compatibility
inline void clearScreen()
{
    std::cout << "\033[2J\033[H" << std::endl;
}

inline void toggle_cursor(bool hidden)
{
    if(hidden){
#ifdef WIN32
        std::cout << "\033[?25l";  // hide cursor
#else
        std::cout << "\e[?25l" << std::flush;  // Hide cursor
#endif
    } else {
#ifdef WIN32
        std::cout << "\033[?25h";  // hide cursor
#else
        std::cout << "\x1B[?25h" << std::flush; 
#endif
    }
}
