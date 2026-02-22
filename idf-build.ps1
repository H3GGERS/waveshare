# Build this project using ESP-IDF. Run from project root.
# Usage: .\idf-build.ps1 [build|flash|monitor|flash monitor|...]
#
# Prefer using "ESP-IDF 5.4 PowerShell" from the Start menu, then:
#   cd c:\Users\mrheg\dev\waveshare
#   idf.py build
#   idf.py flash monitor
#
# This script tries to activate IDF from C:\Espressif if idf.py is not in PATH.
# If activation fails (e.g. Python env not found), use the Start menu shortcut above.

$IdfRoot = "C:\Espressif\frameworks\esp-idf-v5.4.2"
if (-not (Get-Command idf.py -ErrorAction SilentlyContinue)) {
    if (Test-Path "$IdfRoot\export.ps1") {
        Write-Host "Activating ESP-IDF from $IdfRoot ..."
        Push-Location $IdfRoot
        try {
            & .\export.ps1
        } finally {
            Pop-Location
        }
    }
}

if (-not (Get-Command idf.py -ErrorAction SilentlyContinue)) {
    Write-Host ""
    Write-Host "idf.py is not available. Do one of the following:"
    Write-Host "  1. Open 'ESP-IDF 5.4 PowerShell' from the Start menu, then run: idf.py build"
    Write-Host "  2. Or run the ESP-IDF Tools installer to repair the Python environment."
    exit 1
}

$target = if ($args.Count -gt 0) { $args } else { @("build") }
& idf.py @target
