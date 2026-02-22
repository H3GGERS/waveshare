#pragma once

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>

// Display resolution (landscape mode)
#define LCD_WIDTH   400
#define LCD_HEIGHT  300

// Use LUT-based pixel addressing for best CPU performance
#define AlgorithmOptimization 3

enum ColorSelection {
    ColorBlack = 0,
    ColorWhite = 0xff
};

class DisplayPort {
private:
    esp_lcd_panel_io_handle_t io_handle = NULL;
    const char *TAG = "Display";
    int mosi_, scl_, dc_, cs_, rst_;
    int width_, height_;
    uint8_t  *DispBuffer    = NULL;
    int       DisplayLen;
    uint16_t (*PixelIndexLUT)[300];
    uint8_t  (*PixelBitLUT)[300];

    void InitPortraitLUT();
    void InitLandscapeLUT();
    void Set_ResetIOLevel(uint8_t level);
    void RLCD_SendCommand(uint8_t Reg);
    void RLCD_SendData(uint8_t Data);
    void RLCD_Sendbuffera(uint8_t *Data, int len);
    void RLCD_Reset(void);

public:
    DisplayPort(int mosi, int scl, int dc, int cs, int rst,
                int width, int height,
                spi_host_device_t spihost = SPI3_HOST);
    ~DisplayPort();

    void RLCD_Init();
    void RLCD_ColorClear(uint8_t color);
    void RLCD_Display();
    void RLCD_SetPixel(uint16_t x, uint16_t y, uint8_t color);
};
