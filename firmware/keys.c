#include "keys.h"

static const uint8_t letter_usage[26] = {
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z
};

static const uint8_t digit_usage[10] = {
    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0
};

bool ascii_to_key(char c, uint8_t *mod, uint8_t *usage)
{
    if (c == ' ') {
        *mod = 0;
        *usage = KEY_SPACE;
        return true;
    }
    if (c >= 'a' && c <= 'z') {
        *mod = 0;
        *usage = letter_usage[c - 'a'];
        return true;
    }
    if (c >= 'A' && c <= 'Z') {
        *mod = KEY_SHIFT;
        *usage = letter_usage[c - 'A'];
        return true;
    }
    if (c >= '0' && c <= '9') {
        *mod = 0;
        *usage = digit_usage[c - '0'];
        return true;
    }

    switch (c) {
        case '!': *mod = KEY_SHIFT; *usage = KEY_1; return true;
        case '@': *mod = KEY_SHIFT; *usage = KEY_2; return true;
        case '#': *mod = KEY_SHIFT; *usage = KEY_3; return true;
        case '$': *mod = KEY_SHIFT; *usage = KEY_4; return true;
        case '%': *mod = KEY_SHIFT; *usage = KEY_5; return true;
        case '^': *mod = KEY_SHIFT; *usage = KEY_6; return true;
        case '&': *mod = KEY_SHIFT; *usage = KEY_7; return true;
        case '*': *mod = KEY_SHIFT; *usage = KEY_8; return true;
        case '(': *mod = KEY_SHIFT; *usage = KEY_9; return true;
        case ')': *mod = KEY_SHIFT; *usage = KEY_0; return true;
        case '_': *mod = KEY_SHIFT; *usage = 0x2D; return true; /* - */
        case '+': *mod = KEY_SHIFT; *usage = 0x2E; return true; /* = */
        case '{': *mod = KEY_SHIFT; *usage = 0x2F; return true; /* [ */
        case '}': *mod = KEY_SHIFT; *usage = 0x30; return true; /* ] */
        case '|': *mod = KEY_SHIFT; *usage = 0x31; return true; /* \ */
        case ':': *mod = KEY_SHIFT; *usage = 0x33; return true; /* ; */
        case '"': *mod = KEY_SHIFT; *usage = 0x34; return true; /* ' */
        case '~': *mod = KEY_SHIFT; *usage = 0x35; return true; /* ` */
        case '<': *mod = KEY_SHIFT; *usage = 0x36; return true; /* , */
        case '>': *mod = KEY_SHIFT; *usage = 0x37; return true; /* . */
        case '?': *mod = KEY_SHIFT; *usage = 0x38; return true; /* / */
        case '-': *mod = 0; *usage = 0x2D; return true;
        case '=': *mod = 0; *usage = 0x2E; return true;
        case '[': *mod = 0; *usage = 0x2F; return true;
        case ']': *mod = 0; *usage = 0x30; return true;
        case '\\': *mod = 0; *usage = 0x31; return true;
        case ';': *mod = 0; *usage = 0x33; return true;
        case '\'': *mod = 0; *usage = 0x34; return true;
        case '`': *mod = 0; *usage = 0x35; return true;
        case ',': *mod = 0; *usage = 0x36; return true;
        case '.': *mod = 0; *usage = 0x37; return true;
        case '/': *mod = 0; *usage = 0x38; return true;
        default: return false;
    }
}
