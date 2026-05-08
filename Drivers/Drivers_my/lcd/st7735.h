#ifndef ST7735_H
#define ST7735_H

#include "main.h"
#include "spi.h"
#include "font.h"
#include "fonts.h"
#include "stdio.h"

#define ST7735_RST_Pin SPI_RST_Pin
#define ST7735_RST_GPIO_Port SPI_RST_GPIO_Port
#define ST7735_DC_Pin SPI_DC_Pin
#define ST7735_DC_GPIO_Port SPI_DC_GPIO_Port
#define ST7735_CS_Pin SPI_CS_Pin
#define ST7735_CS_GPIO_Port SPI_CS_GPIO_Port
    
#define ST7735_SPI_INSTANCE hspi1

#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

// Screen Direction
#define ST7735_ROTATION 0
// Color Mode: RGB or BGR
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MODE ST7735_MADCTL_RGB
// Color Inverse: 0=NO, 1=YES
#define ST7735_INVERSE 0

// Color definitions
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#define UI_COLOR_BG      ST7735_BLACK
#define UI_COLOR_TOP     ST7735_COLOR565(0, 95, 255)
#define UI_COLOR_LEFT    ST7735_COLOR565(255, 95, 45)
#define UI_COLOR_RIGHT   ST7735_COLOR565(185, 230, 255)
#define UI_COLOR_DARK    ST7735_COLOR565(15, 35, 130)

void ST7735_Init(void);
void ST7735_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ST7735_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const FontDef *font);
void ST7735_DrawStringSize(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const FontDef *font, uint16_t height);
void ST7735_DrawStringMaple(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const font_t *font);
void ST7735_DrawStringMapleSize(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const font_t *font, uint16_t size);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image);
void ST7735_DrawImageLE(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image);
void ST7735_DrawImageLESize(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image, uint16_t size);
void ST7735_DrawImageTransparent(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image, uint16_t transparentColor);
void ST7735_DrawImageTransparentLE(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image, uint16_t transparentColor);
void ST7735_DrawBitmap1Bpp(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *bitmap, uint16_t color, uint16_t bgColor);
void ST7735_DrawBitmap1BppV(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *bitmap, uint16_t color, uint16_t bgColor);

void UI_DrawLayout(void);
#endif

