#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_timer.h>
#include "lvgl_bsp.h"

static const char *TAG = "LvglPort";
static SemaphoreHandle_t lvgl_mux = NULL;

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))

static void lvgl_tick_cb(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

bool Lvgl_lock(int timeout_ms)
{
    const TickType_t ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, ticks) == pdTRUE;
}

void Lvgl_unlock(void)
{
    assert(lvgl_mux && "Lvgl_PortInit must be called first");
    xSemaphoreGive(lvgl_mux);
}

static void lvgl_task(void *arg)
{
    uint32_t delay_ms = LVGL_TASK_MAX_DELAY_MS;
    for (;;) {
        if (Lvgl_lock(-1)) {
            delay_ms = lv_timer_handler();
            Lvgl_unlock();
        }
        if (delay_ms > LVGL_TASK_MAX_DELAY_MS) delay_ms = LVGL_TASK_MAX_DELAY_MS;
        if (delay_ms < LVGL_TASK_MIN_DELAY_MS) delay_ms = LVGL_TASK_MIN_DELAY_MS;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void Lvgl_PortInit(int width, int height, DispFlushCb flush_cb)
{
    lvgl_mux = xSemaphoreCreateMutex();
    lv_init();

    lv_display_t *disp = lv_display_create(width, height);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, flush_cb);

    size_t buf_size = width * height * BYTES_PER_PIXEL;
    uint8_t *buf1 = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    uint8_t *buf2 = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    assert(buf1 && buf2);

    lv_display_set_buffers(disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_FULL);

    ESP_LOGI(TAG, "Installing LVGL tick timer");
    esp_timer_create_args_t tick_args = {};
    tick_args.callback = lvgl_tick_cb;
    tick_args.name     = "lvgl_tick";
    esp_timer_handle_t tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&tick_args, &tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    xTaskCreatePinnedToCore(lvgl_task, "lvgl", 8 * 1024, NULL, 5, NULL, 0);
    ESP_LOGI(TAG, "LVGL port ready (%dx%d)", width, height);
}
