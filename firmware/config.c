#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "config.h"

/* The config lives in the last 4 KB sector of flash so that a config-only .uf2
 * (dragged in BOOTSEL mode) only overwrites this region, never the firmware. */
#define CONFIG_FLASH_OFFSET (0x1F0000u)   /* 1.9375 MB into a 2 MB flash */
#define CONFIG_MAGIC        0x31434B50u   /* 'PKC1' little-endian */
#define CONFIG_VERSION      1
#define CONFIG_BYTES        sizeof(config_t)   /* 20 bytes */

static const config_t DEFAULT_CONFIG = {
    .magic    = CONFIG_MAGIC,
    .version  = CONFIG_VERSION,
    .reserved = 0,
    .targets  = 0,          /* no pranks until a config is flashed */
    .pranks   = 0,
    .delay_ms = 3000,
};

static uint32_t crc32(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (uint32_t)(-(int32_t)(crc & 1)));
        }
    }
    return ~crc;
}

config_t config_load(void)
{
    config_t cfg = DEFAULT_CONFIG;

    /* Flash is memory-mapped at XIP_BASE (0x10000000). */
    const uint8_t *raw = (const uint8_t *)(XIP_BASE + CONFIG_FLASH_OFFSET);

    memcpy(&cfg, raw, CONFIG_BYTES);

    if (cfg.magic != CONFIG_MAGIC) {
        return DEFAULT_CONFIG;
    }
    if (cfg.version != CONFIG_VERSION) {
        return DEFAULT_CONFIG;
    }

    uint32_t stored_crc;
    memcpy(&stored_crc, raw + CONFIG_BYTES, 4);

    if (crc32(raw, CONFIG_BYTES) != stored_crc) {
        return DEFAULT_CONFIG;
    }

    return cfg;
}
