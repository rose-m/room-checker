#include <LiquidCrystal_I2C.h>

#ifndef LCD_H_
#define LCD_H_

/* ==================== */
/* LCD Constants */
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
/* -- LCD Constants */
/* ==================== */

void lcd_init();

void lcd_clear();

void lcd_print_top(String text, bool clearBefore = true);

void lcd_print_bottom(String text, bool clearBefore = true);

#endif
