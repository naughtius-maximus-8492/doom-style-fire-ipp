#ifdef WIN32
#define NOMINMAX
#include "Windows.h"

inline bool detect_key_press(const char code)
{
    return (GetAsyncKeyState(code) & 0x8000) != 0;
}


#else

// TODO Equivalent linux key detection

#endif
