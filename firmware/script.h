#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <stdint.h>

/* One step of a payload sequence. */
typedef enum {
    OP_KEY,     /* tap one key: modifiers + keycode */
    OP_STRING,  /* type a printable-ASCII string */
    OP_DELAY,   /* wait delay_ms */
    OP_END,     /* end of sequence */
} script_op_t;

typedef struct {
    script_op_t op;
    uint8_t mod;       /* OP_KEY: HID modifiers */
    uint8_t usage;     /* OP_KEY: HID keycode */
    uint16_t delay_ms; /* OP_DELAY */
    const char *str;   /* OP_STRING */
} script_evt_t;

/* Run a sequence to completion. Returns false if USB was never ready. */
bool script_run(const script_evt_t *prog);

#endif /* SCRIPT_H_ */
