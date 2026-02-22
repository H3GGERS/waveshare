idf_component_register(
    SRCS "lvgl_bsp.cpp"
    INCLUDE_DIRS "."
    REQUIRES display_bsp freertos esp_timer
    PRIV_REQUIRES lvgl__lvgl)
