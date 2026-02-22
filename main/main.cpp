/**
 * ESP32-S3 4.2" RLCD - Clock + date view
 * Shows: "Hello Kyle, its Sunday, February 22nd" and time [HH:MM:SS] with live seconds.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <nvs_flash.h>
#include <esp_sntp.h>
#include <esp_timer.h>

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

#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1
#define NTP_SYNC_BIT        BIT2

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static const int WIFI_MAX_RETRY = 10;

static lv_obj_t *s_date_label = NULL;
static lv_obj_t *s_time_label = NULL;

static const char *const WEEKDAY[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
static const char *const MONTH[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

DisplayPort RlcdPort(LCD_PIN_MOSI, LCD_PIN_SCL, LCD_PIN_DC, LCD_PIN_CS, LCD_PIN_RST,
                     LCD_WIDTH, LCD_HEIGHT);

// Flush: write LVGL area into RLCD buffer, push to display once, then signal ready.
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

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, CONFIG_WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, CONFIG_WIFI_PASSWORD, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Time synced");
    xEventGroupSetBits(s_wifi_event_group, NTP_SYNC_BIT);
}

static void init_time(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    // Wait for first sync (up to 15 s)
    for (int i = 0; i < 15; i++) {
        if (xEventGroupGetBits(s_wifi_event_group) & NTP_SYNC_BIT)
            break;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1);  // Central (adjust for your timezone)
    tzset();
}

// Ordinal suffix for day: 1st, 2nd, 3rd, 4th, 22nd, etc.
static void ordinal_suffix(int day, char *buf, size_t cap)
{
    if (day >= 11 && day <= 13) {
        strncpy(buf, "th", cap - 1);
        buf[cap - 1] = '\0';
        return;
    }
    switch (day % 10) {
        case 1: strncpy(buf, "st", cap - 1); break;
        case 2: strncpy(buf, "nd", cap - 1); break;
        case 3: strncpy(buf, "rd", cap - 1); break;
        default: strncpy(buf, "th", cap - 1); break;
    }
    buf[cap - 1] = '\0';
}

static void update_clock_cb(void *arg)
{
    time_t now = time(NULL);
    struct tm tm;
    if (localtime_r(&now, &tm) == NULL)
        return;

    char date_buf[64];
    char suffix[4];
    ordinal_suffix(tm.tm_mday, suffix, sizeof(suffix));
    snprintf(date_buf, sizeof(date_buf), "Hello Kyle, its %s, %s %d%s",
             WEEKDAY[tm.tm_wday], MONTH[tm.tm_mon], tm.tm_mday, suffix);

    char time_buf[16];
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d",
             tm.tm_hour, tm.tm_min, tm.tm_sec);

    if (Lvgl_lock(50)) {
        if (s_date_label)
            lv_label_set_text(s_date_label, date_buf);
        if (s_time_label)
            lv_label_set_text(s_time_label, time_buf);
        Lvgl_unlock();
    }
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare ESP32-S3-RLCD-4.2 clock starting...");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_sta();
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi connected");
        init_time();
    } else {
        ESP_LOGW(TAG, "WiFi not connected; set CONFIG_WIFI_SSID/PASSWORD in menuconfig. Using build time.");
        time_t t = 1730000000;  // fallback: set a fixed time so clock still runs
        struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
        settimeofday(&tv, NULL);
        setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1);
        tzset();
    }

    RlcdPort.RLCD_Init();
    Lvgl_PortInit(LCD_WIDTH, LCD_HEIGHT, lvgl_flush_cb);

    if (Lvgl_lock(-1)) {
        lv_obj_t *screen = lv_screen_active();

        s_date_label = lv_label_create(screen);
        lv_label_set_text(s_date_label, "Hello Kyle, its ...");
        lv_obj_set_style_text_font(s_date_label, &lv_font_montserrat_14, 0);
        lv_obj_align(s_date_label, LV_ALIGN_TOP_MID, 0, 24);

        s_time_label = lv_label_create(screen);
        lv_label_set_text(s_time_label, "00:00:00");
        lv_obj_set_style_text_font(s_time_label, &lv_font_montserrat_24, 0);
        lv_obj_align(s_time_label, LV_ALIGN_CENTER, 0, 24);

        Lvgl_unlock();
    }

    update_clock_cb(NULL);

    esp_timer_handle_t clock_timer = NULL;
    esp_timer_create_args_t timer_args = {};
    timer_args.callback = update_clock_cb;
    timer_args.name = "clock";
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &clock_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(clock_timer, 1000000));  // 1 s in us

    ESP_LOGI(TAG, "Clock display ready");
}
