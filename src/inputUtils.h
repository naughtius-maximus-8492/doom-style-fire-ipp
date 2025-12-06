#ifdef WIN32
#define NOMINMAX
#include "Windows.h"

inline bool detect_key_press(const char code)
{
    return (GetAsyncKeyState(code) & 0x8000) != 0;
}


#else

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

inline void enable_raw_input() {
    static bool enabled = false;
    if (enabled) return;

    termios t;
    tcgetattr(STDIN_FILENO, &t);

    t.c_lflag &= ~(ICANON | ECHO);  // raw mode
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    // non-blocking stdin
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    enabled = true;
}

// returns: -1 = no key, otherwise the ASCII code
inline int poll_key() {
    enable_raw_input();
    return getchar();
}

// simulate "key is down" by seeing if the key was pressed this frame
inline bool detect_key_press(char key) {
    int c = poll_key();
    if (c == -1) return false;
    if (c == key) return true;

    // if it was another key, put it back
    ungetc(c, stdin);
    return false;
}

#endif