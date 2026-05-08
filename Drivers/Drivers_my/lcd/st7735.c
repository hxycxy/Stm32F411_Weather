#include "st7735.h"
#include <string.h>

#define ST7735_SLPOUT   0x11
#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5
#define ST7735_COLMOD   0x3A
#define ST7735_GMCTRP1  0xE0
#define ST7735_GMCTRN1  0xE1
#define ST7735_NORON    0x13
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_INVOFF   0x20
#define ST7735_INVON    0x21

#define ST7735_MADCTL     0x36
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MV  0x20

void ST7735_Reset(void)
{
  HAL_GPIO_WritePin(ST7735_RST_GPIO_Port, ST7735_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(ST7735_RST_GPIO_Port, ST7735_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
}

void ST7735_WriteCommand(uint8_t cmd)
{
  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&ST7735_SPI_INSTANCE, &cmd, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

void ST7735_WriteByte(uint8_t data)
{
  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&ST7735_SPI_INSTANCE, &data, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

void ST7735_WriteData(uint8_t *data, size_t data_size)
{
  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&ST7735_SPI_INSTANCE, data, data_size, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

void ST7735_SetRotation(uint8_t rotation)
{
  uint8_t madctl = 0;

  switch (rotation)
  {
    case 0:
      madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_MODE;
      break;
    case 1:
      madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_MODE;
      break;
    case 2:
      madctl = ST7735_MADCTL_MODE;
      break;
    case 3:
      madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_MODE;
      break;
  }

  ST7735_WriteCommand(ST7735_MADCTL);
  ST7735_WriteByte(madctl);
}

void ST7735_Init(void) {
  // Initialize the display
  ST7735_Reset();
  ST7735_WriteCommand(ST7735_SLPOUT);
  HAL_Delay(120);
  ST7735_WriteCommand(ST7735_FRMCTR1);
  ST7735_WriteByte(0x01);
  ST7735_WriteByte(0x2C);
  ST7735_WriteByte(0x2D);
  ST7735_WriteCommand(ST7735_FRMCTR2);
  ST7735_WriteByte(0x01);
  ST7735_WriteByte(0x2C);
  ST7735_WriteByte(0x2D);
  ST7735_WriteCommand(ST7735_FRMCTR3);
  ST7735_WriteByte(0x01);
  ST7735_WriteByte(0x2C);
  ST7735_WriteByte(0x2D);
  ST7735_WriteByte(0x01);
  ST7735_WriteByte(0x2C);
  ST7735_WriteByte(0x2D);
  ST7735_WriteCommand(ST7735_INVCTR);
  ST7735_WriteByte(0x07);
  ST7735_WriteCommand(ST7735_PWCTR1);
  ST7735_WriteByte(0xA2);
  ST7735_WriteByte(0x02);
  ST7735_WriteByte(0x84);
  ST7735_WriteCommand(ST7735_PWCTR2);
  ST7735_WriteByte(0xC5);
  ST7735_WriteCommand(ST7735_PWCTR3);
  ST7735_WriteByte(0x0A);
  ST7735_WriteByte(0x00);
  ST7735_WriteCommand(ST7735_PWCTR4);
  ST7735_WriteByte(0x8A);
  ST7735_WriteByte(0x2A);
  ST7735_WriteCommand(ST7735_PWCTR5);
  ST7735_WriteByte(0x8A);
  ST7735_WriteByte(0xEE);
  ST7735_WriteCommand(ST7735_VMCTR1);
  ST7735_WriteByte(0x0E);
  ST7735_WriteCommand(ST7735_INVERSE ? ST7735_INVON : ST7735_INVOFF);
  ST7735_WriteCommand(ST7735_COLMOD);
  ST7735_WriteByte(0x05);
  ST7735_WriteCommand(ST7735_CASET);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x7F);
  ST7735_WriteCommand(ST7735_RASET);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x9F);
  ST7735_WriteCommand(ST7735_GMCTRP1);
  ST7735_WriteByte(0x02);
  ST7735_WriteByte(0x1C);
  ST7735_WriteByte(0x07);
  ST7735_WriteByte(0x12);
  ST7735_WriteByte(0x37);
  ST7735_WriteByte(0x32);
  ST7735_WriteByte(0x29);
  ST7735_WriteByte(0x2D);
  ST7735_WriteByte(0x29);
  ST7735_WriteByte(0x25);
  ST7735_WriteByte(0x2B);
  ST7735_WriteByte(0x39);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x01);
  ST7735_WriteByte(0x03);
  ST7735_WriteByte(0x10);
  ST7735_WriteCommand(ST7735_GMCTRN1);
  ST7735_WriteByte(0x03);
  ST7735_WriteByte(0x1D);
  ST7735_WriteByte(0x07);
  ST7735_WriteByte(0x06);
  ST7735_WriteByte(0x2E);
  ST7735_WriteByte(0x2C);
  ST7735_WriteByte(0x29);
  ST7735_WriteByte(0x2D);
  ST7735_WriteByte(0x2E);
  ST7735_WriteByte(0x2E);
  ST7735_WriteByte(0x37);
  ST7735_WriteByte(0x3F);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x00);
  ST7735_WriteByte(0x02);
  ST7735_WriteByte(0x10);
  ST7735_WriteCommand(ST7735_NORON);
  HAL_Delay(10);
  ST7735_WriteCommand(ST7735_DISPON);
  HAL_Delay(10);

  ST7735_SetRotation(ST7735_ROTATION);
  ST7735_FillScreen(ST7735_BLACK);
}

void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  x0 += ST7735_XSTART;
  y0 += ST7735_YSTART;

  x1 += ST7735_XSTART;
  y1 += ST7735_YSTART;

  ST7735_WriteCommand(ST7735_CASET);
  uint8_t data[] = { 0x00, x0, 0x00, x1};
  ST7735_WriteData(data, sizeof(data));

  ST7735_WriteCommand(ST7735_RASET);
  data[1] = y0;
  data[3] = y1;
  ST7735_WriteData(data, sizeof(data));
}

