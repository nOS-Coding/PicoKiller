// Generate a sample config .uf2 using the exact same functions as web/app.js.
// Usage: node tools/gen_sample.mjs [delay_s]  (writes tools/sample-config.uf2)
import { readFileSync, writeFileSync } from "node:fs";
import vm from "node:vm";

const appJs = readFileSync(new URL("../web/app.js", import.meta.url), "utf8");

// Minimal DOM stub so app.js loads without a browser.
const docStub = { addEventListener() {} };
const ctx = vm.createContext({ document: docStub });
vm.runInContext(
  appJs + "\nthis.__exports = { crc32, buildConfig, toUf2, CONFIG_FLASH_OFFSET, UF2_FLASH_BASE, OS, PRANK };",
  ctx);
const { buildConfig, toUf2, CONFIG_FLASH_OFFSET, UF2_FLASH_BASE, OS, PRANK } = ctx.__exports;

// Windows + Linux targets; Fake BSOD + Fake Update; default delay.
const delayS = Number(process.argv[2] || 3);
const targets = OS.windows | OS.linux;
const pranks = PRANK.bsod | PRANK.update;

const cfg = buildConfig(targets, pranks, delayS * 1000);
const uf2 = toUf2(cfg, UF2_FLASH_BASE + CONFIG_FLASH_OFFSET);

const out = new URL("./sample-config.uf2", import.meta.url);
writeFileSync(out, Buffer.from(uf2));
console.log(`wrote ${out.pathname} (${uf2.length} bytes)`);
