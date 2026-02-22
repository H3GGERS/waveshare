/**
 * ESP32-S3 4.2" RLCD - Waveshare demo-style app
 * Based on: Waveshare 09_LVGL_V9_Test (ESP32-S3-RLCD-4.2)
 * Flush: update buffer in callback, then RLCD_Display() once per flush (no separate refresh task).
 */

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>

#include "display_bsp.h"
#include "lvgl_bsp.h"
#include "lvgl.h"

static const char *TAG = "main";

// Pin assignments for Waveshare ESP32-S3-RLCD-4.2 (300×400 landscape)
#define LCD_PIN_MOSI  12
#define LCD_PIN_SCL   11
#define LCD_PIN_DC    5
#define LCD_PIN_CS    40
#define LCD_PIN_RST   41

DisplayPort RlcdPort(LCD_PIN_MOSI, LCD_PIN_SCL, LCD_PIN_DC, LCD_PIN_CS, LCD_PIN_RST,
                     LCD_WIDTH, LCD_HEIGHT);

// Flush: write LVGL area into RLCD buffer, push to display once, then signal ready.
// This matches the official Waveshare 09_LVGL_V9_Test and avoids flicker from a separate refresh task.
static void lvgl_flush_cb(lv_display_t *drv, const lv_area_t *area, uint8_t *color_map)
{
    uint16_t *buf = (uint16_t *)color_map;
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            uint8_t color = (*buf < 0x7fff) ? ColorBlack : ColorWhite;
            RlcdPort.RLCD_SetPixel(x, y, color);
            buf++;
        }
    }
    RlcdPort.RLCD_Display();
    lv_display_flush_ready(drv);
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare ESP32-S3-RLCD-4.2 starting...");

    RlcdPort.RLCD_Init();
    Lvgl_PortInit(LCD_WIDTH, LCD_HEIGHT, lvgl_flush_cb);

    if (Lvgl_lock(-1)) {
        lv_obj_t *label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Hello Kyle Hegge");
        lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
        lv_obj_center(label);
        Lvgl_unlock();
    }

    ESP_LOGI(TAG, "Display initialized");
}
