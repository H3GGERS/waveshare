# Custom fonts

Put your LVGL font `.c` file here (from the [LVGL font converter](https://lvgl.io/tools/fontconverter)).

- Name it `font_custom.c`, **or**
- Edit `main/CMakeLists.txt` and replace `font_custom.c` in `SRCS` with your file name.

Then `#include "font_custom.h"` (or your header) in `main.cpp` and use the font descriptor (e.g. `&lv_font_xxx`) in `lv_obj_set_style_text_font()`.
