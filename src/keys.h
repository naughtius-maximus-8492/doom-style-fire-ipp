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
    DEL,
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

#ifdef WIN32
#include "Windows.h"
inline int keyToVkCode(Key key) noexcept{
    switch (key) {
        // --- Control / special keys ---
        case ESC:        return VK_ESCAPE;
        case TAB:        return VK_TAB;
        case ENTER:      return VK_RETURN;
        case BACKSPACE:  return VK_BACK;

        // --- Arrow keys ---
        case UP:         return VK_UP;
        case DOWN:       return VK_DOWN;
        case RIGHT:      return VK_RIGHT;
        case LEFT:       return VK_LEFT;

        // --- Navigation keys ---
        case HOME:       return VK_HOME;
        case END:        return VK_END;
        case INSERT:     return VK_INSERT;
        case DEL:     return VK_DELETE;
        case PAGEUP:     return VK_PRIOR;  // PAGE UP
        case PAGEDOWN:   return VK_NEXT;   // PAGE DOWN

        // --- Function keys ---
        case F1:         return VK_F1;
        case F2:         return VK_F2;
        case F3:         return VK_F3;
        case F4:         return VK_F4;
        case F5:         return VK_F5;
        case F6:         return VK_F6;
        case F7:         return VK_F7;
        case F8:         return VK_F8;
        case F9:         return VK_F9;
        case F10:        return VK_F10;
        case F11:        return VK_F11;
        case F12:        return VK_F12;

        // --- Uppercase letters ---
        case A: return 'A';
        case B: return 'B';
        case C: return 'C';
        case D: return 'D';
        case E: return 'E';
        case F: return 'F';
        case G: return 'G';
        case H: return 'H';
        case I: return 'I';
        case J: return 'J';
        case K: return 'K';
        case L: return 'L';
        case M: return 'M';
        case N: return 'N';
        case O: return 'O';
        case P: return 'P';
        case Q: return 'Q';
        case R: return 'R';
        case S: return 'S';
        case T: return 'T';
        case U: return 'U';
        case V: return 'V';
        case W: return 'W';
        case X: return 'X';
        case Y: return 'Y';
        case Z: return 'Z';

        // --- Lowercase letters ---
        // Note: VK codes are case-insensitive, 'A' and 'a' have *the same* VK code.
        case a: return 'A';
        case b: return 'B';
        case c: return 'C';
        case d: return 'D';
        case e: return 'E';
        case f: return 'F';
        case g: return 'G';
        case h: return 'H';
        case i: return 'I';
        case j: return 'J';
        case k: return 'K';
        case l: return 'L';
        case m: return 'M';
        case n: return 'N';
        case o: return 'O';
        case p: return 'P';
        case q: return 'Q';
        case r: return 'R';
        case s: return 'S';
        case t: return 'T';
        case u: return 'U';
        case v: return 'V';
        case w: return 'W';
        case x: return 'X';
        case y: return 'Y';
        case z: return 'Z';

        default:
        case KEY_NONE:   return 0;
    }
}
#endif
