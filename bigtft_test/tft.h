#include <Adafruit_GFX.h>
#include <ILI9486_SPI.h>
#include <Adafruit_ST7735.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef enum {ILI, ADA} BoardType;
void tft_printi(Adafruit_GFX& tft, BoardType board, short x, short y, short w, short h, const uint8_t *img, uint8_t bits = 16, uint16_t fore = BLACK, uint16_t bg = WHITE) { // assumes that w is divisible by 8
    tft.startWrite();
    uint16_t *colorArray = new uint16_t[w];

    if (board == ILI) {
        ILI9486_SPI& ili = *static_cast<ILI9486_SPI*>(&tft);
        ili.setWindow(x, y, w, h);
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                if (bits == 1) colorArray[c] = ((pgm_read_byte_near(img + (c/8) + r * (w/8)) >> (7- c%8)) & 0x01) ? fore : bg; 
                else if (bits == 16) colorArray[c] = pgm_read_word_near(img + c + w * r);
            }
            ili.pushColors(colorArray, w);
        }
    } else if (board == ADA) {
        Adafruit_ST7735& ada = *static_cast<Adafruit_ST7735*>(&tft);
        ada.setAddrWindow(x, y, w, h);
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                if (bits == 1) colorArray[c] = ((pgm_read_byte_near(img + (c/8) + r * (w/8)) >> (7- c%8)) & 0x01) ? fore : bg; 
                else if (bits == 16) colorArray[c] = pgm_read_word_near(img + c + w * r);
            }
            ada.writePixels(colorArray, w);
        }
    } else {
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                if (bits == 1) colorArray[c] = ((pgm_read_byte_near(img + (c/8) + r * (w/8)) >> (7- c%8)) & 0x01) ? fore : bg; 
                else if (bits == 16) colorArray[c] = pgm_read_word_near(img + c + w * r);
            }
        }
    }

    delete colorArray;
    tft.endWrite();
}

void tft_printc(Adafruit_GFX& tft, BoardType board, short x, short y, const char chr, uint16_t color = BLACK, uint16_t bg = WHITE) {
    static uint16_t colorArray[8 * 16];
    int counter = 0;
    for (int r = 0; r < 16; r++) {
        uint8_t temp = pgm_read_word_near(asciiBitmap[chr - ' '] + r);
        for (int c = 0; c < 8; c++) {
            colorArray[counter++] = (temp & 0x01) ? color : bg;
            temp >>= 1;
        }
    }
    int w = 8;
    int h = 16;
    tft.startWrite();
    if (board == ILI) {
        ILI9486_SPI& ili = *static_cast<ILI9486_SPI*>(&tft);
        ili.setWindow(x, y, w, h);
        ili.pushColors(colorArray, w * h);
    } else if (board == ADA) {
        Adafruit_ST7735& ada = *static_cast<Adafruit_ST7735*>(&tft);
        ada.setAddrWindow(x, y, w, h);
        ada.writePixels(colorArray, w * h);
    } else {
        tft.drawRGBBitmap(x, y, colorArray, 8, 16);
    }
    tft.endWrite();
}

void _tft_printbigc(Adafruit_GFX& tft, BoardType board, short x, short y, const char chr, short w = 8, short h = 16, uint16_t color = BLACK, uint16_t bg = WHITE) {
    tft.startWrite();
    uint16_t *colorArray = new uint16_t[w];
    if (board == ILI) {
        ILI9486_SPI& ili = *static_cast<ILI9486_SPI*>(&tft);
        ili.setWindow(x, y, w, h);
        for (int r = 0; r < h; r++) {
            int rr = (r + 0.5) / h * 16;
            uint8_t temp = pgm_read_word_near(asciiBitmap[chr - ' '] + rr);
            for (int c = 0; c < w; c++) {
                int cc = (c + 0.5) / w * 8;
                colorArray[c] = ((temp >> (cc)) & 0x01) ? color : bg;
            }
            ili.pushColors(colorArray, w);  
        }
    } else if (board == ADA) {
        Adafruit_ST7735& ada = *static_cast<Adafruit_ST7735*>(&tft);
        ada.setAddrWindow(x, y, w, h);
        for (int r = 0; r < h; r++) {
            int rr = (r + 0.5) / h * 16;
            uint8_t temp = pgm_read_word_near(asciiBitmap[chr - ' '] + rr);
            for (int c = 0; c < w; c++) {
                int cc = (c + 0.5) / w * 8;
                colorArray[c] = ((temp >> (cc)) & 0x01) ? color : bg;
            }
            ada.writePixels(colorArray, w);
        }
    } else {
        for (int r = 0; r < h; r++) {
            int rr = (r + 0.5) / h * 16;
            uint8_t temp = pgm_read_word_near(asciiBitmap[chr - ' '] + rr);
            for (int c = 0; c < w; c++) {
                int cc = (c + 0.5) / w * 8;
                colorArray[c] = ((temp >> (cc)) & 0x01) ? color : bg;
            }
            tft.drawRGBBitmap(x, y+r, colorArray, w, 1);
        }
    }

    delete colorArray;
    tft.endWrite();
}

void tft_prints(Adafruit_GFX& tft, BoardType board,  short x, short y, const char *str, uint16_t color = BLACK, uint16_t bg = WHITE) {
    int counter = 0;
    while (*str != '\0') {
        tft_printc(tft, board, x + 8*(counter++), y, *(str++), color, bg);
    }
}

void _tft_printbigs(Adafruit_GFX& tft, BoardType board,  short x, short y, const char *str, short w = 8, short h = 16, uint16_t color = BLACK, uint16_t bg = WHITE) {
    int counter = 0;
    while (*str != '\0') {
        _tft_printbigc(tft, board, x + w*(counter++), y, *(str++), w, h, color, bg);
    }
}

void tft_printbigs(Adafruit_GFX& tft, BoardType board, short x, short y, const char *str, float scale = 1.0, uint16_t color = BLACK, uint16_t bg = WHITE) {
    int w = scale * 8;
    int h = scale * 16;
    _tft_printbigs(tft, board, x, y, str, w, h, color, bg);
}