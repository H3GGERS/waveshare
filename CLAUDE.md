# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP-IDF (v5.4.2) project for the Waveshare ESP32-S3-RLCD-4.2 — a 4.2-inch round LCD display (300×400px in landscape). Displays a WiFi-synced clock with date. Written in C++ using LVGL v9.5.0 for UI.

## Build Commands

Requires ESP-IDF v5.4.2 to be activated. On Windows, open "ESP-IDF 5.4 PowerShell" from the Start menu, or activate in the current terminal:

```powershell
& "C:\Espressif\frameworks\esp-idf-v5.4.2\export.ps1"
```

```bash
idf.py set-target esp32s3   # only needed once
idf.py menuconfig            # configure WiFi SSID/password under "Clock / WiFi Configuration"
idf.py build
idf.py flash monitor         # flash and open serial monitor
```

The helper script `idf-build.ps1` wraps activation + build for convenience.

## Architecture

Three components with clear separation:

**`components/display_bsp/`** — Hardware abstraction for the SPI LCD. `DisplayPort` class owns the SPI driver, builds pixel-address LUTs for 2-bit color in landscape mode, and exposes `RLCD_SetPixel()` / `RLCD_Display()`. All SPI writes happen here.

**`components/lvgl_port/`** — Bridges LVGL to the display and FreeRTOS. `Lvgl_PortInit()` allocates dual SPIRAM buffers, registers the flush callback, and runs LVGL's internal timer loop. Provides `Lvgl_lock()` / `Lvgl_unlock()` for thread-safe UI access from other tasks.

**`main/main.cpp`** — Application logic. Initializes NVS → WiFi → SNTP → display → LVGL → UI layout. A 1-second FreeRTOS timer calls `update_clock_cb()`, which reads system time and updates LVGL labels under the mutex.

### Display Data Path

```
update_clock_cb (1s timer)
  → lv_label_set_text() [LVGL label update]
  → lvgl_flush_cb() [called by LVGL internally]
  → per-pixel: RGB565 threshold (0x7fff) → black/white
  → RlcdPort.RLCD_SetPixel() via LUT
  → RlcdPort.RLCD_Display() → SPI write to LCD
```

### Key Configuration

- **WiFi credentials:** `idf.py menuconfig` → "Clock / WiFi Configuration"
- **Timezone:** hardcoded in `main/main.cpp` line ~133 — `setenv("TZ", "MST7MDT", 1)`. Common values: `PST8PDT`, `EST5EDT`
- **Flash/PSRAM:** 16 MB flash, 8 MB Octal SPI PSRAM — configured in `sdkconfig.defaults`
- **Custom partition table:** `partitions.csv` (NVS at 0x9000, app at 0x10000, 3 MB)

### Hardware Pins (ESP32-S3 GPIO)

| GPIO | Function |
|------|----------|
| 12   | SPI MOSI |
| 11   | SPI SCL  |
| 5    | LCD DC   |
| 40   | LCD CS   |
| 41   | LCD RST  |

### Fonts

LVGL Montserrat 24pt and 48pt are pre-enabled in `sdkconfig.defaults`. A custom Inter 24pt Bold font lives in `main/fonts/Inter_24pt_Bold.c` — see `main/fonts/README.md` for how to generate additional fonts with the LVGL font converter.

### UI Layout Notes

The time display uses a fixed-width 320px container with the digit label left-aligned and AM/PM right-aligned as a separate label. This prevents layout shift as digit widths change. The color threshold `0x7fff` in `lvgl_flush_cb()` controls black/white rendering — the display is effectively 1-bit (black/white only despite 2-bit hardware support).
