/**
 * ESP32-S3 4.2" RLCD - Clock + Chess.com daily board
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cctype>
#include <string>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_sntp.h>
#include <esp_timer.h>
#include <driver/gpio.h>
#include <esp_http_server.h>
#include <esp_http_client.h>
#include <esp_crt_bundle.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>

#include <cJSON.h>

#include "display_bsp.h"
#include "lvgl_bsp.h"
#include "lvgl.h"
#include "chess_sprites/chess_sprites.h"

extern const lv_font_t Inter_12pt_Bold;
extern const lv_font_t Montserrat_20pt_Medium;
extern const lv_font_t Montserrat_20pt_Light;
extern const lv_font_t Montserrat_16pt_Regular;

static const char *TAG = "main";

#define LCD_PIN_MOSI  12
#define LCD_PIN_SCL   11
#define LCD_PIN_DC    5
#define LCD_PIN_CS    40
#define LCD_PIN_RST   41

#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1
#define NTP_SYNC_BIT        BIT2

#ifndef CONFIG_KEY_BUTTON_GPIO
#define CONFIG_KEY_BUTTON_GPIO 18
#endif

static const int WIFI_MAX_RETRY = 10;
static const char *NVS_NAMESPACE = "chess";
static const char *NVS_KEY_SELECTED_URL = "selected_url";

static EventGroupHandle_t s_wifi_event_group;
static SemaphoreHandle_t s_chess_mutex;
static TaskHandle_t s_chess_task = NULL;
static TaskHandle_t s_key_task = NULL;
static httpd_handle_t s_httpd = NULL;
static int s_retry_num = 0;

static lv_obj_t *s_date_label = NULL;
static lv_obj_t *s_time_label = NULL;
static lv_obj_t *s_status_label = NULL;
static lv_obj_t *s_game_value_label = NULL;
static lv_obj_t *s_last_move_value_label = NULL;
static lv_obj_t *s_advantage_value_label = NULL;
static lv_obj_t *s_square_obj[8][8] = {};
static lv_obj_t *s_piece_image[8][8] = {};
static chess_sprite_id_t s_current_sprite[8][8] = {};

static adc_oneshot_unit_handle_t s_adc_handle = NULL;
static adc_channel_t s_battery_channel = ADC_CHANNEL_0;
static adc_cali_handle_t s_adc_cali_handle = NULL;
static bool s_adc_cali_enabled = false;
static int s_battery_last_raw = 0;
static int s_battery_last_pin_mv = 0;
static int s_battery_last_vbat_mv = 0;
static int s_battery_filtered_pct = -1;
static int s_battery_display_pct = -1;
static int s_battery_update_tick = 0;

struct LastMove {
    bool valid = false;
    int from_file = -1;
    int from_rank = -1;
    int to_file = -1;
    int to_rank = -1;
};

struct BoardState {
    char board[8][8] = {};
    LastMove last_move = {};
    bool white_to_move = true;
};

struct ChessGameSummary {
    std::string url;
    std::string white;
    std::string black;
    std::string fen;
    std::string pgn;
};

struct ChessState {
    std::string selected_url;
    std::vector<ChessGameSummary> games;
    BoardState board = {};
    bool board_valid = false;
    bool flip_for_black = false;
    std::string status_text = "Loading...";
    std::string game_text = "";
    std::string last_move_text = "";
    std::string advantage_text = "Unknown";
};

static ChessState s_chess_state;

static const char *const WEEKDAY[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
static const char *const MONTH[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

DisplayPort RlcdPort(LCD_PIN_MOSI, LCD_PIN_SCL, LCD_PIN_DC, LCD_PIN_CS, LCD_PIN_RST,
                     LCD_WIDTH, LCD_HEIGHT);

static bool battery_gpio_to_channel(int gpio, adc_channel_t *out_channel)
{
    switch (gpio) {
        case 1: *out_channel = ADC_CHANNEL_0; return true;
        case 2: *out_channel = ADC_CHANNEL_1; return true;
        case 3: *out_channel = ADC_CHANNEL_2; return true;
        case 4: *out_channel = ADC_CHANNEL_3; return true;
        case 5: *out_channel = ADC_CHANNEL_4; return true;
        case 6: *out_channel = ADC_CHANNEL_5; return true;
        case 7: *out_channel = ADC_CHANNEL_6; return true;
        case 8: *out_channel = ADC_CHANNEL_7; return true;
        case 9: *out_channel = ADC_CHANNEL_8; return true;
        case 10: *out_channel = ADC_CHANNEL_9; return true;
        default: return false;
    }
}

static void battery_adc_init(void)
{
    if (!battery_gpio_to_channel(CONFIG_BATTERY_ADC_GPIO, &s_battery_channel)) {
        ESP_LOGW(TAG, "Invalid battery ADC GPIO=%d; battery percent disabled", CONFIG_BATTERY_ADC_GPIO);
        return;
    }

    adc_oneshot_unit_init_cfg_t unit_cfg = {};
    unit_cfg.unit_id = ADC_UNIT_1;
    unit_cfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    if (adc_oneshot_new_unit(&unit_cfg, &s_adc_handle) != ESP_OK) {
        ESP_LOGW(TAG, "Battery ADC unit init failed");
        s_adc_handle = NULL;
        return;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {};
    chan_cfg.atten = ADC_ATTEN_DB_12;
    chan_cfg.bitwidth = ADC_BITWIDTH_DEFAULT;
    if (adc_oneshot_config_channel(s_adc_handle, s_battery_channel, &chan_cfg) != ESP_OK) {
        ESP_LOGW(TAG, "Battery ADC channel config failed");
        s_adc_handle = NULL;
        return;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_cfg = {};
    cali_cfg.unit_id = ADC_UNIT_1;
    cali_cfg.chan = s_battery_channel;
    cali_cfg.atten = ADC_ATTEN_DB_12;
    cali_cfg.bitwidth = ADC_BITWIDTH_DEFAULT;
    if (adc_cali_create_scheme_curve_fitting(&cali_cfg, &s_adc_cali_handle) == ESP_OK) {
        s_adc_cali_enabled = true;
    }
#endif
}

static int read_battery_percent(void)
{
    if (!s_adc_handle) {
        return -1;
    }

    // Make displayed battery "stickier": refresh underlying ADC every 10 seconds.
    s_battery_update_tick++;
    if ((s_battery_update_tick % 10) != 0 && s_battery_display_pct >= 0) {
        return s_battery_display_pct;
    }

    // Reduce ADC noise by averaging several back-to-back samples.
    const int sample_count = 8;
    int raw_sum = 0;
    for (int i = 0; i < sample_count; ++i) {
        int raw_sample = 0;
        if (adc_oneshot_read(s_adc_handle, s_battery_channel, &raw_sample) != ESP_OK) {
            return -1;
        }
        raw_sum += raw_sample;
    }
    int raw = raw_sum / sample_count;
    s_battery_last_raw = raw;

    int mv_at_pin = 0;
    if (s_adc_cali_enabled) {
        if (adc_cali_raw_to_voltage(s_adc_cali_handle, raw, &mv_at_pin) != ESP_OK) {
            return -1;
        }
    } else {
        mv_at_pin = (raw * 3300) / 4095;
    }
    s_battery_last_pin_mv = mv_at_pin;

    int64_t rtop = CONFIG_BATTERY_DIVIDER_RTOP_OHMS;
    int64_t rbot = CONFIG_BATTERY_DIVIDER_RBOT_OHMS;
    int64_t vbat_mv = (int64_t)mv_at_pin * (rtop + rbot) / rbot;
    s_battery_last_vbat_mv = (int)vbat_mv;

    int empty_mv = CONFIG_BATTERY_EMPTY_MV;
    int full_mv = CONFIG_BATTERY_FULL_MV;
    if (full_mv <= empty_mv) {
        return -1;
    }

    int pct = (int)((vbat_mv - empty_mv) * 100 / (full_mv - empty_mv));
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;

    // Low-pass filter to stabilize readings.
    if (s_battery_filtered_pct < 0) {
        s_battery_filtered_pct = pct;
    } else {
        s_battery_filtered_pct = (s_battery_filtered_pct * 7 + pct) / 8;
    }

    // Hysteresis so displayed percentage doesn't flicker every second.
    if (s_battery_display_pct < 0) {
        s_battery_display_pct = s_battery_filtered_pct;
    } else {
        int diff = s_battery_filtered_pct - s_battery_display_pct;
        if (diff >= 2) {
            s_battery_display_pct++;
        } else if (diff <= -2) {
            s_battery_display_pct--;
        }
    }

    return s_battery_display_pct;
}

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
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

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
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    for (int i = 0; i < 15; i++) {
        if (xEventGroupGetBits(s_wifi_event_group) & NTP_SYNC_BIT) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    setenv("TZ", "MST7MDT,M3.2.0,M11.1.0", 1);
    tzset();
}

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
    static int s_battery_log_tick = 0;
    time_t now = time(NULL);
    struct tm tm;
    if (localtime_r(&now, &tm) == NULL) {
        return;
    }

    char date_buf[64];
    char suffix[4];
    ordinal_suffix(tm.tm_mday, suffix, sizeof(suffix));
    snprintf(date_buf, sizeof(date_buf), "Its %s, %s %d%s",
             WEEKDAY[tm.tm_wday], MONTH[tm.tm_mon], tm.tm_mday, suffix);

    int h12 = tm.tm_hour % 12;
    if (h12 == 0) {
        h12 = 12;
    }
    const char *ampm = (tm.tm_hour < 12) ? "AM" : "PM";
    int batt_pct = read_battery_percent();
    char time_buf[32];
    snprintf(time_buf, sizeof(time_buf), "%d:%02d %s", h12, tm.tm_min, ampm);

    if (Lvgl_lock(50)) {
        if (s_date_label) {
            lv_label_set_text(s_date_label, date_buf);
        }
        if (s_time_label) {
            lv_label_set_text(s_time_label, time_buf);
        }
        Lvgl_unlock();
    }

    if (++s_battery_log_tick >= 30) {
        s_battery_log_tick = 0;
        ESP_LOGI(TAG, "Battery ADC gpio=%d raw=%d pin=%dmV vbat=%dmV pct=%d",
                 CONFIG_BATTERY_ADC_GPIO,
                 s_battery_last_raw,
                 s_battery_last_pin_mv,
                 s_battery_last_vbat_mv,
                 batt_pct);
        if (s_battery_last_raw < 20) {
            ESP_LOGW(TAG, "Battery ADC raw is near zero; check ADC GPIO/divider wiring and menuconfig");
        }
    }
}

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    std::string *body = static_cast<std::string *>(evt->user_data);
    if (evt->event_id == HTTP_EVENT_ON_DATA && evt->data && evt->data_len > 0) {
        body->append((const char *)evt->data, evt->data_len);
    }
    return ESP_OK;
}

static bool http_get_string(const std::string &url, std::string &out_body)
{
    out_body.clear();
    esp_http_client_config_t cfg = {};
    cfg.url = url.c_str();
    cfg.method = HTTP_METHOD_GET;
    cfg.timeout_ms = 12000;
    cfg.event_handler = http_event_handler;
    cfg.user_data = &out_body;
    cfg.crt_bundle_attach = esp_crt_bundle_attach;
    cfg.user_agent = "waveshare-esp32s3/1.0";

    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) {
        ESP_LOGE(TAG, "http_get_string: client init failed for %s", url.c_str());
        return false;
    }
    esp_http_client_set_header(client, "Accept", "application/json");
    esp_http_client_set_header(client, "Accept-Encoding", "identity");

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    int64_t content_length = esp_http_client_get_content_length(client);
    if (err != ESP_OK || status != 200) {
        ESP_LOGE(TAG, "HTTP GET failed: url=%s err=%s status=%d len=%lld body_bytes=%u",
                 url.c_str(), esp_err_to_name(err), status, (long long)content_length,
                 (unsigned int)out_body.size());
        if (!out_body.empty()) {
            size_t preview = out_body.size() > 180 ? 180 : out_body.size();
            ESP_LOGW(TAG, "HTTP body preview: %.*s", (int)preview, out_body.c_str());
        }
    } else {
        ESP_LOGI(TAG, "HTTP GET ok: url=%s len=%u", url.c_str(), (unsigned int)out_body.size());
    }
    esp_http_client_cleanup(client);
    return (err == ESP_OK && status == 200);
}

static bool parse_fen_to_board(const std::string &fen, BoardState &out)
{
    memset(out.board, 0, sizeof(out.board));
    out.last_move.valid = false;

    size_t space = fen.find(' ');
    std::string board_part = (space == std::string::npos) ? fen : fen.substr(0, space);
    std::string turn_part;
    if (space != std::string::npos && space + 2 < fen.size()) {
        turn_part = fen.substr(space + 1, 1);
    }
    out.white_to_move = (turn_part != "b");

    int rank = 0;
    int file = 0;
    for (char ch : board_part) {
        if (ch == '/') {
            rank++;
            file = 0;
            continue;
        }
        if (ch >= '1' && ch <= '8') {
            file += (ch - '0');
            continue;
        }
        if (rank < 0 || rank > 7 || file < 0 || file > 7) {
            return false;
        }
        out.board[rank][file] = ch;
        file++;
    }
    return true;
}

static bool extract_last_move_from_pgn(const std::string &pgn, LastMove &out)
{
    out.valid = false;
    for (size_t i = 0; i + 3 < pgn.size(); ++i) {
        if (pgn[i] >= 'a' && pgn[i] <= 'h' &&
            pgn[i + 1] >= '1' && pgn[i + 1] <= '8' &&
            pgn[i + 2] >= 'a' && pgn[i + 2] <= 'h' &&
            pgn[i + 3] >= '1' && pgn[i + 3] <= '8') {
            out.from_file = pgn[i] - 'a';
            out.from_rank = 7 - (pgn[i + 1] - '1');
            out.to_file = pgn[i + 2] - 'a';
            out.to_rank = 7 - (pgn[i + 3] - '1');
            out.valid = true;
        }
    }
    return out.valid;
}

static std::string extract_last_san_from_pgn(const std::string &pgn)
{
    size_t body_pos = pgn.find("\r\n\r\n");
    size_t skip = 4;
    if (body_pos == std::string::npos) {
        body_pos = pgn.find("\n\n");
        skip = 2;
    }
    std::string moves = (body_pos == std::string::npos) ? pgn : pgn.substr(body_pos + skip);
    size_t first_move = moves.find("1.");
    if (first_move != std::string::npos) {
        moves = moves.substr(first_move);
    }
    std::string token;
    std::string last_san;
    int current_move_no = 0;
    bool current_black = false;
    int last_move_no = 0;
    bool last_black = false;

    auto is_san_token = [](const std::string &t) -> bool {
        if (t.empty()) {
            return false;
        }
        if (t == "1-0" || t == "0-1" || t == "1/2-1/2" || t == "*") {
            return false;
        }
        if (t[0] == '{' || t[0] == '[' || t[0] == '(' || t[0] == '$') {
            return false;
        }
        if (t.find(':') != std::string::npos) {
            return false; // avoid clock timestamps like 0:01:23
        }
        if (t.find("...") != std::string::npos) {
            return false;
        }
        size_t dot_pos = t.find('.');
        if (dot_pos != std::string::npos) {
            bool numeric = true;
            for (size_t i = 0; i < dot_pos; ++i) {
                if (!std::isdigit((unsigned char)t[i])) {
                    numeric = false;
                    break;
                }
            }
            if (numeric) {
                return false;
            }
        }
        return true;
    };

    auto parse_move_number_token = [](const std::string &t, int &move_no, bool &black_move) -> bool {
        size_t dot = t.find('.');
        if (dot == std::string::npos) {
            return false;
        }
        for (size_t i = 0; i < dot; ++i) {
            if (!std::isdigit((unsigned char)t[i])) {
                return false;
            }
        }
        move_no = atoi(t.substr(0, dot).c_str());
        black_move = (t.find("...") != std::string::npos);
        return true;
    };

    for (char ch : moves) {
        if (ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t') {
            if (token.empty()) {
                continue;
            }
            int parsed_no = 0;
            bool parsed_black = false;
            if (parse_move_number_token(token, parsed_no, parsed_black)) {
                current_move_no = parsed_no;
                current_black = parsed_black;
                token.clear();
                continue;
            }
            if (is_san_token(token)) {
                last_san = token;
                last_move_no = current_move_no;
                last_black = current_black;
                current_black = !current_black;
            }
            token.clear();
            continue;
        }
        token.push_back(ch);
    }

    if (is_san_token(token)) {
        last_san = token;
        last_move_no = current_move_no;
        last_black = current_black;
    }
    if (last_san.empty()) {
        return last_san;
    }
    if (last_move_no > 0) {
        return std::to_string(last_move_no) + (last_black ? "... " : ". ") + last_san;
    }
    return last_san;
}

static std::string safe_json_string(cJSON *obj, const char *key)
{
    cJSON *v = cJSON_GetObjectItemCaseSensitive(obj, key);
    return cJSON_IsString(v) && v->valuestring ? std::string(v->valuestring) : std::string();
}

static std::string extract_username_from_url(const std::string &url)
{
    size_t pos = url.find_last_of('/');
    if (pos == std::string::npos || pos + 1 >= url.size()) {
        return url;
    }
    return url.substr(pos + 1);
}

static cJSON *parse_games_root_from_body(const std::string &body)
{
    cJSON *root = cJSON_Parse(body.c_str());
    if (root) {
        cJSON *arr = cJSON_GetObjectItemCaseSensitive(root, "games");
        if (cJSON_IsArray(arr)) {
            return root;
        }
        cJSON_Delete(root);
    }

    // Some Chess.com responses can include a prefixed advisory JSON object.
    size_t games_pos = body.find("{\"games\":");
    if (games_pos == std::string::npos) {
        return NULL;
    }
    std::string tail = body.substr(games_pos);
    return cJSON_Parse(tail.c_str());
}

static bool fetch_daily_games(std::vector<ChessGameSummary> &games)
{
    games.clear();
    std::string body;
    std::string username = CONFIG_CHESS_USERNAME;
    for (char &ch : username) {
        ch = (char)std::tolower((unsigned char)ch);
    }
    std::string url = "https://api.chess.com/pub/player/" + username + "/games";
    if (!http_get_string(url, body)) {
        ESP_LOGE(TAG, "Failed to fetch games from Chess.com endpoint");
        return false;
    }

    cJSON *root = parse_games_root_from_body(body);
    if (!root) {
        const char *json_err = cJSON_GetErrorPtr();
        ESP_LOGE(TAG, "Chess.com JSON parse failed near: %s", json_err ? json_err : "(unknown)");
        size_t preview = body.size() > 200 ? 200 : body.size();
        ESP_LOGW(TAG, "JSON preview: %.*s", (int)preview, body.c_str());
        return false;
    }

    cJSON *arr = cJSON_GetObjectItemCaseSensitive(root, "games");
    if (!cJSON_IsArray(arr)) {
        std::string code = safe_json_string(root, "code");
        std::string message = safe_json_string(root, "message");
        ESP_LOGE(TAG, "Chess.com payload missing 'games' array (code=%s message=%s)",
                 code.c_str(), message.c_str());
        size_t preview = body.size() > 220 ? 220 : body.size();
        ESP_LOGW(TAG, "Payload preview: %.*s", (int)preview, body.c_str());
        cJSON_Delete(root);
        return false;
    }

    cJSON *game = NULL;
    cJSON_ArrayForEach(game, arr) {
        std::string time_class = safe_json_string(game, "time_class");
        std::string rules = safe_json_string(game, "rules");
        if (time_class != "daily" || rules != "chess") {
            continue;
        }

        ChessGameSummary g;
        g.url = safe_json_string(game, "url");
        g.fen = safe_json_string(game, "fen");
        g.pgn = safe_json_string(game, "pgn");

        cJSON *white = cJSON_GetObjectItemCaseSensitive(game, "white");
        cJSON *black = cJSON_GetObjectItemCaseSensitive(game, "black");
        if (cJSON_IsObject(white)) {
            g.white = safe_json_string(white, "username");
        } else if (cJSON_IsString(white) && white->valuestring) {
            g.white = extract_username_from_url(white->valuestring);
        }
        if (cJSON_IsObject(black)) {
            g.black = safe_json_string(black, "username");
        } else if (cJSON_IsString(black) && black->valuestring) {
            g.black = extract_username_from_url(black->valuestring);
        }
        if (g.white.empty()) {
            g.white = "White";
        }
        if (g.black.empty()) {
            g.black = "Black";
        }
        if (!g.url.empty() && !g.fen.empty()) {
            games.push_back(g);
        }
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "Parsed %u daily games", (unsigned int)games.size());
    return true;
}

static bool load_selected_url_from_nvs(std::string &url)
{
    url.clear();
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }
    size_t required = 0;
    if (nvs_get_str(handle, NVS_KEY_SELECTED_URL, NULL, &required) != ESP_OK || required == 0) {
        nvs_close(handle);
        return false;
    }
    std::vector<char> buf(required);
    if (nvs_get_str(handle, NVS_KEY_SELECTED_URL, buf.data(), &required) == ESP_OK) {
        url = std::string(buf.data());
        nvs_close(handle);
        return true;
    }
    nvs_close(handle);
    return false;
}

static void save_selected_url_to_nvs(const std::string &url)
{
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) {
        return;
    }
    nvs_set_str(handle, NVS_KEY_SELECTED_URL, url.c_str());
    nvs_commit(handle);
    nvs_close(handle);
}

static int piece_material_value(char piece)
{
    switch ((char)std::tolower((unsigned char)piece)) {
        case 'p': return 1;
        case 'n': return 3;
        case 'b': return 3;
        case 'r': return 5;
        case 'q': return 9;
        default: return 0;
    }
}

static std::string compute_advantage_text(const BoardState &board)
{
    int white_score = 0;
    int black_score = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = board.board[r][c];
            if (piece == 0) {
                continue;
            }
            int value = piece_material_value(piece);
            if (std::isupper((unsigned char)piece)) {
                white_score += value;
            } else {
                black_score += value;
            }
        }
    }

    int diff = white_score - black_score;
    if (diff == 0) {
        return "Equal";
    }
    if (diff > 0) {
        return "White +" + std::to_string(diff);
    }
    return "Black +" + std::to_string(-diff);
}

static chess_sprite_id_t sprite_for_piece(char piece, bool light_square)
{
    if (piece == 0) {
        return light_square ? CHESS_SPRITE_EMPTY_LIGHT : CHESS_SPRITE_EMPTY_DARK;
    }
    const bool is_white = std::isupper((unsigned char)piece);
    char p = (char)std::tolower((unsigned char)piece);
    switch (p) {
        case 'k':
            if (is_white) return light_square ? CHESS_SPRITE_WK_LIGHT : CHESS_SPRITE_WK_DARK;
            return light_square ? CHESS_SPRITE_BK_LIGHT : CHESS_SPRITE_BK_DARK;
        case 'q':
            if (is_white) return light_square ? CHESS_SPRITE_WQ_LIGHT : CHESS_SPRITE_WQ_DARK;
            return light_square ? CHESS_SPRITE_BQ_LIGHT : CHESS_SPRITE_BQ_DARK;
        case 'b':
            if (is_white) return light_square ? CHESS_SPRITE_WB_LIGHT : CHESS_SPRITE_WB_DARK;
            return light_square ? CHESS_SPRITE_BB_LIGHT : CHESS_SPRITE_BB_DARK;
        case 'n':
            if (is_white) return light_square ? CHESS_SPRITE_WN_LIGHT : CHESS_SPRITE_WN_DARK;
            return light_square ? CHESS_SPRITE_BN_LIGHT : CHESS_SPRITE_BN_DARK;
        case 'r':
            if (is_white) return light_square ? CHESS_SPRITE_WR_LIGHT : CHESS_SPRITE_WR_DARK;
            return light_square ? CHESS_SPRITE_BR_LIGHT : CHESS_SPRITE_BR_DARK;
        case 'p':
            if (is_white) return light_square ? CHESS_SPRITE_WP_LIGHT : CHESS_SPRITE_WP_DARK;
            return light_square ? CHESS_SPRITE_BP_LIGHT : CHESS_SPRITE_BP_DARK;
        default:
            return light_square ? CHESS_SPRITE_EMPTY_LIGHT : CHESS_SPRITE_EMPTY_DARK;
    }
}

static void render_board_ui(const BoardState &board,
                            bool valid,
                            bool flip_for_black,
                            const char *status,
                            const char *game_text,
                            const char *last_move_text,
                            const char *advantage_text)
{
    if (!Lvgl_lock(200)) {
        return;
    }

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            const int br = flip_for_black ? (7 - r) : r;
            const int bc = flip_for_black ? (7 - c) : c;
            bool light = ((br + bc) % 2 == 0);
            chess_sprite_id_t sprite = light ? CHESS_SPRITE_EMPTY_LIGHT : CHESS_SPRITE_EMPTY_DARK;
            if (valid) {
                sprite = sprite_for_piece(board.board[br][bc], light);
            }

            if (!s_piece_image[r][c]) {
                continue;
            }
            if (s_current_sprite[r][c] != sprite) {
                s_current_sprite[r][c] = sprite;
                lv_image_set_src(s_piece_image[r][c], chess_sprite_get(sprite));
            }

            bool is_last_move_square = valid && board.last_move.valid &&
                ((board.last_move.from_rank == br && board.last_move.from_file == bc) ||
                 (board.last_move.to_rank == br && board.last_move.to_file == bc));
            lv_obj_set_style_border_width(s_square_obj[r][c], is_last_move_square ? 2 : 0, 0);
            if (is_last_move_square) {
                lv_obj_set_style_border_color(s_square_obj[r][c], light ? lv_color_black() : lv_color_white(), 0);
            }
        }
    }

    if (s_status_label) {
        lv_label_set_text(s_status_label, status ? status : "");
    }
    if (s_game_value_label) {
        lv_label_set_text(s_game_value_label, game_text ? game_text : "");
    }
    if (s_last_move_value_label) {
        lv_label_set_text(s_last_move_value_label, last_move_text ? last_move_text : "");
    }
    if (s_advantage_value_label) {
        lv_label_set_text(s_advantage_value_label, advantage_text ? advantage_text : "");
    }
    Lvgl_unlock();
}

static bool refresh_chess_state(void)
{
    std::vector<ChessGameSummary> games;
    std::string username = CONFIG_CHESS_USERNAME;
    std::string user_lower = username;
    for (char &ch : user_lower) {
        ch = (char)std::tolower((unsigned char)ch);
    }
    if (!fetch_daily_games(games)) {
        xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
        s_chess_state.status_text = "Chess.com offline";
        s_chess_state.game_text = "VS --";
        s_chess_state.last_move_text = "--";
        s_chess_state.advantage_text = "Unknown";
        xSemaphoreGive(s_chess_mutex);
        return false;
    }

    xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
    s_chess_state.games = games;
    if (s_chess_state.selected_url.empty() && !games.empty()) {
        s_chess_state.selected_url = games[0].url;
    }
    std::string selected = s_chess_state.selected_url;
    xSemaphoreGive(s_chess_mutex);

    if (games.empty()) {
        xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
        s_chess_state.board_valid = false;
        s_chess_state.status_text = "No daily games";
        s_chess_state.game_text = "VS --";
        s_chess_state.last_move_text = "--";
        s_chess_state.advantage_text = "Unknown";
        xSemaphoreGive(s_chess_mutex);
        return false;
    }

    const ChessGameSummary *selected_game = nullptr;
    for (const auto &g : games) {
        if (g.url == selected) {
            selected_game = &g;
            break;
        }
    }
    if (!selected_game) {
        selected_game = &games[0];
        selected = selected_game->url;
    }

    BoardState board = {};
    if (!parse_fen_to_board(selected_game->fen, board)) {
        xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
        s_chess_state.board_valid = false;
        s_chess_state.status_text = "FEN parse error";
        s_chess_state.game_text = "VS --";
        s_chess_state.last_move_text = "--";
        s_chess_state.advantage_text = "Unknown";
        xSemaphoreGive(s_chess_mutex);
        return false;
    }
    extract_last_move_from_pgn(selected_game->pgn, board.last_move);
    std::string last_san = extract_last_san_from_pgn(selected_game->pgn);
    if (last_san.empty()) {
        last_san = "--";
    }

    std::string white_lower = selected_game->white;
    std::string black_lower = selected_game->black;
    for (char &ch : white_lower) ch = (char)std::tolower((unsigned char)ch);
    for (char &ch : black_lower) ch = (char)std::tolower((unsigned char)ch);

    std::string opponent = selected_game->black;
    bool user_is_black = false;
    if (user_lower == black_lower) {
        opponent = selected_game->white;
        user_is_black = true;
    }

    xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
    s_chess_state.selected_url = selected;
    s_chess_state.board = board;
    s_chess_state.board_valid = true;
    s_chess_state.flip_for_black = user_is_black;
    s_chess_state.status_text = selected_game->white + " vs " + selected_game->black;
    s_chess_state.game_text = "VS " + opponent;
    s_chess_state.last_move_text = last_san;
    s_chess_state.advantage_text = compute_advantage_text(board);
    xSemaphoreGive(s_chess_mutex);
    save_selected_url_to_nvs(selected);
    return true;
}

static std::string http_escape(const std::string &s)
{
    std::string out;
    out.reserve(s.size() + 16);
    for (char ch : s) {
        if (ch == '&') out += "&amp;";
        else if (ch == '<') out += "&lt;";
        else if (ch == '>') out += "&gt;";
        else if (ch == '"') out += "&quot;";
        else out.push_back(ch);
    }
    return out;
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    std::vector<ChessGameSummary> games;
    std::string selected;
    xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
    games = s_chess_state.games;
    selected = s_chess_state.selected_url;
    xSemaphoreGive(s_chess_mutex);

    std::string html;
    html += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>Chess Board Selector</title></head><body>";
    html += "<h2>Chess.com daily games</h2><p>User: " + http_escape(CONFIG_CHESS_USERNAME) + "</p>";
    html += "<form method='POST' action='/select'><select name='idx'>";
    for (size_t i = 0; i < games.size(); ++i) {
        bool is_selected = (games[i].url == selected);
        html += "<option value='" + std::to_string(i) + "'";
        if (is_selected) {
            html += " selected";
        }
        html += ">" + http_escape(games[i].white + " vs " + games[i].black) + "</option>";
    }
    html += "</select><button type='submit'>Select</button></form>";
    html += "<p><a href='/refresh'>Refresh now</a></p></body></html>";

    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html.c_str(), html.size());
}

static esp_err_t refresh_get_handler(httpd_req_t *req)
{
    if (s_chess_task) {
        xTaskNotifyGive(s_chess_task);
    }
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "Refresh requested");
}

static esp_err_t select_post_handler(httpd_req_t *req)
{
    char content[64] = {};
    int len = httpd_req_recv(req, content, sizeof(content) - 1);
    if (len <= 0) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No body");
    }
    content[len] = '\0';

    int idx = -1;
    if (sscanf(content, "idx=%d", &idx) != 1) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing idx");
    }

    std::string selected;
    bool ok = false;
    xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
    if (idx >= 0 && (size_t)idx < s_chess_state.games.size()) {
        selected = s_chess_state.games[idx].url;
        s_chess_state.selected_url = selected;
        ok = true;
    }
    xSemaphoreGive(s_chess_mutex);

    if (!ok) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid idx");
    }

    save_selected_url_to_nvs(selected);
    if (s_chess_task) {
        xTaskNotifyGive(s_chess_task);
    }
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    return httpd_resp_send(req, NULL, 0);
}

static void start_http_server(void)
{
    if (s_httpd) {
        return;
    }
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&s_httpd, &config) != ESP_OK) {
        return;
    }
    httpd_uri_t root_uri = {};
    root_uri.uri = "/";
    root_uri.method = HTTP_GET;
    root_uri.handler = root_get_handler;
    httpd_register_uri_handler(s_httpd, &root_uri);

    httpd_uri_t refresh_uri = {};
    refresh_uri.uri = "/refresh";
    refresh_uri.method = HTTP_GET;
    refresh_uri.handler = refresh_get_handler;
    httpd_register_uri_handler(s_httpd, &refresh_uri);

    httpd_uri_t select_uri = {};
    select_uri.uri = "/select";
    select_uri.method = HTTP_POST;
    select_uri.handler = select_post_handler;
    httpd_register_uri_handler(s_httpd, &select_uri);
}

static void cycle_to_next_game(void)
{
    std::string selected;
    bool ok = false;
    xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
    size_t n = s_chess_state.games.size();
    if (n > 0) {
        size_t idx = 0;
        bool found = false;
        for (size_t i = 0; i < n; ++i) {
            if (s_chess_state.games[i].url == s_chess_state.selected_url) {
                idx = i;
                found = true;
                break;
            }
        }
        size_t next = found ? ((idx + 1) % n) : 0;
        selected = s_chess_state.games[next].url;
        s_chess_state.selected_url = selected;
        ok = true;
    }
    xSemaphoreGive(s_chess_mutex);

    if (!ok) {
        return;
    }

    save_selected_url_to_nvs(selected);
    if (s_chess_task) {
        xTaskNotifyGive(s_chess_task);
    }
}

static void key_button_task(void *arg)
{
    const gpio_num_t key_gpio = (gpio_num_t)CONFIG_KEY_BUTTON_GPIO;
    int prev_level = gpio_get_level(key_gpio);
    TickType_t last_press = 0;
    const TickType_t debounce = pdMS_TO_TICKS(220);

    while (true) {
        int level = gpio_get_level(key_gpio);
        TickType_t now = xTaskGetTickCount();
        if (prev_level == 1 && level == 0 && (now - last_press) > debounce) {
            last_press = now;
            cycle_to_next_game();
        }
        prev_level = level;
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

static void chess_refresh_task(void *arg)
{
    const TickType_t interval = pdMS_TO_TICKS(CONFIG_CHESS_REFRESH_SECONDS * 1000);
    while (true) {
        refresh_chess_state();
        xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
        BoardState board = s_chess_state.board;
        bool valid = s_chess_state.board_valid;
        bool flip_for_black = s_chess_state.flip_for_black;
        std::string status = s_chess_state.status_text;
        std::string game = s_chess_state.game_text;
        std::string last_move = s_chess_state.last_move_text;
        std::string advantage = s_chess_state.advantage_text;
        xSemaphoreGive(s_chess_mutex);
        render_board_ui(board, valid, flip_for_black, status.c_str(), game.c_str(), last_move.c_str(), advantage.c_str());
        ulTaskNotifyTake(pdTRUE, interval);
    }
}

static void create_chess_ui(lv_obj_t *content)
{
    const int SQUARE_SIZE = CHESS_SPRITE_TILE_SIZE;
    const int BOARD_SIZE = SQUARE_SIZE * 8;
    const int BODY_GAP = 16;
    const int DETAILS_W = LCD_WIDTH - (12 * 2) - BOARD_SIZE - BODY_GAP;

    lv_obj_t *body = lv_obj_create(content);
    lv_obj_set_size(body, lv_pct(100), BOARD_SIZE + 4);
    lv_obj_set_style_bg_opa(body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_set_style_pad_all(body, 0, 0);
    lv_obj_set_style_pad_column(body, BODY_GAP, 0);
    lv_obj_set_style_radius(body, 0, 0);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(body, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *board_container = lv_obj_create(body);
    lv_obj_set_size(board_container, BOARD_SIZE + 4, BOARD_SIZE + 4);
    lv_obj_set_style_bg_opa(board_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(board_container, 2, 0);
    lv_obj_set_style_border_color(board_container, lv_color_black(), 0);
    lv_obj_set_style_pad_all(board_container, 0, 0);
    lv_obj_set_style_radius(board_container, 0, 0);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            s_square_obj[r][c] = lv_obj_create(board_container);
            lv_obj_set_size(s_square_obj[r][c], SQUARE_SIZE, SQUARE_SIZE);
            lv_obj_set_pos(s_square_obj[r][c], 2 + (c * SQUARE_SIZE), 2 + (r * SQUARE_SIZE));
            lv_obj_set_style_border_width(s_square_obj[r][c], 0, 0);
            lv_obj_set_style_radius(s_square_obj[r][c], 0, 0);
            lv_obj_set_style_pad_all(s_square_obj[r][c], 0, 0);
            lv_obj_set_style_bg_color(s_square_obj[r][c], lv_color_white(), 0);

            s_piece_image[r][c] = lv_image_create(s_square_obj[r][c]);
            lv_obj_set_size(s_piece_image[r][c], SQUARE_SIZE, SQUARE_SIZE);
            lv_obj_center(s_piece_image[r][c]);
            s_current_sprite[r][c] = CHESS_SPRITE_COUNT;
            chess_sprite_id_t initial = ((r + c) % 2 == 0) ? CHESS_SPRITE_EMPTY_LIGHT : CHESS_SPRITE_EMPTY_DARK;
            lv_image_set_src(s_piece_image[r][c], chess_sprite_get(initial));
            s_current_sprite[r][c] = initial;
        }
    }

    lv_obj_t *details = lv_obj_create(body);
    lv_obj_set_size(details, DETAILS_W, BOARD_SIZE);
    lv_obj_set_style_bg_opa(details, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(details, 0, 0);
    lv_obj_set_style_pad_top(details, 8, 0);
    lv_obj_set_style_pad_bottom(details, 8, 0);
    lv_obj_set_style_pad_left(details, 0, 0);
    lv_obj_set_style_pad_right(details, 0, 0);
    lv_obj_set_style_pad_row(details, 16, 0);
    lv_obj_set_style_radius(details, 0, 0);
    lv_obj_set_flex_flow(details, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(details, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t *title_game = lv_label_create(details);
    lv_obj_set_style_text_font(title_game, &Inter_12pt_Bold, 0);
    lv_obj_set_style_text_color(title_game, lv_color_black(), 0);
    lv_label_set_text(title_game, "Game");

    s_game_value_label = lv_label_create(details);
    lv_obj_set_style_text_font(s_game_value_label, &Montserrat_16pt_Regular, 0);
    lv_obj_set_style_text_color(s_game_value_label, lv_color_black(), 0);
    lv_label_set_text(s_game_value_label, "VS --");

    lv_obj_t *title_move = lv_label_create(details);
    lv_obj_set_style_text_font(title_move, &Inter_12pt_Bold, 0);
    lv_obj_set_style_text_color(title_move, lv_color_black(), 0);
    lv_label_set_text(title_move, "Last Move");

    s_last_move_value_label = lv_label_create(details);
    lv_obj_set_style_text_font(s_last_move_value_label, &Montserrat_16pt_Regular, 0);
    lv_obj_set_style_text_color(s_last_move_value_label, lv_color_black(), 0);
    lv_label_set_text(s_last_move_value_label, "--");

    lv_obj_t *title_adv = lv_label_create(details);
    lv_obj_set_style_text_font(title_adv, &Inter_12pt_Bold, 0);
    lv_obj_set_style_text_color(title_adv, lv_color_black(), 0);
    lv_label_set_text(title_adv, "Advantage");

    s_advantage_value_label = lv_label_create(details);
    lv_obj_set_style_text_font(s_advantage_value_label, &Montserrat_16pt_Regular, 0);
    lv_obj_set_style_text_color(s_advantage_value_label, lv_color_black(), 0);
    lv_label_set_text(s_advantage_value_label, "Unknown");

    // Status retained for diagnostics, kept hidden from layout.
    s_status_label = lv_label_create(content);
    lv_obj_set_style_text_font(s_status_label, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(s_status_label, LCD_WIDTH + 8, LCD_HEIGHT + 8);
    lv_label_set_text(s_status_label, "");
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare ESP32-S3-RLCD-4.2 clock + chess starting...");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    s_chess_mutex = xSemaphoreCreateMutex();
    std::string saved_url;
    if (load_selected_url_from_nvs(saved_url)) {
        xSemaphoreTake(s_chess_mutex, portMAX_DELAY);
        s_chess_state.selected_url = saved_url;
        xSemaphoreGive(s_chess_mutex);
    }

    gpio_config_t key_cfg = {};
    key_cfg.pin_bit_mask = (1ULL << CONFIG_KEY_BUTTON_GPIO);
    key_cfg.mode = GPIO_MODE_INPUT;
    key_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    key_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    key_cfg.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&key_cfg);

    wifi_init_sta();
    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        init_time();
        start_http_server();
    } else {
        ESP_LOGW(TAG, "WiFi not connected; using fallback time");
        time_t t = 1730000000;
        struct timeval tv = {.tv_sec = t, .tv_usec = 0};
        settimeofday(&tv, NULL);
        setenv("TZ", "MST7MDT,M3.2.0,M11.1.0", 1);
        tzset();
    }

    RlcdPort.RLCD_Init();
    battery_adc_init();
    Lvgl_PortInit(LCD_WIDTH, LCD_HEIGHT, lvgl_flush_cb);

    if (Lvgl_lock(-1)) {
        lv_obj_t *screen = lv_screen_active();
        lv_obj_t *content = lv_obj_create(screen);
        lv_obj_set_size(content, LCD_WIDTH, LCD_HEIGHT);
        lv_obj_center(content);
        lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(content, 0, 0);
        lv_obj_set_style_pad_all(content, 12, 0);
        lv_obj_set_style_pad_row(content, 8, 0);
        lv_obj_set_style_radius(content, 0, 0);
        lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

        lv_obj_t *top = lv_obj_create(content);
        lv_obj_set_width(top, lv_pct(100));
        lv_obj_set_height(top, 30);
        lv_obj_set_style_bg_opa(top, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(top, 0, 0);
        lv_obj_set_style_pad_all(top, 0, 0);
        lv_obj_set_style_radius(top, 0, 0);
        lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        s_date_label = lv_label_create(top);
        lv_label_set_text(s_date_label, "Its ...");
        lv_obj_set_style_text_font(s_date_label, &Montserrat_20pt_Medium, 0);

        s_time_label = lv_label_create(top);
        lv_label_set_text(s_time_label, "10:53 AM");
        lv_obj_set_style_text_font(s_time_label, &Montserrat_20pt_Light, 0);

        create_chess_ui(content);
        Lvgl_unlock();
    }

    update_clock_cb(NULL);

    esp_timer_handle_t clock_timer = NULL;
    esp_timer_create_args_t timer_args = {};
    timer_args.callback = update_clock_cb;
    timer_args.name = "clock";
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &clock_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(clock_timer, 1000000));

    xTaskCreate(chess_refresh_task, "chess_refresh", 12288, NULL, 4, &s_chess_task);
    xTaskCreate(key_button_task, "key_button", 4096, NULL, 3, &s_key_task);
}
