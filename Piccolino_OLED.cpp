//#include <avr/pgmspace.h>

/*************************************************************
* VERY (very) basic I2C library for the SSD1306 OLED LCD     *
* Some of the functions were borrowed from Adafruit's GFX    *
* Library, so thank her for them (see below).                *
*                                                            *
* Version: 1.2 - June 2015                                   *
*                                                            *
* Written by fr333n3rgy@yahoo.com for WCTEK.com              *
*************************************************************/


#include "Piccolino_OLED.h"
#include "glcdfont.c"
#include <Piccolino_RAM.h>

Piccolino_RAM _v_ram;

/**
 * Initialize the class and set the default settings
 */
Piccolino_OLED::Piccolino_OLED() {
  cursor_x = 0;
  cursor_y = 0;
  textcolor = WHITE;
  textbgcolor = BLACK;
  _switchvss = SSD1306_SWITCHCAPVCC;
  _i2caddr = SSD1306_I2C_ADDRESS;
  buff = NULL;
}

void Piccolino_OLED::setVss(uint8_t vss) {
  _switchvss = vss;
}

void Piccolino_OLED::setI2CAddr(uint8_t i2caddr) {
  _i2caddr = i2caddr;
}

void Piccolino_OLED::begin(bool use_sram) {
  bool external_vcc = _switchvss == SSD1306_EXTERNALVCC;

  sram = use_sram;
  if(sram) {
    // Initialize RAM
    _v_ram.begin(ADDR_VIDEOBUFFER);
    buff = (byte *)calloc(TMP_BUFFER_SIZE, sizeof(byte));
  } else {
    // Allocate the buffer to 1K
    buff = (byte *)calloc(BUFFER_SIZE, sizeof(byte));
  }
  if(!buff) {
    abort();
  }

  // set pin directions
  // I2C Init
  Wire.begin(); // Is this the right place for this?

  // Init sequence for 128x64 OLED module
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80
  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(0x3F);
  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  ssd1306_command(external_vcc ? 0x10 : 0x14);
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_command(external_vcc ? 0x9F: 0xCF);
  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  ssd1306_command(external_vcc ? 0x22 : 0xF1);
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

  clear(); // clear sram ...
  setTextColor(WHITE);
  setTextSize(1);
  update();

  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}

/**
 * Initialize the display using SRAM
 *
 * Slower but uses only 128 bytes in local memory instead
 * of 1K.
 */
void Piccolino_OLED::begin_sram() {
  begin(true);
}

void Piccolino_OLED::clearpart(int from)
{
  clearpart(from,7);
}

void Piccolino_OLED::dim(bool how)
{
    ssd1306_command(SSD1306_SETCONTRAST);

    if(how==ON)
      ssd1306_command(0x25);
    else
      ssd1306_command(0xCF);
}

void Piccolino_OLED::clearpart(int from, int tto)
{
  if(sram) {
    memset(buff, 0, TMP_BUFFER_SIZE);
    for(int f=from; f<=tto; f++) {
      _v_ram.write(f*SSD1306_LCDWIDTH, buff, TMP_BUFFER_SIZE);
    }
  } else {
    int bytestoclear=((tto+1)-from)*128;
    memset(&buff[from*128],0,bytestoclear);
  }
}

void Piccolino_OLED::displayOFF()
{
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE    
}

void Piccolino_OLED::displayON()
{
  ssd1306_command(SSD1306_DISPLAYON);                    // 0xAE    
}

void Piccolino_OLED::invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

void Piccolino_OLED::ssd1306_command(uint8_t c) {
    // I2C
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}


void Piccolino_OLED::ssd1306_data(uint8_t c) {
    // I2C
    uint8_t control = 0x40;   // Co = 0, D/C = 1
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}

void Piccolino_OLED::update()
{
  updateRow(0, 8);
}

void Piccolino_OLED::clearLine(byte line)
{
    ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
    ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

    // save I2C bitrate
    uint8_t twbrbackup = TWBR;
    TWBR = 18; // upgrade to 400KHz!

    // send a bunch of data in one xmission
    ssd1306_command(0xB0 + line);//set page address
    ssd1306_command(0);//set lower column address
    ssd1306_command(0x10);//set higher column address

    for(byte j = 0; j < 8; j++){
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x40);
        for (byte k = 0; k < 16; k++) {
            Wire.write(0);
        }
        Wire.endTransmission();
    }

    TWBR = twbrbackup;
}


