#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "keys.h"
#include "keyboard.h"

#define PRESS_MS  20
#define RELEASE_MS 20

void keyboard_poll(void)
{
    tud_task();
}

/* Blocking send of an 8-byte keyboard report. */
static void send_report(const uint8_t report[8])
{
    while (!tud_hid_n_ready(0)) {
        tud_task();
        sleep_ms(1);
    }
    tud_hid_report(0, report, 8);
    tud_task();
}

void keyboard_send(uint8_t mod, uint8_t usage)
{
    uint8_t down[8] = {0};
    uint8_t up[8] = {0};

    down[0] = mod;
    down[2] = usage;

    if (!tud_ready()) return;

    send_report(down);
    sleep_ms(PRESS_MS);
    send_report(up);
    sleep_ms(RELEASE_MS);
}

/* Type a single ASCII character (US layout) as a keypress. */
void keyboard_type_char(char c)
{
    uint8_t mod, usage;
    if (!ascii_to_key(c, &mod, &usage)) return;
    keyboard_send(mod, usage);
}

/* Type a whole string. */
void keyboard_type_string(const char *s)
{
    while (*s) {
        keyboard_type_char(*s++);
    }
}
