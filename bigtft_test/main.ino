#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library
#include <ILI9486_SPI.h>
#include <Keypad.h>
#include <SPI.h>

#include "tft.h"

#define TFT_S tft_small, ADA
#define TFT_B tft_big, ILI

extern volatile unsigned long timer0_overflow_count;
unsigned long ticks() { return timer0_overflow_count; }
#define AlignCenter(len, scale) ((128 - (len)*8 * (scale)) / 2)
#define AlignCenterB(len, scale) ((480 - (len)*8 * (scale)) / 2)
#define AlignCenter2(width, screen_width) ((screen_width - width) / 2)

#define TFTB_CS PIN_A2
#define TFTS_CS PIN_A3
#define TFT_DC 9
#define TFT_RST 8  // Or set to -1 and connect to Arduino RESET pin

Adafruit_ST7735 tft_small = Adafruit_ST7735(TFTS_CS, TFT_DC, TFT_RST);
ILI9486_SPI tft_big = ILI9486_SPI(TFTB_CS, PIN_A4, PIN_A5);
// TFT FUNCTIONS
typedef enum {
    INIT,
    IDLE_STATE,  // enter money
    SCAN_BRACELET,
    SCAN_FACE,
    PAYMENT_SUCCESS,
    CANCELLED,
} States;
States state = INIT;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                             {'4', '5', '6', 'B'},
                             {'7', '8', '9', 'C'},
                             {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, PIN_A0, PIN_A1};

Keypad customKeypad =
    Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
String amount = "";

bool tft_update_flag = false;

void eventListener(char c) {
    if (customKeypad.getState() == PRESSED) {
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '*':
                if (state == IDLE_STATE) {
                    if (amount.length() < 5) {
                        if (c != '*') {
                            amount += c;
                            tft_update_flag = true;
                        } else if (c == '*' && amount.indexOf('.') == -1) {
                            amount += '.';
                            tft_update_flag = true;
                        }
                    }
                }
                break;
            case '#':
                if (state == IDLE_STATE) {
                    if (amount.length() > 0) {
                        amount.remove(amount.length() - 1);
                        tft_update_flag = true;
                    }
                }
                break;
            case 'C':
                if (state == IDLE_STATE) {
                    amount = "";
                    tft_update_flag = true;
                } else {
                    state = CANCELLED;
                    tft_update_flag = true;
                }
                break;
            case 'D':
                if (state == IDLE_STATE) {
                    if (amount != "" || amount != ".") state = SCAN_BRACELET;
                    tft_small.fillScreen(WHITE);
                    tft_big.fillScreen(WHITE);

                    tft_update_flag = true;
                } else if (state == SCAN_BRACELET) {
                    tft_printbigs(TFT_S, 20, 50, "SUCCESS!", 1.5, ST7735_BLACK,
                                  ST7735_GREEN);
                    tft_printbigs(TFT_B, AlignCenterB(9, 2.0), 120, "SUCCESS!",
                                  2.0, ST7735_BLACK, ST7735_GREEN);
                    _delay_ms(500);
                    tft_small.fillScreen(ST77XX_WHITE);
                    tft_big.fillScreen(ST77XX_WHITE);
                    state = SCAN_FACE;
                    tft_update_flag = true;
                } else if (state == SCAN_FACE) {
                    tft_printbigs(TFT_S, 20, 50, "SUCCESS!", 1.5, ST7735_BLACK,
                                  ST7735_GREEN);
                    tft_printbigs(TFT_B, AlignCenterB(9, 2.0), 120, "SUCCESS!",
                                  2.0, ST7735_BLACK, ST7735_GREEN);
                    _delay_ms(500);
                    state = PAYMENT_SUCCESS;
                    tft_update_flag = true;
                }
                break;
        }
    }
}

void setup(void) {
    customKeypad.addEventListener(eventListener);
    customKeypad.setDebounceTime(1);
    Serial.begin(9600);
    tft_big.setSpiKludge(false);         // false to disable rpi_spi16_mode
    tft_small.initR(INITR_144GREENTAB);  // Init ST7735R chip, green tab
    tft_big.init();
    tft_small.setRotation(0);  // set display orientation
    tft_big.setRotation(1);    // set display orientation
    tft_big.fillScreen(ST77XX_WHITE);
    tft_small.fillScreen(ST77XX_WHITE);
    tft_update_flag = true;
}

char buff[20];

