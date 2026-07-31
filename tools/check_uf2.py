#!/usr/bin/env python3
"""Validate a PicoKiller config .uf2 and print the decoded config.

Usage: check_uf2.py picokiller-config.uf2
"""
import sys
import zlib

UF2_MAGIC0 = 0x0A324655  # "UF2\n"
UF2_MAGIC1 = 0x9E5D5157
UF2_ENDMAGIC = 0x0AB16F30
CONFIG_MAGIC = 0x31434B50  # "PKC1"
CONFIG_VERSION = 1
FLASH_BASE = 0x10000000
CONFIG_FLASH_OFFSET = 0x1F0000

OS_NAMES = {1: "Windows", 2: "Linux", 4: "macOS", 8: "Android"}
PRANK_NAMES = {1: "Fake BSOD", 2: "Fake Update", 4: "Network kill", 8: "Shutdown"}


def le32(buf, off):
    return int.from_bytes(buf[off : off + 4], "little")


def check(path):
    data = open(path, "rb").read()
    if len(data) % 512 != 0 or len(data) == 0:
        sys.exit(f"bad size: {len(data)} (must be whole 512-byte blocks)")

    config_bytes = None
    blockno = 0
    for off in range(0, len(data), 512):
        block = data[off : off + 512]
        if le32(block, 0) != UF2_MAGIC0 or le32(block, 4) != UF2_MAGIC1:
            sys.exit(f"bad magic at block offset {off}")
        if le32(block, 508) != UF2_ENDMAGIC:
            sys.exit(f"bad end magic at block offset {off}")

        addr = le32(block, 12)
        size = le32(block, 16)
        no = le32(block, 20)
        total = le32(block, 24)
        assert no == blockno, f"block number {no} != {blockno}"
        blockno += 1

        if addr < FLASH_BASE + CONFIG_FLASH_OFFSET or addr >= FLASH_BASE + CONFIG_FLASH_OFFSET + 0x1000:
            sys.exit(f"block {no} writes outside the config sector: 0x{addr:08x}")

        payload = block[32 : 32 + size]
        if config_bytes is None:
            config_bytes = payload
        else:
            config_bytes += payload

    print(f"OK: {blockno} block(s), {len(config_bytes)} config bytes")

    cfg = config_bytes[:20]
    magic = le32(cfg, 0)
    version = int.from_bytes(cfg[4:6], "little")
    targets = le32(cfg, 8)
    pranks = le32(cfg, 12)
    delay_ms = le32(cfg, 16)
    stored_crc = le32(config_bytes, 20)
    calc_crc = zlib.crc32(cfg)

    if magic != CONFIG_MAGIC:
        sys.exit(f"BAD magic: 0x{magic:08x} (expected 0x{CONFIG_MAGIC:08x})")
    if version != CONFIG_VERSION:
        sys.exit(f"BAD version: {version}")
    if stored_crc != calc_crc:
        sys.exit(f"BAD crc32: stored 0x{stored_crc:08x}, computed 0x{calc_crc:08x}")

    print(f"magic   : PKC1 v{version}")
    print(f"crc32   : 0x{stored_crc:08x} OK")
    print(f"targets : {' '.join(n for f, n in OS_NAMES.items() if targets & f) or 'none'}")
    print(f"pranks  : {' '.join(n for f, n in PRANK_NAMES.items() if pranks & f) or 'none'}")
    print(f"fuse    : {delay_ms} ms")
    print("PASS")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    check(sys.argv[1])
