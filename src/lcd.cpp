#include "lcd.h"

LiquidCrystal_I2C __lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
char __clearText[LCD_COLS];

void lcd_init() {
  for (uint8_t i = 0; i < LCD_COLS; i++) {
    __clearText[i] = ' ';
  }
  
  __lcd.init();
  __lcd.clear();
  __lcd.backlight();
}

void lcd_clear() {
  __lcd.clear();
}

void lcd_print_top(String text, bool clearBefore) {
  __lcd.setCursor(0, 0);
  if (clearBefore) {
    __lcd.print(__clearText);
    __lcd.setCursor(0, 0);
  }
  __lcd.print(text);
}

void lcd_print_bottom(String text, bool clearBefore) {
  __lcd.setCursor(0, 1);
  if (clearBefore) {
    __lcd.print(__clearText);
    __lcd.setCursor(0, 1);
  }
  __lcd.print(text);
}
