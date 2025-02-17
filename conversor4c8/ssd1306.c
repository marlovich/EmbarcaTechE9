#include "ssd1306.h"
#include <stdlib.h>
#include <string.h>

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

static void send_command(ssd1306_t *display, uint8_t command) {
    uint8_t buf[2] = {0x00, command};
    i2c_write_blocking(display->i2c_inst, display->address, buf, 2, false);
}

void ssd1306_init(ssd1306_t *display, uint8_t width, uint8_t height, i2c_inst_t *i2c_inst, uint8_t addr) {
    display->width = width;
    display->height = height;
    display->pages = height / 8;
    display->i2c_inst = i2c_inst;
    display->address = addr;
    display->buffer = malloc(width * display->pages);

    send_command(display, SSD1306_DISPLAYOFF);
    send_command(display, SSD1306_SETDISPLAYCLOCKDIV);
    send_command(display, 0x80);
    send_command(display, SSD1306_SETMULTIPLEX);
    send_command(display, height - 1);
    send_command(display, SSD1306_SETDISPLAYOFFSET);
    send_command(display, 0x00);
    send_command(display, SSD1306_SETSTARTLINE | 0x00);
    send_command(display, SSD1306_CHARGEPUMP);
    send_command(display, 0x14);
    send_command(display, SSD1306_MEMORYMODE);
    send_command(display, 0x00);
    send_command(display, SSD1306_SEGREMAP | 0x1);
    send_command(display, SSD1306_COMSCANDEC);
    send_command(display, SSD1306_SETCOMPINS);
    send_command(display, height == 64 ? 0x12 : 0x02);
    send_command(display, SSD1306_SETCONTRAST);
    send_command(display, height == 64 ? 0xCF : 0x8F);
    send_command(display, SSD1306_SETPRECHARGE);
    send_command(display, 0xF1);
    send_command(display, SSD1306_SETVCOMDETECT);
    send_command(display, 0x40);
    send_command(display, SSD1306_DISPLAYALLON_RESUME);
    send_command(display, SSD1306_NORMALDISPLAY);
    send_command(display, SSD1306_DISPLAYON);
}

// Implement other functions here...
