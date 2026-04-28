# Custom fonts

Put your LVGL font `.c` file here (from the [LVGL font converter](https://lvgl.io/tools/fontconverter)).

- Name it `font_custom.c`, **or**
- Edit `main/CMakeLists.txt` and replace `font_custom.c` in `SRCS` with your file name.

Then `#include "font_custom.h"` (or your header) in `main.cpp` and use the font descriptor (e.g. `&lv_font_xxx`) in `lv_obj_set_style_text_font()`.

## Chess glyph font (Noto Sans Symbols 2)

For chess Unicode glyphs (`U+2654`..`U+265F`), generate a font file and descriptor named:

- File: `NotoSansSymbols2_24.c`
- Symbol: `NotoSansSymbols2_24`

Then add the file to `main/CMakeLists.txt` under `SRCS`:

- `"fonts/NotoSansSymbols2_24.c"`

The app already tries to use `NotoSansSymbols2_24` for board pieces and falls back to Montserrat if the symbol is not linked.
