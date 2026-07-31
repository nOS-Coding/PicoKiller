#include "pico/stdlib.h"
#include "tusb.h"
#include "script.h"
#include "keyboard.h"

bool script_run(const script_evt_t *prog)
{
    for (const script_evt_t *e = prog; e->op != OP_END; e++) {
        switch (e->op) {
            case OP_KEY:
                keyboard_send(e->mod, e->usage);
                break;
            case OP_STRING:
                keyboard_type_string(e->str);
                break;
            case OP_DELAY:
                sleep_ms(e->delay_ms);
                break;
            default:
                break;
        }
    }
    return tud_ready();
}
