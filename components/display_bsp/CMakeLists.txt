idf_component_register(
    SRCS "display_bsp.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio esp_lcd
    PRIV_REQUIRES driver esp_timer)
