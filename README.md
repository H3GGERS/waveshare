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
