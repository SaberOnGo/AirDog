#ifndef TEST1_TEXT_H
#define TEST1_TEXT_H

#include "tft_graph.h"
#include "tft_glcdfont.h"

#define TRANSPARENT_COLOR CYAN

#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))

void LCD_write(uint8_t c);
void LCD_writeString(uint8_t *s);
void LCD_writeString2(uint8_t *string);

void LCD_setCursor(u16 x, u16 y);
void LCD_setTextSize(u8 size);
void LCD_setTextColor(u16 color);
void LCD_setTextBgColor(u16 color);

u16 LCD_getCursorX(void);
u16 LCD_getCursorY(void);

#endif //TEST1_TEXT_H

