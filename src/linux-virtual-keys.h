#ifndef LINUX_VIRTUAL_KEYS
#define LINUX_VIRTUAL_KEYS

// This header defines common terminal input sequences for special keys.
// These are the raw bytes (as C strings) that most Linux terminals
// output when you press an arrow key, function key, etc.
// You can compare these to sequences read from read(STDIN_FILENO, ...).

// --- Arrow keys ---
#include <cstring>
#include <unistd.h>
#define VK_UP     "\033[A"
#define VK_DOWN   "\033[B"
#define VK_RIGHT  "\033[C"
#define VK_LEFT   "\033[D"

// --- Navigation keys ---
#define VK_HOME   "\033[H"
#define VK_END    "\033[F"
#define VK_INSERT "\033[2~"
#define VK_DELETE "\033[3~"
#define VK_PAGEUP "\033[5~"
#define VK_PAGEDOWN "\033[6~"

// --- Function keys (common xterm values) ---
#define VK_F1     "\033OP"
#define VK_F2     "\033OQ"
#define VK_F3     "\033OR"
#define VK_F4     "\033OS"
#define VK_F5     "\033[15~"
#define VK_F6     "\033[17~"
#define VK_F7     "\033[18~"
#define VK_F8     "\033[19~"
#define VK_F9     "\033[20~"
#define VK_F10    "\033[21~"
#define VK_F11    "\033[23~"
#define VK_F12    "\033[24~"

// --- Misc single-byte keys ---
#define VK_ESC    "\033"   // 0x1B
#define VK_TAB    "\t"     // 0x09
#define VK_ENTER  "\n"     // 0x0A
#define VK_BACKSPACE "\177" // 0x7F (DEL)

enum Key{
    // --- Special / control keys ---
    KEY_NONE = 0,
    ESC,
    TAB,
    ENTER,
    BACKSPACE,

    // --- Arrow keys ---
    UP,
    DOWN,
    RIGHT,
    LEFT,

    // --- Navigation keys ---
    HOME,
    END,
    INSERT,
    DELETE,
    PAGEUP,
    PAGEDOWN,

    // --- Function keys ---
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // --- Letter keys (uppercase) ---
    A, B, C, D, E, F, G, H, I, J,
    K, L, M, N, O, P, Q, R, S, T,
    U, V, W, X, Y, Z,

    // --- Letter keys (lowercase) ---
    a, b, c, d, e, f, g, h, i, j,
    k, l, m, n, o, p, q, r, s, t,
    u, v, w, x, y, z,

    KEY_COUNT  // Always last — counts the total number of keys
};

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

#endif  // TERMINAL_KEYCODES_H
