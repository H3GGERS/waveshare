set(MAIN_SRCS
    "main.cpp"
    "fonts/Inter_24pt_Bold.c"
    "fonts/NotoSansSymbols2_24.c"
)

idf_component_register(SRCS ${MAIN_SRCS}
                       INCLUDE_DIRS "." "fonts"
                       REQUIRES display_bsp lvgl_port lvgl__lvgl nvs_flash esp_netif esp_wifi esp_event lwip esp_http_server esp_http_client json mbedtls esp_adc)
