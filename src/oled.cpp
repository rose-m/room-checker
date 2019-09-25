
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "oled.h"

/* ==================== */
/* Constants */
#define OLED_ADDRESS 0x3c
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
/* -- Constants */
/* ==================== */

Adafruit_SSD1306 __oled = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire);

String __textTop = "";
String __textBottom = "";

void oled_init()
{
    __oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
    __oled.display();
    delay(1000);
    oled_clear();
}

void oled_clear()
{
    __oled.clearDisplay();
    __oled.display();
}

void __print_text()
{
    __oled.clearDisplay();
    __oled.setTextSize(1);
    __oled.setTextColor(WHITE);
    __oled.setCursor(0, 0);
    __oled.println(__textTop);
    __oled.setCursor(0, 15);
    __oled.println(__textBottom);
    __oled.display();
}

void oled_print_top(String text)
{
    __textTop = text;
    __print_text();
}

void oled_print_bottom(String text)
{
    __textBottom = text;
    __print_text();
}
