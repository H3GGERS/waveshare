#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare ESP32-S3-RLCD-4.2 starting...");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
