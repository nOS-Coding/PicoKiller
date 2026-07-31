#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

/* Target operating systems (bit flags, stored in flash). */
#define OS_WINDOWS (1u << 0)
#define OS_LINUX   (1u << 1)
#define OS_MACOS   (1u << 2)
#define OS_ANDROID (1u << 3)

/* Pranks (bit flags, stored in flash). */
#define PRANK_BSOD    (1u << 0)
#define PRANK_UPDATE  (1u << 1)
#define PRANK_NETWORK (1u << 2)
#define PRANK_SHUTDOWN (1u << 3)

typedef struct __attribute__((packed)) {
    uint32_t magic;      /* 'PKC1' */
    uint16_t version;    /* config format version */
    uint16_t reserved;
    uint32_t targets;    /* OR of OS_* */
    uint32_t pranks;     /* OR of PRANK_* */
    uint32_t delay_ms;   /* seconds-equivalent countdown before firing */
} config_t;

/* Load the config from flash, falling back to defaults if absent/invalid. */
config_t config_load(void);

#endif /* CONFIG_H_ */
