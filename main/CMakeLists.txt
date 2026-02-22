idf_component_register(SRCS "main.cpp"
                        INCLUDE_DIRS "."
                        REQUIRES display_bsp lvgl_port lvgl__lvgl nvs_flash esp_netif esp_wifi esp_event lwip)
