#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST 8  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// TFT FUNCTIONS

void tft_printc(short x, short y, const char chr, uint16_t color = ST7735_BLACK, uint16_t bg = ST7735_WHITE) {
    static uint16_t colorArray[8 * 16];
    int counter = 0;
    for (int r = 0; r < 16; r++) {
        uint8_t temp = pgm_read_word_near(asciiBitmap[chr - ' '] + r);
        for (int c = 0; c < 8; c++) {
            colorArray[counter++] = (temp & 0x01) ? color : bg;
            temp >>= 1;
        }
    }
    tft.startWrite();
    tft.drawRGBBitmap(x, y, colorArray, 8, 16);
    tft.endWrite();
}

void _tft_printbigc(short x, short y, const char chr, short w = 8, short h = 16, uint16_t color = ST7735_BLACK, uint16_t bg = ST7735_WHITE) {
    uint16_t *colorArray = new uint16_t[w * h];
    int counter = 0;
    for (int r = 0; r < h; r++) {
        int rr = (r + 0.5) / h * 16;
        uint8_t temp = pgm_read_word_near(asciiBitmap[chr - ' '] + rr);
        for (int c = 0; c < w; c++) {
            int cc = (c + 0.5) / w * 8;
            colorArray[counter++] = ((temp >> (cc)) & 0x01) ? color : bg;
        }
    }
    tft.startWrite();
    tft.drawRGBBitmap(x, y, colorArray, w, h);
    tft.endWrite();
    delete colorArray;
}

void tft_prints(short x, short y, const char *str, uint16_t color = ST7735_BLACK, uint16_t bg = ST7735_WHITE) {
    int counter = 0;
    while (*str != '\0') {
        tft_printc(x + 8*(counter++), y, *(str++), color, bg);
    }
}

void _tft_printbigs(short x, short y, const char *str, short w = 8, short h = 16, uint16_t color = ST7735_BLACK, uint16_t bg = ST7735_WHITE) {
    int counter = 0;
    while (*str != '\0') {
        _tft_printbigc(x + w*(counter++), y, *(str++), w, h, color, bg);
    }
}

void tft_printbigs(short x, short y, const char *str, float scale = 1.0, uint16_t color = ST7735_BLACK, uint16_t bg = ST7735_WHITE) {
    int w = scale * 8;
    int h = scale * 16;
    _tft_printbigs(x, y, str, w, h, color, bg);
}

bool matrixStates[16] = {false};
typedef enum {
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_A,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_B,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_C,
    KEY_STAR,
    KEY_0,
    KEY_SHARP,
    KEY_D,
    NumMatrixButtons
} MatrixButtons;
int readMatrix() {
    int res = -1;
    for (int col = 0; col <= 3; col++) {
        digitalWrite(3-col, HIGH);
        _delay_ms(10);
        for (int row = 0; row <= 3; row++) {
            matrixStates[col + 4 * row] = digitalRead(7 - row);
            if (matrixStates[col + 4 * row]) res = col + 4 * row;
        }
        digitalWrite(3-col, LOW);
    }
    return res;
}

void setup(void) {
    Serial.begin(9600);
    tft.initR(INITR_144GREENTAB);  // Init ST7735R chip, green tab
    // tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(0);  // set display orientation
    pinMode(LED_BUILTIN, OUTPUT); // pull down

    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);

    // pinmode()
}

// AIN LOOP

char buff[20];
void loop() {
    // tft.fillScreen(ST77XX_RED);

    tft_prints(25, 30, "12345!bitach");
    // tft_printbigs(25, 60, "$123", 2.0);
    sprintf(buff, "KEY: %2d", readMatrix());
    tft_printbigs(0, 60, buff, 2.0);


    // tft.flush();
    // print_text(25, 30, "HELLO", 3, ST77XX_ORANGE);
    // print_text(20, 70, "WORLD!", 3, ST77XX_BLUE);
    // delay(5000);

    // tft.fillScreen(ST77XX_BLACK);
    // tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
    // tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
    // delay(5000);

    // tft.fillScreen(ST77XX_CYAN);
    // tft.drawRect(5, 5, 120, 120, ST77XX_RED);
    // tft.drawFastHLine(5, 60, 120, ST77XX_RED);
    // tft.drawFastVLine(60, 5, 120, ST77XX_RED);
    // delay(5000);
}

