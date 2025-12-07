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


#else

// TODO Equivalent linux key detection

inline void printFrameFast(const std::string& frame)
{
    write(STDOUT_FILENO, frame.data(), frame.size());
}

#endif
