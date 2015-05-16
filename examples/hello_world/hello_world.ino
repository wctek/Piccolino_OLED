// For the OLED, we need I2C
#include <Wire.h>

// For the FRAM we need SPI
#include <SPI.h>

// I2C OLED library
#include <Piccolino_OLED.h>

Piccolino_OLED display; // our OLED object ...


void setup() {

  display.begin();

}


void loop () {
  
   display.clear();
   display.setCursor(0,16); // skip yellow portion of display
   
   for(int f=0; f<14; f+=3) {
     display.drawLine(0,0,127,f,WHITE);
     display.drawLine(0,14-f,127,0,WHITE);
   }
   display.update();
   
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.print("Size 1\n");
   display.update();
   display.setTextSize(2);
   display.print("Size 2\n");
   display.update();
   display.setTextSize(3);
   display.print("Size 3\n");
   display.update();
   delay(3000);

   display.setTextSize(1);
   display.clear();
   
   for(char f=0; f<50; f++) {
     display.drawLine(0,random(60),127,random(60),WHITE);
     display.update();
   }

  delay(3000); 
  display.clear();
  display.update();

  for(char f=0; f<3; f++) {
    display.clear();
    display.update();
    
    display.setCursor(0,16);
    display.setTextSize(1);
    display.print("Size 1\n");
    display.setTextSize(2);
    display.print("Size 2\n");
    display.setTextSize(3);
    display.print("Size 3\n");
    display.update();
    delay(500);
  } 
  delay(3000);
  display.setTextSize(1);
   
     for(char f=0; f<20; f++) {
     display.clear();
     display.setCursor(random(50),random(45)+16); // just move the text around ...
     display.print(F("Hello, world!"));
     display.update();
     delay(10);
   }

}