void ST7735_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  static uint8_t buff[ST7735_WIDTH * 2];
  uint16_t i = 0;

  for (i = 0; i < width; i++) {
    buff[i * 2] = color >> 8;
    buff[i * 2 + 1] = color & 0xFF;
  }

  ST7735_SetAddressWindow(x, y, x + width - 1, y + height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);
  // Write the color data
  for (i = 0; i < height; i++) {
    ST7735_WriteData(buff, sizeof(uint16_t) * width);
  }
}

void ST7735_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgColor, const FontDef *font)
{
  uint32_t i, b, j;

  ST7735_SetAddressWindow(x, y, x + font->width - 1, y + font->height - 1);
  ST7735_WriteCommand(0x2C);

  for (i = 0; i < font->height; i++) {
    b = font->data[(font == &Font_Custom ? (c - 46) : (c - 32)) * font->height + i];
    for (j = 0; j < font->width; j++) {
      if ((b << j) & (font->width > 16 ? 0x80000000:0x8000))  {
        uint8_t data[] = { color >> 8, color & 0xFF };
        ST7735_WriteData(data, sizeof(data));
      } else {
        uint8_t data[] = { bgColor >> 8, bgColor & 0xFF };
        ST7735_WriteData(data, sizeof(data));
      }
    }
  }
}

void ST7735_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const FontDef *font)
{
  while (*str) {
    if (x + font->width > ST7735_WIDTH) {
      x = 0;
      y += font->height;
    }

    if (y + font->height > ST7735_HEIGHT) {
      break;
    }

    ST7735_DrawChar(x, y, *str, color, bgColor, font);
    x += font->width;
    str++;
  }
}

static uint32_t ST7735_FontRowBits(const FontDef *font, char c, uint16_t row)
{
  uint32_t index;

  if (font == &Font_Custom) {
    if ((uint8_t)c < 46) {
      return 0;
    }
    index = (uint32_t)(c - 46);
  } else {
    if (((uint8_t)c < 32) || ((uint8_t)c > 126)) {
      c = ' ';
    }
    index = (uint32_t)(c - 32);
  }

  return font->data[index * font->height + row];
}

