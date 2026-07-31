#include <stdio.h>
#include "pico/stdlib.h"
#include "script.h"
#include "keys.h"
#include "config.h"
#include "payloads.h"

/* Event initializer helpers. */
#define KEY(m, u)    { OP_KEY, (m), (u), 0, NULL }
#define STR(s)       { OP_STRING, 0, 0, 0, (s) }
#define DLY(ms)      { OP_DELAY, 0, 0, (ms), NULL }
#define ENDEV        { OP_END, 0, 0, 0, NULL }

/* ------------------------------------------------------------------ */
/* Windows                                                             */
/* ------------------------------------------------------------------ */

/* Fullscreen fake blue screen of death. */
static const script_evt_t seq_win_bsod[] = {
    KEY(KEY_GUI, KEY_R),
    DLY(700),
    STR("powershell"),
    KEY(0, KEY_ENTER),
    DLY(2500),
    KEY(KEY_ALT, KEY_ENTER),                    /* fullscreen the console */
    DLY(600),
    STR("$h=$Host.UI.RawUI;$h.BackgroundColor='Blue';$h.ForegroundColor='White';cls;"
        "Write-Host '';Write-Host '  :(' -f White;Write-Host '';"
        "Write-Host '  Your PC ran into a problem and needs to restart.' -f White;"
        "Write-Host '  We are just collecting some error info, and then' -f White;"
        "Write-Host '  we will restart for you.' -f White;"
        "Write-Host '';Write-Host '';"
        "Write-Host '  Stop code: CRITICAL_PROCESS_DIED' -f White;"
        "Start-Sleep 99999"),
    KEY(0, KEY_ENTER),
    ENDEV
};

/* Fullscreen fake Windows Update progress screen. */
static const script_evt_t seq_win_update[] = {
    KEY(KEY_GUI, KEY_R),
    DLY(700),
    STR("powershell"),
    KEY(0, KEY_ENTER),
    DLY(2500),
    KEY(KEY_ALT, KEY_ENTER),
    DLY(600),
    STR("$h=$Host.UI.RawUI;$h.BackgroundColor='Blue';cls;"
        "1..100|%{cls;Write-Host ('Windows Update '+$_+'%') -f White -b Blue;"
        "Write-Host '';Write-Host 'Do not turn off your computer' -f White -b Blue;"
        "Start-Sleep -m 300}"),
    KEY(0, KEY_ENTER),
    ENDEV
};

/* Release the DHCP lease on every adapter (reversible, no damage). */
static const script_evt_t seq_win_network[] = {
    KEY(KEY_GUI, KEY_R),
    DLY(700),
    STR("cmd /c ipconfig /release&ipconfig /release6"),
    KEY(0, KEY_ENTER),
    ENDEV
};

static const script_evt_t seq_win_shutdown[] = {
    KEY(KEY_GUI, KEY_R),
    DLY(700),
    STR("shutdown /s /t 0"),
    KEY(0, KEY_ENTER),
    ENDEV
};

/* ------------------------------------------------------------------ */
/* Linux                                                               */
/* ------------------------------------------------------------------ */

static const script_evt_t seq_linux_update[] = {
    KEY(KEY_CTRL | KEY_ALT, KEY_T),             /* open a terminal */
    DLY(1500),
    STR("while true; do clear; printf $'\\033[44m\\033[37m Updating 42%%\\n Do not turn off your computer'; sleep 1; done"),
    KEY(0, KEY_ENTER),
    ENDEV
};

static const script_evt_t seq_linux_network[] = {
    KEY(KEY_CTRL | KEY_ALT, KEY_T),
    DLY(1500),
    STR("nmcli networking off"),
    KEY(0, KEY_ENTER),
    ENDEV
};

static const script_evt_t seq_linux_shutdown[] = {
    KEY(KEY_CTRL | KEY_ALT, KEY_T),
    DLY(1500),
    STR("poweroff"),
    KEY(0, KEY_ENTER),
    ENDEV
};

/* ------------------------------------------------------------------ */
/* macOS                                                               */
/* ------------------------------------------------------------------ */

static const script_evt_t seq_macos_update[] = {
    KEY(KEY_GUI, KEY_SPACE),                    /* Spotlight */
    DLY(900),
    STR("Terminal"),
    KEY(0, KEY_ENTER),
    DLY(2000),
    STR("while true; do clear; printf $'\\033[44m\\033[37m Updating 42%%\\n Do not turn off your computer'; sleep 1; done"),
    KEY(0, KEY_ENTER),
    ENDEV
};

static const script_evt_t seq_macos_network[] = {
    KEY(KEY_GUI, KEY_SPACE),
    DLY(900),
    STR("networksetup -setairportpower en0 off"),
    KEY(0, KEY_ENTER),
    ENDEV
};

/* Display sleep: looks like the machine died, but is 100% harmless. */
static const script_evt_t seq_macos_shutdown[] = {
    KEY(KEY_GUI, KEY_SPACE),
    DLY(900),
    STR("pmset displaysleepnow"),
    KEY(0, KEY_ENTER),
    ENDEV
};

/* ------------------------------------------------------------------ */
/* Lookup                                                              */
/* ------------------------------------------------------------------ */

const script_evt_t *payload_get(uint32_t prank, uint32_t os)
{
    if (os == OS_WINDOWS) {
        switch (prank) {
            case PRANK_BSOD:     return seq_win_bsod;
            case PRANK_UPDATE:   return seq_win_update;
            case PRANK_NETWORK:  return seq_win_network;
            case PRANK_SHUTDOWN: return seq_win_shutdown;
        }
    } else if (os == OS_LINUX) {
        switch (prank) {
            case PRANK_UPDATE:   return seq_linux_update;
            case PRANK_NETWORK:  return seq_linux_network;
            case PRANK_SHUTDOWN: return seq_linux_shutdown;
        }
    } else if (os == OS_MACOS) {
        switch (prank) {
            case PRANK_UPDATE:   return seq_macos_update;
            case PRANK_NETWORK:  return seq_macos_network;
            case PRANK_SHUTDOWN: return seq_macos_shutdown;
        }
    }
    return NULL;
}

void payload_run_all(uint32_t targets, uint32_t pranks)
{
    static const uint32_t prank_order[] = {
        PRANK_BSOD, PRANK_UPDATE, PRANK_NETWORK, PRANK_SHUTDOWN
    };
    static const uint32_t os_order[] = {
        OS_WINDOWS, OS_LINUX, OS_MACOS, OS_ANDROID
    };

    for (size_t pi = 0; pi < 4; pi++) {
        if (!(pranks & prank_order[pi])) continue;
        for (size_t oi = 0; oi < 4; oi++) {
            if (!(targets & os_order[oi])) continue;
            const script_evt_t *seq = payload_get(prank_order[pi], os_order[oi]);
            if (!seq) {
                printf("PicoKiller: no sequence for prank 0x%02lx os 0x%02lx\n",
                       (unsigned long)prank_order[pi], (unsigned long)os_order[oi]);
                continue;
            }
            printf("PicoKiller: running prank 0x%02lx on os 0x%02lx\n",
                   (unsigned long)prank_order[pi], (unsigned long)os_order[oi]);
            script_run(seq);
            sleep_ms(500);
        }
    }
}
