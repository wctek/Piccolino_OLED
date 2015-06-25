#include "Arduino.h"
#include <Wire.h>

#ifndef _PICCOLINO_OLED_H
#define _PICCOLINO_OLED_H

#define ADDR_VIDEOBUFFER              0x7c00

#define SSD1306_I2C_ADDRESS           0x3C	// 011110+SA0+RW - 0x3C or 0x3D
#define SSD1306_128_64
#define SSD1306_LCDWIDTH              128
#define SSD1306_LCDHEIGHT             64
#define SSD1306_SETCONTRAST           0x81
#define SSD1306_DISPLAYALLON_RESUME   0xA4
#define SSD1306_DISPLAYALLON          0xA5
#define SSD1306_NORMALDISPLAY         0xA6
#define SSD1306_INVERTDISPLAY         0xA7
#define SSD1306_DISPLAYOFF            0xAE
#define SSD1306_DISPLAYON             0xAF
#define SSD1306_SETDISPLAYOFFSET      0xD3
#define SSD1306_SETCOMPINS            0xDA
#define SSD1306_SETVCOMDETECT         0xDB
#define SSD1306_SETDISPLAYCLOCKDIV    0xD5
#define SSD1306_SETPRECHARGE          0xD9
#define SSD1306_SETMULTIPLEX          0xA8
#define SSD1306_SETLOWCOLUMN          0x00
#define SSD1306_SETHIGHCOLUMN         0x10
#define SSD1306_SETSTARTLINE          0x40
#define SSD1306_MEMORYMODE            0x20
#define SSD1306_COMSCANINC            0xC0
#define SSD1306_COMSCANDEC            0xC8
#define SSD1306_SEGREMAP              0xA0
#define SSD1306_CHARGEPUMP            0x8D
#define SSD1306_EXTERNALVCC           0x1
#define SSD1306_SWITCHCAPVCC          0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL                       0x2F
#define SSD1306_DEACTIVATE_SCROLL                     0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA              0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL               0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL  0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL   0x2A

#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define WHITE 1
#define BLACK 0
#define ON    1
#define OFF   0

class Piccolino_OLED : public Print  {
public:

  Piccolino_OLED();
  ~Piccolino_OLED();

  void begin(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = SSD1306_I2C_ADDRESS);
  void ssd1306_command(uint8_t c);
  void ssd1306_data(uint8_t c);

  void invertDisplay(uint8_t i);
  
  void update();
  void clearBuffer();

  //void setCursor(byte column, byte line);
  void clear();
  void clearpart(int from);
  void clearpart(int from, int tto);
  void clearLine(byte line);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
  void setCursor(int16_t x, int16_t y);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void setTextSize(uint8_t s);
  void setTextColor(uint16_t c);
  void setTextColor(uint16_t c, uint16_t bg);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  virtual size_t write(uint8_t);
  void updateRow(int rowID);
  void updateRow(int startID, int endID);
  byte buff[1024]; // video buffer
  void displayOFF();
  void displayON();
  void dim(bool how);

protected:
  uint8_t _i2caddr;
    int16_t cursor_x, cursor_y, textcolor, textbgcolor;
    uint8_t textsize;
    boolean  wrap; // If set, 'wrap' text at right edge of display

private:
  int8_t sclk, dc, rst, cs;
  byte m_col;
  byte m_row;
};

#endif