static uint8_t ST7735_FontPixelIsOn(const FontDef *font, char c, uint16_t src_x, uint16_t src_y)
{
  uint32_t row_bits = ST7735_FontRowBits(font, c, src_y);
  uint32_t mask = (font->width > 16) ? (0x80000000u >> src_x) : (0x8000u >> src_x);

  return (row_bits & mask) != 0;
}

static void ST7735_DrawCharSize(uint16_t x,
                                uint16_t y,
                                char c,
                                uint16_t color,
                                uint16_t bgColor,
                                const FontDef *font,
                                uint16_t height)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint8_t fg_hi = color >> 8;
  uint8_t fg_lo = color & 0xFF;
  uint8_t bg_hi = bgColor >> 8;
  uint8_t bg_lo = bgColor & 0xFF;
  uint16_t width;
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((font == NULL) || (font->width == 0) || (font->height == 0) || (height == 0)) {
    return;
  }

  width = ((uint32_t)font->width * height + (font->height / 2)) / font->height;
  if (width == 0) {
    width = 1;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = width;
  draw_height = height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x + draw_width - 1, y + draw_height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  for (row = 0; row < draw_height; row++) {
    uint16_t src_y = ((uint32_t)row * font->height) / height;

    for (col = 0; col < draw_width; col++) {
      uint16_t src_x = ((uint32_t)col * font->width) / width;
      uint8_t pixel_on = ST7735_FontPixelIsOn(font, c, src_x, src_y);

      line[col * 2] = pixel_on ? fg_hi : bg_hi;
      line[col * 2 + 1] = pixel_on ? fg_lo : bg_lo;
    }

    ST7735_WriteData(line, draw_width * 2);
  }
}

void ST7735_DrawStringSize(uint16_t x,
                           uint16_t y,
                           const char *str,
                           uint16_t color,
                           uint16_t bgColor,
                           const FontDef *font,
                           uint16_t height)
{
  uint16_t width;

  if ((str == NULL) || (font == NULL) || (font->width == 0) || (font->height == 0) || (height == 0)) {
    return;
  }

  width = ((uint32_t)font->width * height + (font->height / 2)) / font->height;
  if (width == 0) {
    width = 1;
  }

  while (*str) {
    if (*str == '\r') {
      str++;
      continue;
    }

    if (*str == '\n') {
      x = 0;
      y += height;
      str++;
      continue;
    }

    if (x + width > ST7735_WIDTH) {
      x = 0;
      y += height;
    }

    if (y + height > ST7735_HEIGHT) {
      break;
    }

    ST7735_DrawCharSize(x, y, *str, color, bgColor, font, height);
    x += width;
    str++;
  }
}

