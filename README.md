# Waveshare ESP32-S3-RLCD-4.2

ESP-IDF project for the Waveshare ESP32-S3 Round LCD 4.2" display.

## Requirements

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/) v5.x
- ESP32-S3 target

## Setup

### Activating ESP-IDF in PowerShell (Windows)

`idf.py` is only available after activating the ESP-IDF environment. In a **new** PowerShell window, run **one** of these:

**Option A – Use the ESP-IDF shortcut**  
Open **“ESP-IDF 5.4 PowerShell”** (or your version) from the Start menu, then:

```powershell
cd c:\Users\mrheg\dev\waveshare
idf.py build
idf.py flash monitor
```

**Option B – Activate in this terminal**  
If you’re already in a normal PowerShell in the project folder:

```powershell
# Activate ESP-IDF (run once per session; path may vary)
& "C:\Espressif\frameworks\esp-idf-v5.4.2\export.ps1"

# Then build / flash
idf.py build
idf.py flash monitor
```

If you see an error about the Python virtual environment not found, use **Option A** (the “ESP-IDF 5.4 PowerShell” shortcut) or run the ESP-IDF Tools installer to repair the environment.

### Build commands

```bash
idf.py set-target esp32s3
idf.py menuconfig
idf.py build
idf.py flash monitor
```

### Clock (WiFi + NTP)

The clock shows “Hello Kyle, its Sunday, February 22nd” and the time in `HH:MM:SS` with seconds updating every second. To get real time:

1. Run `idf.py menuconfig`.
2. Open **“Clock / WiFi Configuration”**.
3. Set **WiFi SSID** and **WiFi Password** to your network.
4. Build and flash.

Time is synced from NTP (pool.ntp.org). The default timezone is US Central (`CST6CDT`). To change it, edit the `setenv("TZ", ...)` line in `main/main.cpp` (e.g. `PST8PDT` for Pacific, `EST5EDT` for Eastern).

### Chess.com daily board

The display now uses the large lower area for an 8x8 chess board sourced from one selected Chess.com daily game.

1. Run `idf.py menuconfig`.
2. Open **Clock / WiFi Configuration**.
3. Set:
   - **Chess.com Username** (default: `H3GGERS`)
   - **Chess refresh interval (seconds)** (default: `60`)
4. Build and flash.

After WiFi connects, the ESP32 hosts a small game selector page on its local IP:

- Open `http://<device-ip>/` from your phone/laptop on the same network.
- Choose the daily game from the dropdown and submit.
- Selection is stored in NVS and survives reboot.
- `http://<device-ip>/refresh` triggers an immediate fetch.

Notes:
- Board state comes from Chess.com FEN data.
- Last-move highlight is best-effort (if a coordinate-style move token is present in PGN text).
- If Chess.com is unreachable, the last rendered board stays on screen and status text updates.

### Chess sprite pipeline (Figma -> firmware)

Board squares are rendered from prebuilt sprite tiles (piece + square baked together) for crisp, deterministic output on this monochrome panel.

Export spec:
- 26 tiles total in this order: white/black king, queen, bishop, knight, rook, pawn (each on light and dark), then empty light/dark.
- Tile size target: use native export size (`30x30` currently).
- Use pure black/white in Figma and snap shapes to whole pixels.
- Export individual PNGs into `chess_pieces/` using Figma's variant naming.

Generate C assets:

```powershell
python tools/generate_chess_sprites.py `
  --source-dir "chess_pieces" `
  --out-dir "main/chess_sprites" `
  --threshold 128 `
  --renamed-png-dir "chess_pieces/normalized"
```

Optional: pass `--tile-size <n>` only if you intentionally want resampling.

The generator emits:
- `main/chess_sprites/chess_sprites.h`
- `main/chess_sprites/chess_sprites.c`
- Optional normalized filenames like `wk_light.png`, `bp_dark.png` under `chess_pieces/normalized`.

These are compiled into firmware and selected per square at runtime.
