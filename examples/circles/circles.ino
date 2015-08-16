// For the OLED, we need I2C
#include <Wire.h>

// For the FRAM we need SPI
#include <SPI.h>

// I2C OLED library
#include <Piccolino_OLED.h>

// Piccolino RAM library
#include <Piccolino_RAM.h>

Piccolino_OLED display; // our OLED object ...

void setup() {
  // This will use local memory and should look faster
  display.begin();

  // This will use SRAM and only 128 byte of local memory
  // Try this out by commenting out the display.begin() and
  // uncommenting display.begin_sram().
  //display.begin_sram();
}

void loop() {
  display.clear();
  int cx = 20 + rand() % (display.width / 2);
  int cy = 5 + rand() % (display.height / 2);
  int radius = rand() % 20;
  int bg = rand() % 2;
  display.drawCircle(cx, cy, radius, WHITE, bg);
  display.update();
}