void loop() {
    customKeypad.getKey();

    if (state == INIT) {
        tft_prints(TFT_S, AlignCenter(10, 1), 0, "Welcome to", BLUE);
        tft_prints(TFT_S, AlignCenter(16, 1), 16, "Kinki's Veggies!", BLUE);
        tft_printi(TFT_S, AlignCenter2(96, 128),
                   32 + AlignCenter2(64, 128 - 32), 96, 64, tomato, 1, WHITE,
                   RED);
        tft_printbigs(TFT_B, 0, 0, "> Kinki's Veggies", 1.5, BLUE);
        tft_printbigs(TFT_B, 480 - 7 * 2.0 * 8, 0, "Payment", 2.0, BLACK);
        tft_printbigs(TFT_B, AlignCenterB(20, 2.0), 100,
                      "Enter payment amount:", 2.0, BLUE);
        state = IDLE_STATE;

    } else if (state == IDLE_STATE) {
        if (tft_update_flag) {
            sprintf(buff, "$ %5s", amount.c_str());
            tft_printbigs(TFT_B, AlignCenterB(20, 2.0), 132, buff, 5.0,
                          ST7735_RED);
            tft_update_flag = false;
        }

    } else if (state == SCAN_BRACELET) {
        if (tft_update_flag) {
            tft_printi(TFT_S, 16, 48, 96, 80, scan_bracelet, 1);
            tft_prints(TFT_S, AlignCenter(13, 1), 48, "Scan Bracelet");
            sprintf(buff, "$%s", amount.c_str());
            tft_printbigs(TFT_S, AlignCenter(strlen(buff), 2.5), 0, buff, 2.5,
                          ST7735_RED);

            tft_printi(TFT_B, AlignCenter2(96, 480), 200, 96, 96, scan_bracelet,
                       1);
            tft_printbigs(TFT_B, AlignCenterB(13, 2.0), 180, "Scan Bracelet", 2.0);
            sprintf(buff, "$%s", amount.c_str());
            tft_printbigs(TFT_B, AlignCenterB(strlen(buff), 5.0), 30, buff, 5.0,
                          ST7735_RED);
            tft_update_flag = false;
        }

    } else if (state == SCAN_FACE) {
        if (tft_update_flag) {
            tft_prints(TFT_S, 0, 0, "Place finger:", RED);
            tft_printi(TFT_S, AlignCenter2(75, 128), 16, 80, 96,
                       scan_fingerprint, 1);

            tft_printi(TFT_B, AlignCenter2(75, 480), 200, 80, 96,
                       scan_fingerprint, 1);
            tft_printbigs(TFT_B, AlignCenterB(16, 2.0), 180, "Scan Fingerprint",
                       2.0);
            sprintf(buff, "$%s", amount.c_str());
            tft_printbigs(TFT_B, AlignCenterB(strlen(buff), 5.0), 30, buff, 5.0,
                          ST7735_RED);
            tft_update_flag = false;
        }

    } else if (state == PAYMENT_SUCCESS) {
        amount = "";
        tft_printbigs(TFT_S, 0, 35, "Transaction", 1.5, ST7735_RED,
                      ST7735_GREEN);
        tft_printbigs(TFT_S, 0, 65, "Success!", 1.5, ST7735_RED, ST7735_GREEN);
        tft_printbigs(TFT_B, AlignCenterB(20, 2.5), 35, "Transaction Success!",
                      2.5, ST7735_RED, ST7735_GREEN);
        _delay_ms(2500);
        state = INIT;
        tft_small.fillScreen(ST77XX_WHITE);
        tft_big.fillScreen(ST77XX_WHITE);
        tft_update_flag = true;
    } else if (state == CANCELLED) {
        amount = "";
        tft_printbigs(TFT_S, 0, 35, "Transaction", 1.5, ST7735_RED,
                      ST7735_YELLOW);
        tft_printbigs(TFT_S, 0, 65, "Cancelled!", 1.5, ST7735_RED,
                      ST7735_YELLOW);
        tft_printbigs(TFT_B, AlignCenterB(22, 2.5), 35,
                      "Transaction Cancelled!", 2.5, ST7735_RED, ST7735_YELLOW);
        _delay_ms(1500);
        state = INIT;
        tft_small.fillScreen(ST77XX_WHITE);
        tft_big.fillScreen(ST77XX_WHITE);
        tft_update_flag = true;  N  
    }
}
