#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library

#define TFT_CS        10
#define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
  //tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(0); // set display orientation 
}

void loop() {
  tft.fillScreen(ST77XX_WHITE);
  print_text(25,30,"HELLO",3,ST77XX_ORANGE);
  print_text(20,70,"WORLD!",3,ST77XX_BLUE);
  delay(5000);
  
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(5000);
  
  tft.fillScreen(ST77XX_CYAN);
  tft.drawRect(5,5,120,120,ST77XX_RED);
  tft.drawFastHLine(5,60,120,ST77XX_RED);
  tft.drawFastVLine(60,5,120,ST77XX_RED);
  delay(5000);
}

void print_text(byte x_pos, byte y_pos, char *text, byte text_size, uint16_t color) {
  tft.setCursor(x_pos, y_pos);
  tft.setTextSize(text_size);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}