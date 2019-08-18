/* ==================== */
/* LCD Constants */
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C __lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
/* -- LCD Constants */
/* ==================== */

char __clearText[LCD_COLS];

LiquidCrystal_I2C lcd() {
  return __lcd;
}

void lcd_init() {
  for (uint8_t i = 0; i < LCD_COLS; i++) {
    __clearText[i] = ' ';
  }
  
  __lcd.clear();
  __lcd.backlight();
}

void lcd_clear() {
  __lcd.clear();
}

void lcd_print_top(String text, boolean clearBefore) {
  __lcd.setCursor(0, 0);
  if (clearBefore) {
    __lcd.print(__clearText);
  }
  __lcd.print(text);
}

void lcd_print_bottom(String text, boolean clearBefore) {
  __lcd.setCursor(0, 1);
  if (clearBefore) {
    __lcd.print(__clearText);
  }
  __lcd.print(text);
}