void Piccolino_OLED::updateRow(int rowID)
{
  unsigned char x;
  int pos=0;
  byte *buffer = NULL;

  ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

  // save I2C bitrate
  uint8_t twbrbackup = TWBR;
  TWBR = 18; // upgrade to 400KHz!

  Wire.beginTransmission(_i2caddr);
  Wire.write(0x00); // send command
  Wire.write(0xB0+rowID);
  Wire.write(0);
  Wire.endTransmission();

  if(sram) {
    _v_ram.read(rowID*SSD1306_LCDWIDTH, buff, TMP_BUFFER_SIZE);
    buffer = buff;
  } else {
    pos = rowID*SSD1306_LCDWIDTH;
    buffer = buff;
  }

  for(byte j = 0; j < 8; j++){
    Wire.beginTransmission(_i2caddr);
    Wire.write(0x40);
    for (byte k = 0; k < 16; k++) {
        Wire.write(buffer[pos++]);
    }
    Wire.endTransmission();
  }
  TWBR = twbrbackup;
}

void Piccolino_OLED::updateRow(int startID, int endID)
{
  unsigned char y =0;
  for(y=startID; y<endID; y++)
  {
    updateRow(y);
  }
}


// Original concept borrowed from Adafruit's GFX library modified for Piccolino by AS
void Piccolino_OLED::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  int row;
  unsigned char offset;
  unsigned char  preData; //previous data.
  unsigned char result;
  unsigned char val;

  if (x>127||y>63) return;
  if (x<0||y<0) return;

  row = y/8;
  offset =y%8;

  x+=row*SSD1306_LCDWIDTH;
  if(sram) {
    _v_ram.read(x, buff);
    preData = buff[0];
  } else {
    preData = buff[x];
  }

  //set pixel;
  val = 1<<offset;

  if(color!=0)
  {//white! set bit.
    result = preData | val;
  }
  else
  {       //black! clear bit.
    result = preData & (~val);
  }
  if(sram) {
    buff[0] = result;
    _v_ram.write(x, buff);
  } else {
    buff[x] = result;
  }
}

void Piccolino_OLED::clear()
{
  	clearpart(0);
}

// Original concept borrowed from Adafruit's GFX library
void Piccolino_OLED::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font+(c*5)+i);

    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void Piccolino_OLED::setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void Piccolino_OLED::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void Piccolino_OLED::setTextColor(uint16_t c) {
  textcolor = c;
}
void Piccolino_OLED::setTextColor(uint16_t c,uint16_t b) {
  textcolor = c;
  textbgcolor = b;
}

void Piccolino_OLED::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {  
 for (int16_t i=x; i<x+w; i++) {
    drawLine(i, y, i, y+h-1, color);
  }
}

void Piccolino_OLED::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {  
  drawLine(x, y, x, y+h, color);
  drawLine(x+w, y, x+w, y+h, color);
  drawLine(x, y, x+w, y, color);
  drawLine(x, y+h, x+w, y+h, color);
}

// Bresenham's algorithm - borrowed from Adafruit's GFX Library
void Piccolino_OLED::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      if(color==GRAY)
        drawPixel(y0, x0, !(x0%2));
      else
        drawPixel(y0, x0, color);
    } else {
      if(color==GRAY)
        drawPixel(x0, y0, !(x0%2));
      else
        drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

/**
 * Get the color that should be used
 *
 * This function is to deal with Gray since every other pixel is printed
 *
 * @param color The color that is requested
 * @param index Current index of the pixel that is to be printed
 * @return The color to be used
 */
uint16_t Piccolino_OLED::getColor(uint16_t color, uint32_t index)
{
  if(color == GRAY) {
    return !(index % 2);
  }
  return color;
}

/**
 * Draw a circle with an option to fill it
 *
 * NOTE: The caller must call the display update function
 * in order for the circle to be displayed.
 *
 * @param cx The center of the circle - X
 * @param cy The center of the circle - Y
 * @param radius The radius of the circle
 * @param color The color of the circle and fill if applicable
 * @param fill Whether to fill the circle
 */
void Piccolino_OLED::drawCircle(
  int16_t cx, int16_t cy, int16_t radius,
  uint16_t color, bool fill)
{
  int deg;
  double px, py;
  uint16_t cur_color;
  for(deg = 0; deg < 360; deg++) {
    px = cx + (cos(deg) * radius);
    py = cy + (sin(deg) * radius);
    cur_color = getColor(color, deg);
    drawPixel(px, py, cur_color);

    // Filling
    if(fill) {
      int ix;
      if(px >= cx) {
        for(ix = cx; ix < px; ix++) {
          drawPixel(ix, py, cur_color);
        }
      } else {
        for(ix = cx; ix > px; ix--) {
          drawPixel(ix, py, cur_color);
        }
      }
    }
  }
}


size_t Piccolino_OLED::write(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(1+cursor_x, 1+cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (SSD1306_LCDWIDTH - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
  return 1;
}

Piccolino_OLED::~Piccolino_OLED()
{
  if(buff)
    free(buff);
}
