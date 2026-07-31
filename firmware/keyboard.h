#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>

/* Pump the USB task so TinyUSB stays alive. */
void keyboard_poll(void);

/* Press (and release) one key with the given modifiers, waiting for USB. */
void keyboard_send(uint8_t mod, uint8_t usage);

/* Type a single printable ASCII character. */
void keyboard_type_char(char c);

/* Type a whole string of printable ASCII characters. */
void keyboard_type_string(const char *s);

#endif /* KEYBOARD_H_ */
