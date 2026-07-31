#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "keyboard.h"
#include "config.h"
#include "payloads.h"

static void blink_led(bool on)
{
    board_led_write(on);
}

int main(void)
{
    board_init();
    stdio_init_all();
    tusb_init();

    config_t cfg = config_load();

    printf("PicoKiller boot: targets=0x%08lx pranks=0x%08lx delay=%lu ms\n",
           (unsigned long)cfg.targets, (unsigned long)cfg.pranks,
           (unsigned long)cfg.delay_ms);

    /* Wait for the host to finish enumerating the keyboard. */
    uint32_t timeout = 10000;
    while (!tud_ready() && timeout-- > 0) {
        tud_task();
        sleep_ms(2);
    }

    /* Countdown so the victim can be surprised (or the user can unplug). */
    uint32_t remaining = cfg.delay_ms;
    while (remaining > 0) {
        blink_led((remaining / 500) & 1);
        tud_task();
        sleep_ms(1);
        remaining--;
    }
    blink_led(true);

    if (!tud_ready()) {
        printf("PicoKiller: no USB host, aborting\n");
        while (1) {
            tud_task();
            sleep_ms(10);
        }
    }

    payload_run_all(cfg.targets, cfg.pranks);

    printf("PicoKiller: done, idle\n");
    blink_led(false);

    while (1) {
        tud_task();
        sleep_ms(10);
    }
}
