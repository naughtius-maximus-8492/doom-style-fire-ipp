#ifdef WIN32
#define NOMINMAX
#include "Windows.h"

inline bool detect_key_press(const char code)
{
    return (GetAsyncKeyState(code) & 0x8000) != 0;
}

inline void printFrameFast(const std::string& frame)
{
    DWORD written;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), frame.c_str(), frame.length(), &written, nullptr);
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
#include <sys/ioctl.h>
#include <unistd.h>


// TODO Equivalent linux key detection

inline void printFrameFast(const std::string& frame)
{
    write(STDOUT_FILENO, frame.data(), frame.size());
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
    std::cout << "\033[2J\033[H";
}