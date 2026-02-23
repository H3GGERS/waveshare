idf_component_register(SRCS "main.cpp" "fonts/Inter_24pt_Bold.c"
                        INCLUDE_DIRS "." "fonts"
                        REQUIRES display_bsp lvgl_port lvgl__lvgl nvs_flash esp_netif esp_wifi esp_event lwip)
