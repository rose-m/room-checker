#include <LiquidCrystal_I2C.h>

#ifndef LCD_H_
#define LCD_H_

void lcd_print_top(String text, boolean clearBefore = true);

void lcd_print_bottom(String text, boolean clearBefore = true);

#endif
