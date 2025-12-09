#ifndef LINUX_VIRTUAL_KEYS
#define LINUX_VIRTUAL_KEYS

// This header defines common terminal input sequences for special keys.
// These are the raw bytes (as C strings) that most Linux terminals
// output when you press an arrow key, function key, etc.
// You can compare these to sequences read from read(STDIN_FILENO, ...).

// --- Arrow keys ---
#include <cstring>
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
#endif  // TERMINAL_KEYCODES_H
