#ifndef _SSD1306_H_
#define _SSD1306_H_

#include <stdint.h>
#include "hardware/i2c.h"

#define BLACK 0
#define WHITE 1

typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t pages;
    uint8_t *buffer;
    i2c_inst_t *i2c_inst;
    uint8_t address;
} ssd1306_t;

void ssd1306_init(ssd1306_t *display, uint8_t width, uint8_t height, i2c_inst_t *i2c_inst, uint8_t addr);
void ssd1306_clear(ssd1306_t *display);
void ssd1306_show(ssd1306_t *display);
void ssd1306_draw_pixel(ssd1306_t *display, uint8_t x, uint8_t y, uint8_t color);
void ssd1306_draw_rectangle(ssd1306_t *display, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void ssd1306_draw_filled_rectangle(ssd1306_t *display, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);

#endif
