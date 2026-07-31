#ifndef KEYS_H_
#define KEYS_H_

#include <stdbool.h>
#include <stdint.h>

/* USB HID keyboard modifier bits (report byte 0). */
#define KEY_CTRL  0x01
#define KEY_SHIFT 0x02
#define KEY_ALT   0x04
#define KEY_GUI   0x08

/* USB HID usage IDs for the keys used by payloads. */
#define KEY_ENTER      0x28
#define KEY_ESCAPE     0x29
#define KEY_BACKSPACE  0x2A
#define KEY_TAB        0x2B
#define KEY_SPACE      0x2C
#define KEY_LEFT       0x50
#define KEY_UP         0x52
#define KEY_DOWN       0x51
#define KEY_RIGHT      0x4F

#define KEY_A 0x04
#define KEY_B 0x05
#define KEY_C 0x06
#define KEY_D 0x07
#define KEY_E 0x08
#define KEY_F 0x09
#define KEY_G 0x0A
#define KEY_H 0x0B
#define KEY_I 0x0C
#define KEY_J 0x0D
#define KEY_K 0x0E
#define KEY_L 0x0F
#define KEY_M 0x10
#define KEY_N 0x11
#define KEY_O 0x12
#define KEY_P 0x13
#define KEY_Q 0x14
#define KEY_R 0x15
#define KEY_S 0x16
#define KEY_T 0x17
#define KEY_U 0x18
#define KEY_V 0x19
#define KEY_W 0x1A
#define KEY_X 0x1B
#define KEY_Y 0x1C
#define KEY_Z 0x1D

#define KEY_1 0x1E
#define KEY_2 0x1F
#define KEY_3 0x20
#define KEY_4 0x21
#define KEY_5 0x22
#define KEY_6 0x23
#define KEY_7 0x24
#define KEY_8 0x25
#define KEY_9 0x26
#define KEY_0 0x27

/* Map an ASCII character (US keyboard layout) to a modifier + keycode.
 * Returns false for characters that cannot be typed (control chars, etc). */
bool ascii_to_key(char c, uint8_t *mod, uint8_t *usage);

#endif /* KEYS_H_ */
