/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --font C:\Users\mrheg\Downloads\Inter\static\Inter_18pt-Bold.ttf --size 12 --bpp 1 --format lvgl --range 0x20-0x7F -o C:\Users\mrheg\dev\waveshare\main\fonts\Inter_12pt_Bold.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef INTER_12PT_BOLD
#define INTER_12PT_BOLD 1
#endif

#if INTER_12PT_BOLD

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xf3, 0xc0,

    /* U+0022 "\"" */
    0xff, 0xff,

    /* U+0023 "#" */
    0x36, 0x26, 0x7f, 0x24, 0x24, 0xfe, 0x6c, 0x6c,
    0x48,

    /* U+0024 "$" */
    0x10, 0xfb, 0x5e, 0x8f, 0xf, 0x87, 0xeb, 0xd6,
    0xf8, 0x40,

    /* U+0025 "%" */
    0x61, 0x24, 0xc9, 0x61, 0x98, 0xd, 0x86, 0x91,
    0xa4, 0xc9, 0x61, 0x80,

    /* U+0026 "&" */
    0x78, 0xcc, 0xdc, 0xf8, 0x70, 0xfa, 0xde, 0xcc,
    0x7e,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x6f, 0x6d, 0xb6, 0xcd, 0x80,

    /* U+0029 ")" */
    0xd9, 0x36, 0xdb, 0x7b, 0x0,

    /* U+002A "*" */
    0x27, 0xc9, 0xf2, 0x0,

    /* U+002B "+" */
    0x18, 0x30, 0x67, 0xf1, 0x83, 0x6, 0x0,

    /* U+002C "," */
    0xea,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x11, 0x33, 0x22, 0x66, 0x4c, 0xc0,

    /* U+0030 "0" */
    0x38, 0xdb, 0x1e, 0x3c, 0x78, 0xf1, 0xb6, 0x38,

    /* U+0031 "1" */
    0x7f, 0xb3, 0x33, 0x33, 0x30,

    /* U+0032 "2" */
    0x7b, 0x38, 0xc3, 0x1c, 0xe7, 0x18, 0xfc,

    /* U+0033 "3" */
    0x7d, 0x8f, 0x18, 0x33, 0x80, 0xc1, 0xe3, 0x7c,

    /* U+0034 "4" */
    0x1c, 0x38, 0xf3, 0x66, 0xd9, 0xbf, 0x86, 0xc,

    /* U+0035 "5" */
    0x7c, 0x81, 0x3, 0xe4, 0x60, 0xd1, 0xb3, 0x3c,

    /* U+0036 "6" */
    0x3c, 0xcf, 0x7, 0xee, 0xf8, 0xf1, 0xa7, 0x3c,

    /* U+0037 "7" */
    0xfc, 0x31, 0x86, 0x10, 0xc3, 0x18, 0x60,

    /* U+0038 "8" */
    0x7d, 0x8f, 0x1e, 0x33, 0x98, 0xf1, 0xe3, 0x7c,

    /* U+0039 "9" */
    0x79, 0x8b, 0x1e, 0x37, 0xe0, 0xf1, 0xb6, 0x38,

    /* U+003A ":" */
    0xf0, 0x3c,

    /* U+003B ";" */
    0xf0, 0xe, 0xa0,

    /* U+003C "<" */
    0xc, 0xff, 0x30, 0x78, 0x70, 0x40,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0xc3, 0xc3, 0xc3, 0x7b, 0x88, 0x0,

    /* U+003F "?" */
    0x7b, 0x30, 0xc7, 0x38, 0xc0, 0xc, 0x30,

    /* U+0040 "@" */
    0x1f, 0x6, 0x39, 0x83, 0x6f, 0xff, 0x1f, 0xe3,
    0xfc, 0x7f, 0x8f, 0xde, 0xcc, 0x0, 0xc0, 0xf,
    0xc0,

    /* U+0041 "A" */
    0x1c, 0xe, 0xd, 0x86, 0xc3, 0x63, 0x19, 0xfc,
    0xc6, 0xc1, 0x80,

    /* U+0042 "B" */
    0xf9, 0x9b, 0x36, 0x6f, 0x98, 0xf1, 0xe3, 0xfc,

    /* U+0043 "C" */
    0x3c, 0x63, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x62,
    0x3c,

    /* U+0044 "D" */
    0xf9, 0x9b, 0x1e, 0x3c, 0x78, 0xf1, 0xe6, 0xf8,

    /* U+0045 "E" */
    0xff, 0xc, 0x30, 0xff, 0xc, 0x30, 0xfc,

    /* U+0046 "F" */
    0xff, 0xc, 0x30, 0xff, 0xc, 0x30, 0xc0,

    /* U+0047 "G" */
    0x3c, 0x66, 0xc3, 0xc0, 0xcf, 0xc3, 0xc3, 0x66,
    0x3c,

    /* U+0048 "H" */
    0xc7, 0x8f, 0x1e, 0x3f, 0xf8, 0xf1, 0xe3, 0xc6,

    /* U+0049 "I" */
    0xff, 0xff, 0xc0,

    /* U+004A "J" */
    0xc, 0x30, 0xc3, 0xc, 0x3c, 0xf3, 0x78,

    /* U+004B "K" */
    0xc6, 0xcc, 0xdc, 0xd8, 0xf8, 0xf8, 0xcc, 0xc6,
    0xc6,

    /* U+004C "L" */
    0xc3, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xfc,

    /* U+004D "M" */
    0xe1, 0xf8, 0x7f, 0x3f, 0xcf, 0xd2, 0xf7, 0xbd,
    0xef, 0x33, 0xcc, 0xc0,

    /* U+004E "N" */
    0xe3, 0xe3, 0xf3, 0xd3, 0xdb, 0xcf, 0xcf, 0xc7,
    0xc7,

    /* U+004F "O" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66,
    0x3c,

    /* U+0050 "P" */
    0xfd, 0x8f, 0x1e, 0x3c, 0xff, 0xb0, 0x60, 0xc0,

    /* U+0051 "Q" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xcb, 0x6e,
    0x3e, 0x3,

    /* U+0052 "R" */
    0xfd, 0x8f, 0x1e, 0x3f, 0xdb, 0x33, 0x66, 0xc6,

    /* U+0053 "S" */
    0x7d, 0x8f, 0x7, 0x87, 0xc3, 0xf1, 0xe3, 0x7c,

    /* U+0054 "T" */
    0xfe, 0x30, 0x60, 0xc1, 0x83, 0x6, 0xc, 0x18,

    /* U+0055 "U" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xe3, 0x3c,

    /* U+0056 "V" */
    0xc3, 0x31, 0x98, 0xcc, 0x43, 0x61, 0xb0, 0xd0,
    0x38, 0x1c, 0x0,

    /* U+0057 "W" */
    0xc6, 0x36, 0x63, 0x67, 0x26, 0xf6, 0x6d, 0x62,
    0xd6, 0x39, 0xc3, 0x9c, 0x38, 0xc0,

    /* U+0058 "X" */
    0x63, 0x1b, 0xd, 0x83, 0x81, 0xc0, 0xe0, 0xd8,
    0xce, 0x63, 0x0,

    /* U+0059 "Y" */
    0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x18,
    0x18,

    /* U+005A "Z" */
    0xfe, 0xc, 0x30, 0xc3, 0x86, 0x18, 0x60, 0xfe,

    /* U+005B "[" */
    0xfb, 0x6d, 0xb6, 0xdb, 0x80,

    /* U+005C "\\" */
    0xc4, 0x46, 0x62, 0x23, 0x31, 0x10,

    /* U+005D "]" */
    0xed, 0xb6, 0xdb, 0x6f, 0x80,

    /* U+005E "^" */
    0x31, 0x9e, 0x9c, 0x80,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0xd0,

    /* U+0061 "a" */
    0x7b, 0x31, 0xff, 0xcf, 0x37, 0xc0,

    /* U+0062 "b" */
    0xc3, 0xf, 0xb3, 0xcf, 0x3c, 0xf3, 0xf8,

    /* U+0063 "c" */
    0x7b, 0xbc, 0x30, 0xc3, 0xb7, 0x80,

    /* U+0064 "d" */
    0xc, 0x37, 0xf3, 0xcf, 0x3c, 0xf3, 0x7c,

    /* U+0065 "e" */
    0x7b, 0x3c, 0xff, 0xc3, 0x37, 0x80,

    /* U+0066 "f" */
    0x33, 0x3c, 0xc6, 0x31, 0x8c, 0x60,

    /* U+0067 "g" */
    0x7f, 0x3c, 0xf3, 0xcf, 0x37, 0xc3, 0xcd, 0xe0,

    /* U+0068 "h" */
    0xc3, 0xd, 0xbb, 0xcf, 0x3c, 0xf3, 0xcc,

    /* U+0069 "i" */
    0xff, 0xff, 0xc0,

    /* U+006A "j" */
    0x6d, 0xb6, 0xdb, 0x6d, 0xe0,

    /* U+006B "k" */
    0xc3, 0xc, 0xf6, 0xf3, 0xcd, 0xb6, 0xcc,

    /* U+006C "l" */
    0xff, 0xff, 0xc0,

    /* U+006D "m" */
    0xfb, 0xb3, 0x3c, 0xcf, 0x33, 0xcc, 0xf3, 0x3c,
    0xcc,

    /* U+006E "n" */
    0xdb, 0xbc, 0xf3, 0xcf, 0x3c, 0xc0,

    /* U+006F "o" */
    0x7b, 0x3c, 0xf3, 0xcf, 0x37, 0x80,

    /* U+0070 "p" */
    0xfb, 0x3c, 0xf3, 0xcf, 0x3f, 0xb0, 0xc3, 0x0,

    /* U+0071 "q" */
    0x7f, 0x3c, 0xf3, 0xcf, 0x37, 0xc3, 0xc, 0x30,

    /* U+0072 "r" */
    0xfc, 0xcc, 0xcc, 0xc0,

    /* U+0073 "s" */
    0x7b, 0x2e, 0x3e, 0x1b, 0x27, 0x80,

    /* U+0074 "t" */
    0x66, 0xf6, 0x66, 0x66, 0x30,

    /* U+0075 "u" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x37, 0xc0,

    /* U+0076 "v" */
    0xc6, 0x89, 0xb3, 0x62, 0x87, 0xe, 0x0,

    /* U+0077 "w" */
    0xcc, 0xd3, 0x36, 0xc9, 0xfe, 0x7b, 0x8c, 0xc3,
    0x30,

    /* U+0078 "x" */
    0x66, 0xd8, 0xe0, 0xc3, 0xcd, 0x99, 0x80,

    /* U+0079 "y" */
    0xc6, 0x89, 0xb3, 0x62, 0x85, 0xe, 0x8, 0x70,
    0xc0,

    /* U+007A "z" */
    0xf8, 0xcc, 0x66, 0x63, 0xe0,

    /* U+007B "{" */
    0x36, 0x66, 0x6c, 0x66, 0x66, 0x30,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xfc,

    /* U+007D "}" */
    0xc6, 0x66, 0x63, 0x66, 0x66, 0xc0,

    /* U+007E "~" */
    0x65, 0xba, 0x30
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 45, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 61, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 103, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 6, .adv_w = 124, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 126, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 25, .adv_w = 192, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 128, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 62, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 47, .adv_w = 71, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 52, .adv_w = 71, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 57, .adv_w = 106, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 61, .adv_w = 129, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 68, .adv_w = 60, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 69, .adv_w = 89, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 70, .adv_w = 60, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 71, .adv_w = 74, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 77, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 81, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 90, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 123, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 129, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 119, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 123, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 110, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 123, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 123, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 60, .box_w = 2, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 62, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 157, .adv_w = 129, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 129, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 166, .adv_w = 129, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 108, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 179, .adv_w = 195, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 196, .adv_w = 142, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 126, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 141, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 138, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 117, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 239, .adv_w = 113, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 143, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 142, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 263, .adv_w = 53, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 266, .adv_w = 111, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 273, .adv_w = 137, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 108, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 177, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 145, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 147, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 124, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 327, .adv_w = 148, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 337, .adv_w = 126, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 126, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 353, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 139, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 142, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 380, .adv_w = 197, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 394, .adv_w = 140, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 139, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 126, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 71, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 427, .adv_w = 74, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 433, .adv_w = 71, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 438, .adv_w = 92, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 442, .adv_w = 91, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 443, .adv_w = 67, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 444, .adv_w = 110, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 112, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 463, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 470, .adv_w = 113, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 476, .adv_w = 75, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 482, .adv_w = 120, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 490, .adv_w = 118, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 51, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 51, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 505, .adv_w = 110, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 51, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 515, .adv_w = 174, .box_w = 10, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 118, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 530, .adv_w = 116, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 536, .adv_w = 120, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 544, .adv_w = 120, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 552, .adv_w = 77, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 106, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 70, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 567, .adv_w = 118, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 573, .adv_w = 113, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 580, .adv_w = 162, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 589, .adv_w = 110, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 596, .adv_w = 114, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 605, .adv_w = 108, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 610, .adv_w = 89, .box_w = 4, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 616, .adv_w = 70, .box_w = 2, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 620, .adv_w = 89, .box_w = 4, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 626, .adv_w = 129, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    3, 7,
    3, 13,
    3, 15,
    3, 21,
    5, 24,
    7, 3,
    7, 8,
    7, 61,
    8, 7,
    8, 13,
    8, 15,
    8, 21,
    9, 93,
    11, 7,
    11, 13,
    11, 15,
    11, 21,
    11, 33,
    11, 64,
    12, 19,
    12, 20,
    12, 24,
    12, 61,
    13, 1,
    13, 2,
    13, 3,
    13, 8,
    13, 17,
    13, 18,
    13, 19,
    13, 20,
    13, 22,
    13, 23,
    13, 24,
    13, 25,
    13, 26,
    13, 32,
    13, 33,
    14, 19,
    14, 20,
    14, 24,
    14, 61,
    15, 1,
    15, 2,
    15, 3,
    15, 8,
    15, 17,
    15, 18,
    15, 19,
    15, 20,
    15, 22,
    15, 23,
    15, 24,
    15, 25,
    15, 26,
    15, 32,
    15, 33,
    16, 13,
    16, 15,
    16, 61,
    17, 13,
    17, 15,
    17, 24,
    17, 61,
    17, 64,
    18, 10,
    18, 62,
    18, 64,
    18, 94,
    19, 21,
    20, 11,
    20, 13,
    20, 15,
    20, 63,
    21, 11,
    21, 13,
    21, 15,
    21, 18,
    21, 24,
    21, 63,
    22, 11,
    22, 13,
    22, 15,
    22, 18,
    22, 63,
    23, 13,
    23, 15,
    23, 64,
    24, 4,
    24, 6,
    24, 7,
    24, 11,
    24, 13,
    24, 15,
    24, 17,
    24, 20,
    24, 21,
    24, 22,
    24, 23,
    24, 24,
    24, 25,
    24, 26,
    24, 27,
    24, 28,
    24, 29,
    24, 63,
    24, 64,
    25, 11,
    25, 13,
    25, 15,
    25, 63,
    26, 13,
    26, 15,
    26, 24,
    26, 61,
    26, 64,
    27, 61,
    28, 61,
    30, 61,
    31, 24,
    31, 61,
    33, 13,
    33, 15,
    33, 16,
    33, 61,
    33, 64,
    60, 93,
    61, 3,
    61, 5,
    61, 8,
    61, 11,
    61, 12,
    61, 14,
    61, 16,
    61, 17,
    61, 18,
    61, 23,
    61, 29,
    61, 30,
    61, 32,
    61, 33,
    61, 61,
    61, 63,
    61, 95,
    63, 7,
    63, 13,
    63, 15,
    63, 21,
    63, 33,
    63, 64,
    64, 11,
    64, 17,
    64, 18,
    64, 20,
    64, 21,
    64, 22,
    64, 23,
    64, 25,
    64, 26,
    64, 33,
    64, 61,
    64, 63,
    64, 93,
    92, 93,
    93, 10,
    93, 62,
    93, 64,
    93, 94,
    95, 19,
    95, 20,
    95, 24,
    95, 61
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -6, -17, -17, -12, 0, -6, -6, -12,
    -6, -17, -17, -12, 3, -6, -26, -26,
    -9, -3, -13, -4, -1, -5, -9, -6,
    -2, -19, -19, -6, -18, 0, -4, -1,
    -6, -5, -5, -2, -19, -8, -4, -1,
    -5, -9, -6, -2, -19, -19, -6, -18,
    0, -4, -1, -6, -5, -5, -2, -19,
    -8, -8, -8, 2, -6, -6, -4, -6,
    -9, 3, 3, 3, 3, -3, -2, -4,
    -4, -2, -8, -6, -6, -8, -2, -8,
    -2, -4, -4, -1, -2, -6, -6, -9,
    -11, 1, -9, 1, -24, -24, -3, -3,
    -11, -2, -3, 4, -3, -2, -6, -6,
    -17, 1, -30, -2, -4, -4, -2, -6,
    -6, -4, -6, -9, -12, -12, -13, -14,
    -14, -8, -8, -8, -8, -8, 3, -15,
    -3, -15, -18, -6, -6, 2, -1, -12,
    -1, -7, -10, -12, -8, -11, -18, -6,
    -6, -26, -26, -9, -3, -13, -13, -9,
    -21, -9, -11, -9, -9, -9, -9, -8,
    -15, -13, 5, 3, 3, 3, 3, 3,
    -4, -1, -5, -9
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 172,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t Inter_12pt_Bold = {
#else
lv_font_t Inter_12pt_Bold = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if INTER_12PT_BOLD*/

