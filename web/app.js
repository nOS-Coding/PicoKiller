/* PicoKiller configurator: builds a config-only .uf2 the Pico reads at boot.
 *
 * Config binary layout (little-endian), must match firmware/config.c:
 *   offset 0:  uint32 magic 'PKC1' (0x31434B50)
 *   offset 4:  uint16 version (1)
 *   offset 6:  uint16 reserved (0)
 *   offset 8:  uint32 target flags (OS_*)
 *   offset 12: uint32 prank flags (PRANK_*)
 *   offset 16: uint32 fuse delay in ms
 *   offset 20: uint32 CRC-32 of bytes 0..19
 *
 * The .uf2 blocks write this into the last 4 KB sector of flash
 * (address 0x10000000 + 0x1F0000), leaving firmware untouched.
 */

"use strict";

// Bit flags (must match firmware/config.h)
const OS = { windows: 1 << 0, linux: 1 << 1, macos: 1 << 2 };
const PRANK = { bsod: 1 << 0, update: 1 << 1, network: 1 << 2, shutdown: 1 << 3 };

const CONFIG_MAGIC = 0x31434B50;          // 'PKC1'
const CONFIG_VERSION = 1;
const CONFIG_FLASH_OFFSET = 0x1F0000;     // must match firmware/config.c
const UF2_FLASH_BASE = 0x10000000;

// ---- CRC-32 (bitwise, identical to firmware/config.c) ----
function crc32(data) {
  let crc = 0xFFFFFFFF;
  for (let i = 0; i < data.length; i++) {
    crc ^= data[i];
    for (let b = 0; b < 8; b++) {
      crc = (crc >>> 1) ^ (0xEDB88320 & -(crc & 1));
    }
  }
  return (~crc) >>> 0;
}

// ---- Config binary ----
function buildConfig(targetFlags, prankFlags, delayMs) {
  const bytes = new Uint8Array(24);
  const dv = new DataView(bytes.buffer);
  dv.setUint32(0, CONFIG_MAGIC, true);
  dv.setUint16(4, CONFIG_VERSION, true);
  dv.setUint16(6, 0, true);
  dv.setUint32(8, targetFlags >>> 0, true);
  dv.setUint32(12, prankFlags >>> 0, true);
  dv.setUint32(16, delayMs >>> 0, true);
  dv.setUint32(20, crc32(bytes.subarray(0, 20)), true);
  return bytes;
}

// ---- UF2 wrapping ----
function toUf2(data, addr) {
  const n = Math.ceil(data.length / 476);
  const out = new Uint8Array(n * 512);
  for (let i = 0; i < n; i++) {
    const off = i * 512;
    // magic "UF2\n", magic2, flags=0 (no family ID -> works on RP2040 & RP2350)
    out.set([0x55, 0x46, 0x32, 0x0A], off);
    out.set([0x57, 0x51, 0x5D, 0x9E], off + 4);
    const a = addr + i * 476;
    out[off + 12] = a & 0xFF;        out[off + 13] = (a >> 8) & 0xFF;
    out[off + 14] = (a >> 16) & 0xFF; out[off + 15] = (a >>> 24) & 0xFF;
    const sz = Math.min(476, data.length - i * 476);
    out[off + 16] = sz & 0xFF;       out[off + 17] = (sz >> 8) & 0xFF;
    out[off + 20] = i & 0xFF;        out[off + 21] = (i >> 8) & 0xFF;
    out[off + 24] = n & 0xFF;        out[off + 25] = (n >> 8) & 0xFF;
    out.set(data.subarray(i * 476, i * 476 + sz), off + 32);
    out.set([0x30, 0x6F, 0xB1, 0x0A], off + 508); // end magic
  }
  return out;
}

// ---- Download helper ----
function download(bytes, name) {
  const blob = new Blob([bytes.buffer], { type: "application/octet-stream" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  a.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}

// ---- UI ----
const el = (id) => document.getElementById(id);

function selected(group, prefix) {
  let flags = 0;
  for (const key of Object.keys(group)) {
    if (el(`${prefix}-${key}`).checked) flags |= group[key];
  }
  return flags;
}

function refresh() {
  const win = el("t-windows").checked;
  const shutdown = el("p-shutdown").checked;

  // BSOD is Windows-only, and shutdown disables everything else
  if (!win) el("p-bsod").checked = false;
  if (shutdown) el("p-bsod").checked = false;
  el("p-bsod").disabled = !win || shutdown;

  for (const id of ["p-update", "p-network"]) {
    el(id).disabled = shutdown;
    if (shutdown) el(id).checked = false;
  }

  updateSummary();
}

function updateSummary() {
  const tFlags = selected(OS, 't');
  const pFlags = selected(PRANK, 'p');
  const delayS = clampDelay();
  const names = {
    windows: "Windows", linux: "Linux", macos: "macOS",
    bsod: "Fake BSOD", update: "Fake Update", network: "Network kill", shutdown: "Shutdown",
  };
  const tNames = Object.keys(OS).filter((k) => tFlags & OS[k]).map((k) => names[k]);
  const pNames = Object.keys(PRANK).filter((k) => pFlags & PRANK[k]).map((k) => names[k]);
  el("summary").textContent =
    `Targets : ${tNames.join(", ") || "none"}\nPranks  : ${pNames.join(", ") || "none"}\nFuse    : ${delayS}s`;
}

function clampDelay() {
  const v = parseInt(el("delay").value, 10);
  return Math.max(0, Math.min(60, isNaN(v) ? 3 : v));
}

function build() {
  const tFlags = selected(OS, 't');
  const pFlags = selected(PRANK, 'p');
  const error = el("error");

  if (!tFlags) { error.textContent = "Select at least one target system."; error.classList.remove("hidden"); return; }
  if (!pFlags) { error.textContent = "Select at least one prank."; error.classList.remove("hidden"); return; }

  const delayMs = clampDelay() * 1000;
  const cfg = buildConfig(tFlags, pFlags, delayMs);
  const uf2 = toUf2(cfg, UF2_FLASH_BASE + CONFIG_FLASH_OFFSET);
  download(uf2, "picokiller-config.uf2");

  error.classList.add("hidden");
}

document.addEventListener("DOMContentLoaded", () => {
  for (const id of ["t-windows", "t-linux", "t-macos",
                    "p-bsod", "p-update", "p-network", "p-shutdown"]) {
    el(id).addEventListener("change", refresh);
  }
  el("delay").addEventListener("input", updateSummary);
  el("build").addEventListener("click", build);
  refresh();
});