static const char kDefaultAsciiMap[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
typedef struct {
  const char *utf8;
  uint8_t utf8_len;
  const char *gbk;
  uint8_t gbk_len;
} ST7735_CharMap;

static const ST7735_CharMap kUtf8ToGbkMap[] = {
  { "\xE5\x8C\x97", 3, "\xB1\xB1", 2 }, /* bei */
  { "\xE4\xBA\x8C", 3, "\xB6\xFE", 2 }, /* er */
  { "\xE8\x82\xA5", 3, "\xB7\xCA", 2 }, /* fei */
  { "\xE6\xB5\xB7", 3, "\xBA\xA3", 2 }, /* hai */
  { "\xE5\x90\x88", 3, "\xBA\xCF", 2 }, /* he */
  { "\xE7\x8E\xAF", 3, "\xBB\xB7", 2 }, /* huan */
  { "\xE4\xBA\xAC", 3, "\xBE\xA9", 2 }, /* jing */
  { "\xE5\xA2\x83", 3, "\xBE\xB3", 2 }, /* jing2 */
  { "\xE5\x85\xAD", 3, "\xC1\xF9", 2 }, /* liu */
  { "\xE5\x8D\x97", 3, "\xC4\xCF", 2 }, /* nan */
  { "\xE5\x86\x85", 3, "\xC4\xDA", 2 }, /* nei */
  { "\xE6\x9C\x9F", 3, "\xC6\xDA", 2 }, /* qi */
  { "\xE6\x97\xA5", 3, "\xC8\xD5", 2 }, /* ri */
  { "\xE4\xB8\x89", 3, "\xC8\xFD", 2 }, /* san */
  { "\xE4\xB8\x8A", 3, "\xC9\xCF", 2 }, /* shang */
  { "\xE5\xAE\xA4", 3, "\xCA\xD2", 2 }, /* shi */
  { "\xE5\x9B\x9B", 3, "\xCB\xC4", 2 }, /* si */
  { "\xE5\xA4\xA9", 3, "\xCC\xEC", 2 }, /* tian */
  { "\xE4\xBA\x94", 3, "\xCE\xE5", 2 }, /* wu */
  { "\xE6\x98\x9F", 3, "\xD0\xC7", 2 }, /* xing */
  { "\xE4\xB8\x80", 3, "\xD2\xBB", 2 }, /* yi */
  { "\xE6\x8E\xA5", 3, "\xBD\xD3", 2 }, /* jie */
  { "\xE8\xBF\x9E", 3, "\xC1\xAC", 2 }, /* lian */
  { "\xE6\xB7\xB1", 3, "\xC9\xEE", 2 }, /* shen */
  { "\xE5\x9C\xB3", 3, "\xDB\xDA", 2 }, /* zhen */
  { "\xE6\xB3\x89", 3, "\xC8\xAA", 2 }, /* quan */
  { "\xE5\xB7\x9E", 3, "\xD6\xDD", 2 }, /* zhou */
  { "\xE4\xB8\xAD", 3, "\xD6\xD0", 2 }, /* zhong */
  { "\xE8\x8A\xB1", 3, "\xBB\xA8", 2 }, /* hua */
  { "\xE6\xA2\x85", 3, "\xC3\xB7", 2 }, /* mei */
  { "\xE6\xB0\x94", 3, "\xC6\xF8", 2 }, /* qi2 */
  { "\xE5\xB5\x8C", 3, "\xC7\xB6", 2 }, /* qian */
  { "\xE5\x85\xA5", 3, "\xC8\xEB", 2 }, /* ru */
  { "\xE6\x97\xB6", 3, "\xCA\xB1", 2 }, /* shi2 */
  { "\xE5\xBC\x8F", 3, "\xCA\xBD", 2 }, /* shi3 */
  { "\xE9\x92\x9F", 3, "\xD6\xD3", 2 }, /* zhong2 */
};

static uint8_t ST7735_Utf8CharLen(const char *str)
{
  uint8_t first = (uint8_t)str[0];

  if (first < 0x80) {
    return 1;
  }

  if ((first & 0xE0) == 0xC0) {
    return 2;
  }

  if ((first & 0xF0) == 0xE0) {
    return 3;
  }

  if ((first & 0xF8) == 0xF0) {
    return 4;
  }

  return 1;
}

static int ST7735_AsciiIndex(const font_t *font, char c)
{
  const char *map = font->ascii_map ? font->ascii_map : kDefaultAsciiMap;
  const char *hit = strchr(map, c);

  if (hit == NULL) {
    return -1;
  }

  return (int)(hit - map);
}

static const font_chinese_t *ST7735_ChineseGlyphByBytes(const font_t *font,
                                                        const char *str,
                                                        uint8_t target_len,
                                                        uint8_t *matched_len)
{
  const font_chinese_t *glyph = font->chinese;
  size_t remaining = strlen(str);
  size_t name_len;

  if (glyph == NULL) {
    return NULL;
  }

  while (glyph->name != NULL) {
    name_len = strlen(glyph->name);
    if ((name_len == target_len) && (name_len <= remaining) && (memcmp(glyph->name, str, name_len) == 0)) {
      *matched_len = (uint8_t)name_len;
      return glyph;
    }
    glyph++;
  }

  return NULL;
}

static const font_chinese_t *ST7735_ChineseGlyph(const font_t *font, const char *str, uint8_t *matched_len)
{
  const font_chinese_t *glyph;
  size_t i;

  glyph = ST7735_ChineseGlyphByBytes(font, str, *matched_len, matched_len);
  if (glyph != NULL) {
    return glyph;
  }

  if (*matched_len == 3) {
    for (i = 0; i < (sizeof(kUtf8ToGbkMap) / sizeof(kUtf8ToGbkMap[0])); i++) {
      if (memcmp(str, kUtf8ToGbkMap[i].utf8, kUtf8ToGbkMap[i].utf8_len) == 0) {
        glyph = ST7735_ChineseGlyphByBytes(font, kUtf8ToGbkMap[i].gbk, kUtf8ToGbkMap[i].gbk_len, matched_len);
        if (glyph != NULL) {
          *matched_len = kUtf8ToGbkMap[i].utf8_len;
          return glyph;
        }
      }
    }
  }

  return NULL;
}

static void ST7735_DrawGlyph(uint16_t x,
                             uint16_t y,
                             uint16_t width,
                             uint16_t height,
                             const uint8_t *bitmap,
                             uint16_t bytes_per_row,
                             uint16_t color,
                             uint16_t bgColor)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint8_t fg_hi = color >> 8;
  uint8_t fg_lo = color & 0xFF;
  uint8_t bg_hi = bgColor >> 8;
  uint8_t bg_lo = bgColor & 0xFF;
  uint16_t row;
  uint16_t col;

  if ((width == 0) || (height == 0) || (x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  if (x + width > ST7735_WIDTH) {
    width = ST7735_WIDTH - x;
  }

  if (y + height > ST7735_HEIGHT) {
    height = ST7735_HEIGHT - y;
  }

  if ((width == 0) || (height == 0)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x + width - 1, y + height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  for (row = 0; row < height; row++) {
    const uint8_t *row_data = (bitmap == NULL) ? NULL : (bitmap + (row * bytes_per_row));

    for (col = 0; col < width; col++) {
      uint8_t pixel_on = 0;

      if (row_data != NULL) {
        uint8_t byte = row_data[col / 8];
        pixel_on = (byte & (0x80u >> (col % 8))) != 0;
      }

      line[col * 2] = pixel_on ? fg_hi : bg_hi;
      line[col * 2 + 1] = pixel_on ? fg_lo : bg_lo;
    }

    ST7735_WriteData(line, width * 2);
  }
}

static void ST7735_DrawGlyphScaled(uint16_t x,
                                   uint16_t y,
                                   uint16_t src_width,
                                   uint16_t src_height,
                                   const uint8_t *bitmap,
                                   uint16_t bytes_per_row,
                                   uint16_t dst_width,
                                   uint16_t dst_height,
                                   uint16_t color,
                                   uint16_t bgColor)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint8_t fg_hi = color >> 8;
  uint8_t fg_lo = color & 0xFF;
  uint8_t bg_hi = bgColor >> 8;
  uint8_t bg_lo = bgColor & 0xFF;
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((src_width == 0) || (src_height == 0) || (dst_width == 0) || (dst_height == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = dst_width;
  draw_height = dst_height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x + draw_width - 1, y + draw_height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  for (row = 0; row < draw_height; row++) {
    uint16_t src_y = ((uint32_t)row * src_height) / dst_height;
    const uint8_t *row_data = (bitmap == NULL) ? NULL : (bitmap + (src_y * bytes_per_row));

    for (col = 0; col < draw_width; col++) {
      uint8_t pixel_on = 0;

      if (row_data != NULL) {
        uint16_t src_x = ((uint32_t)col * src_width) / dst_width;
        uint8_t byte = row_data[src_x / 8];
        pixel_on = (byte & (0x80u >> (src_x % 8))) != 0;
      }

      line[col * 2] = pixel_on ? fg_hi : bg_hi;
      line[col * 2 + 1] = pixel_on ? fg_lo : bg_lo;
    }

    ST7735_WriteData(line, draw_width * 2);
  }
}

static void ST7735_DrawStringMapleInternal(uint16_t x,
                                           uint16_t y,
                                           const char *str,
                                           uint16_t color,
                                           uint16_t bgColor,
                                           const font_t *font,
                                           uint16_t size)
{
  uint8_t utf8_len;
  uint8_t i;
  uint16_t src_width;
  uint16_t src_height;
  uint16_t dst_width;
  uint16_t dst_height;
  uint16_t bytes_per_row;
  const uint8_t *bitmap;

  if ((str == NULL) || (font == NULL) || (font->size == 0) || (size == 0)) {
    return;
  }

  while (*str != '\0') {
    src_height = font->size;
    dst_height = size;
    bitmap = NULL;

    if (*str == '\r') {
      str++;
      continue;
    }

    if (*str == '\n') {
      x = 0;
      y += dst_height;
      str++;
      continue;
    }

    utf8_len = ST7735_Utf8CharLen(str);
    for (i = 1; i < utf8_len; i++) {
      if (str[i] == '\0') {
        utf8_len = 1;
        break;
      }
    }

    if ((uint8_t)str[0] < 0x80) {
      int index = ST7735_AsciiIndex(font, *str);

      src_width = font->size / 2;
      dst_width = size / 2;
      bytes_per_row = (src_width + 7) / 8;

      if ((index >= 0) && (font->ascii_model != NULL)) {
        bitmap = font->ascii_model + ((uint32_t)index * src_height * bytes_per_row);
      }
    } else {
      const font_chinese_t *glyph;

      glyph = ST7735_ChineseGlyph(font, str, &utf8_len);

      src_width = font->size;
      dst_width = size;
      bytes_per_row = (src_width + 7) / 8;

      if (glyph != NULL) {
        bitmap = glyph->model;
      } else if (((uint8_t)str[0] >= 0x80) && (utf8_len == 1) && (str[1] != '\0')) {
        utf8_len = 2;
      }
    }

    if ((dst_width == 0) || (dst_height == 0)) {
      break;
    }

    if ((x + dst_width > ST7735_WIDTH) && (x != 0)) {
      x = 0;
      y += dst_height;
    }

    if (y + dst_height > ST7735_HEIGHT) {
      break;
    }

    if (size == font->size) {
      ST7735_DrawGlyph(x, y, src_width, src_height, bitmap, bytes_per_row, color, bgColor);
    } else {
      ST7735_DrawGlyphScaled(x, y, src_width, src_height, bitmap, bytes_per_row, dst_width, dst_height, color, bgColor);
    }

    x += dst_width;
    str += utf8_len;
  }
}

void ST7735_DrawStringMaple(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const font_t *font)
{
  if (font == NULL) {
    return;
  }

  ST7735_DrawStringMapleInternal(x, y, str, color, bgColor, font, font->size);
}

void ST7735_DrawStringMapleSize(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgColor, const font_t *font, uint16_t size)
{
  ST7735_DrawStringMapleInternal(x, y, str, color, bgColor, font, size);
}

void ST7735_FillScreen(uint16_t color)
{
  ST7735_DrawRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  uint8_t data[2];

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x, y);
  ST7735_WriteCommand(ST7735_RAMWR);

  data[0] = color >> 8;
  data[1] = color & 0xFF;
  ST7735_WriteData(data, sizeof(data));
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image)
{
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;

  if ((image == NULL) || (width == 0) || (height == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = width;
  draw_height = height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  ST7735_SetAddressWindow(x, y, x + draw_width - 1, y + draw_height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  for (row = 0; row < draw_height; row++) {
    ST7735_WriteData((uint8_t *)&image[row * width * 2], draw_width * 2);
  }
}

void ST7735_DrawImageLE(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((image == NULL) || (width == 0) || (height == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = width;
  draw_height = height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x + draw_width - 1, y + draw_height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  for (row = 0; row < draw_height; row++) {
    const uint8_t *src = &image[(uint32_t)row * width * 2];

    for (col = 0; col < draw_width; col++) {
      line[col * 2] = src[col * 2 + 1];
      line[col * 2 + 1] = src[col * 2];
    }

    ST7735_WriteData(line, draw_width * 2);
  }
}

void ST7735_DrawImageLESize(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image, uint16_t size)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint16_t dst_width;
  uint16_t dst_height;
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((image == NULL) || (width == 0) || (height == 0) || (size == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  if (width >= height) {
    dst_width = size;
    dst_height = ((uint32_t)height * size + (width / 2)) / width;
  } else {
    dst_height = size;
    dst_width = ((uint32_t)width * size + (height / 2)) / height;
  }

  if (dst_width == 0) {
    dst_width = 1;
  }

  if (dst_height == 0) {
    dst_height = 1;
  }

  draw_width = dst_width;
  draw_height = dst_height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x + draw_width - 1, y + draw_height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  for (row = 0; row < draw_height; row++) {
    uint16_t src_y = ((uint32_t)row * height) / dst_height;
    const uint8_t *src_row = &image[(uint32_t)src_y * width * 2];

    for (col = 0; col < draw_width; col++) {
      uint16_t src_x = ((uint32_t)col * width) / dst_width;

      line[col * 2] = src_row[src_x * 2 + 1];
      line[col * 2 + 1] = src_row[src_x * 2];
    }

    ST7735_WriteData(line, draw_width * 2);
  }
}

void ST7735_DrawImageTransparent(uint16_t x,
                                 uint16_t y,
                                 uint16_t width,
                                 uint16_t height,
                                 const uint8_t *image,
                                 uint16_t transparentColor)
{
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((image == NULL) || (width == 0) || (height == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = width;
  draw_height = height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  for (row = 0; row < draw_height; row++) {
    uint16_t run_start = 0;
    uint8_t run_active = 0;

    for (col = 0; col < draw_width; col++) {
      uint32_t src_index = ((uint32_t)row * width + col) * 2;
      uint16_t pixel = ((uint16_t)image[src_index] << 8) | image[src_index + 1];

      if (pixel != transparentColor) {
        if (!run_active) {
          run_start = col;
          run_active = 1;
        }
      } else if (run_active) {
        uint16_t run_len = col - run_start;

        ST7735_SetAddressWindow(x + run_start, y + row, x + col - 1, y + row);
        ST7735_WriteCommand(ST7735_RAMWR);
        ST7735_WriteData((uint8_t *)&image[((uint32_t)row * width + run_start) * 2], run_len * 2);
        run_active = 0;
      }
    }

    if (run_active) {
      uint16_t run_len = draw_width - run_start;

      ST7735_SetAddressWindow(x + run_start, y + row, x + draw_width - 1, y + row);
      ST7735_WriteCommand(ST7735_RAMWR);
      ST7735_WriteData((uint8_t *)&image[((uint32_t)row * width + run_start) * 2], run_len * 2);
    }
  }
}

void ST7735_DrawImageTransparentLE(uint16_t x,
                                   uint16_t y,
                                   uint16_t width,
                                   uint16_t height,
                                   const uint8_t *image,
                                   uint16_t transparentColor)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((image == NULL) || (width == 0) || (height == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = width;
  draw_height = height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  for (row = 0; row < draw_height; row++) {
    uint16_t run_start = 0;
    uint16_t run_len = 0;
    const uint8_t *src = &image[(uint32_t)row * width * 2];

    for (col = 0; col < draw_width; col++) {
      uint16_t pixel = ((uint16_t)src[col * 2 + 1] << 8) | src[col * 2];

      if (pixel != transparentColor) {
        if (run_len == 0) {
          run_start = col;
        }

        line[run_len * 2] = src[col * 2 + 1];
        line[run_len * 2 + 1] = src[col * 2];
        run_len++;
      } else if (run_len != 0) {
        ST7735_SetAddressWindow(x + run_start, y + row, x + run_start + run_len - 1, y + row);
        ST7735_WriteCommand(ST7735_RAMWR);
        ST7735_WriteData(line, run_len * 2);
        run_len = 0;
      }
    }

    if (run_len != 0) {
      ST7735_SetAddressWindow(x + run_start, y + row, x + run_start + run_len - 1, y + row);
      ST7735_WriteCommand(ST7735_RAMWR);
      ST7735_WriteData(line, run_len * 2);
    }
  }
}

static void ST7735_DrawBitmap1BppGeneric(uint16_t x,
                                         uint16_t y,
                                         uint16_t width,
                                         uint16_t height,
                                         const uint8_t *bitmap,
                                         uint16_t color,
                                         uint16_t bgColor,
                                         uint8_t vertical)
{
  static uint8_t line[ST7735_WIDTH * 2];
  uint8_t fg_hi = color >> 8;
  uint8_t fg_lo = color & 0xFF;
  uint8_t bg_hi = bgColor >> 8;
  uint8_t bg_lo = bgColor & 0xFF;
  uint16_t draw_width;
  uint16_t draw_height;
  uint16_t row;
  uint16_t col;

  if ((bitmap == NULL) || (width == 0) || (height == 0)) {
    return;
  }

  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) {
    return;
  }

  draw_width = width;
  draw_height = height;

  if (x + draw_width > ST7735_WIDTH) {
    draw_width = ST7735_WIDTH - x;
  }

  if (y + draw_height > ST7735_HEIGHT) {
    draw_height = ST7735_HEIGHT - y;
  }

  if ((draw_width == 0) || (draw_height == 0)) {
    return;
  }

  ST7735_SetAddressWindow(x, y, x + draw_width - 1, y + draw_height - 1);
  ST7735_WriteCommand(ST7735_RAMWR);

  if (vertical) {
    for (row = 0; row < draw_height; row++) {
      for (col = 0; col < draw_width; col++) {
        uint16_t src_byte_index = (row / 8) * width + col;
        uint8_t pixel_on = (bitmap[src_byte_index] & (0x80u >> (row % 8))) != 0;

        line[col * 2] = pixel_on ? fg_hi : bg_hi;
        line[col * 2 + 1] = pixel_on ? fg_lo : bg_lo;
      }

      ST7735_WriteData(line, draw_width * 2);
    }
  } else {
    uint16_t bytes_per_row = (width + 7) / 8;

    for (row = 0; row < draw_height; row++) {
      const uint8_t *row_data = bitmap + (row * bytes_per_row);

      for (col = 0; col < draw_width; col++) {
        uint8_t byte = row_data[col / 8];
        uint8_t pixel_on = (byte & (0x80u >> (col % 8))) != 0;

        line[col * 2] = pixel_on ? fg_hi : bg_hi;
        line[col * 2 + 1] = pixel_on ? fg_lo : bg_lo;
      }

      ST7735_WriteData(line, draw_width * 2);
    }
  }
}

void ST7735_DrawBitmap1Bpp(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *bitmap, uint16_t color, uint16_t bgColor)
{
  ST7735_DrawBitmap1BppGeneric(x, y, width, height, bitmap, color, bgColor, 0);
}

void ST7735_DrawBitmap1BppV(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *bitmap, uint16_t color, uint16_t bgColor)
{
  ST7735_DrawBitmap1BppGeneric(x, y, width, height, bitmap, color, bgColor, 1);
}
/*绘制界面布局 上层蓝色 左橙色 右白色*/
void UI_DrawLayout(void)
{
  ST7735_DrawRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, UI_COLOR_BG);
  ST7735_DrawRectangle(3, 3, 122, 76, UI_COLOR_TOP);
  ST7735_DrawRectangle(3, 85, 58, 72, UI_COLOR_LEFT);
  ST7735_DrawRectangle(67, 85, 58, 72, UI_COLOR_RIGHT);
}
