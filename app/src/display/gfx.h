/*
 * Graphics primitives — text rendering, image drawing, fill.
 *
 * Uses the low-level SPI transport from driver/spi.h and the
 * 5×7 bitmap font.  All functions are static — included from
 * a single .c file only.
 */
#pragma once

#include "driver/spi.h"
#include "font.h"

/* -------------------- Fill screen -------------------- */
static void fill_screen(uint16_t color)
{
    set_addr_window(0, 0, TFT_W, TFT_H);

    cs_low();
    dc_high();
    for (uint32_t i = 0; i < (uint32_t)TFT_W * TFT_H; ++i) {
        spi_write_byte((uint8_t)(color >> 8));
        spi_write_byte((uint8_t)(color & 0xFF));
    }
    cs_high();
}

/* -------------------- Single character -------------------- */
/*
 * Draw character at (x, y) with scale factor.
 * Font is column-major: 5 bytes per char, each byte is one column,
 * bit 0 = top row.  Each font pixel becomes scale×scale display pixels.
 */
static void draw_char(uint16_t x, uint16_t y, char c, uint8_t scale,
                      uint16_t fg, uint16_t bg)
{
    const uint8_t *glyph = &glcdfont[(uint8_t)c * 5];

    for (uint8_t col = 0; col < 5; ++col) {
        uint8_t column = glyph[col];
        for (uint8_t row = 0; row < 7; ++row) {
            uint16_t color = (column & (1 << row)) ? fg : bg;
            set_addr_window(x + col * scale, y + row * scale, scale, scale);
            cs_low();
            dc_high();
            for (uint8_t sy = 0; sy < scale; ++sy) {
                for (uint8_t sx = 0; sx < scale; ++sx) {
                    spi_write_byte((uint8_t)(color >> 8));
                    spi_write_byte((uint8_t)(color & 0xFF));
                }
            }
            cs_high();
        }
    }
    /* 1-column gap (background) between characters */
    for (uint8_t row = 0; row < 7; ++row) {
        set_addr_window(x + 5 * scale, y + row * scale, scale, scale);
        cs_low();
        dc_high();
        for (uint8_t sy = 0; sy < scale; ++sy) {
            for (uint8_t sx = 0; sx < scale; ++sx) {
                spi_write_byte((uint8_t)(bg >> 8));
                spi_write_byte((uint8_t)(bg & 0xFF));
            }
        }
        cs_high();
    }
}

/* -------------------- String helpers -------------------- */

/* Draw a null-terminated string at (x, y). */
static void draw_string(uint16_t x, uint16_t y, const char *s,
                        uint8_t scale, uint16_t fg, uint16_t bg)
{
    uint16_t advance = 6 * scale;  /* 5 cols + 1 col gap */
    while (*s) {
        draw_char(x, y, *s, scale, fg, bg);
        x += advance;
        s++;
    }
}

/* Measure the pixel width of a string at the given scale. */
static uint16_t string_width(const char *s, uint8_t scale)
{
    int len = (int)strlen(s);
    if (len <= 0) return 0;
    return (uint16_t)(len * 6 * scale - scale);
}

/* Draw a string horizontally centred at vertical position y. */
static void draw_string_centered(uint16_t y, const char *s,
                                 uint8_t scale, uint16_t fg, uint16_t bg)
{
    uint16_t w = string_width(s, scale);
    uint16_t x = (TFT_W > w) ? (TFT_W - w) / 2 : 0;
    draw_string(x, y, s, scale, fg, bg);
}

/* -------------------- RGB565 image (centred) -------------------- */
static void draw_image_centered(const uint16_t *img, uint16_t w, uint16_t h)
{
    uint16_t x = (TFT_W > w) ? (TFT_W - w) / 2 : 0;
    uint16_t y = (TFT_H > h) ? (TFT_H - h) / 2 : 0;

    set_addr_window(x, y, w, h);
    cs_low();
    dc_high();
    uint32_t total = (uint32_t)w * h;
    for (uint32_t i = 0; i < total; ++i) {
        spi_write_byte((uint8_t)(img[i] >> 8));
        spi_write_byte((uint8_t)(img[i] & 0xFF));
    }
    cs_high();
}

/* -------------------- Indexed-colour image -------------------- */
/*
 * Draw a palette-indexed image at (x0, y0) with given scale.
 * Pixels with index 0 are treated as transparent (drawn as bg).
 */
static void draw_indexed_image(const uint8_t *pixels,
                               uint16_t w, uint16_t h,
                               const uint16_t *palette,
                               uint8_t scale, uint16_t bg,
                               uint16_t x0, uint16_t y0)
{
    for (uint16_t row = 0; row < h; ++row) {
        for (uint16_t col = 0; col < w; ++col) {
            uint8_t idx = pixels[row * w + col];
            uint16_t color = (idx == 0) ? bg : palette[idx];
            set_addr_window(x0 + col * scale, y0 + row * scale, scale, scale);
            cs_low();
            dc_high();
            for (uint8_t sy = 0; sy < scale; ++sy) {
                for (uint8_t sx = 0; sx < scale; ++sx) {
                    spi_write_byte((uint8_t)(color >> 8));
                    spi_write_byte((uint8_t)(color & 0xFF));
                }
            }
            cs_high();
        }
    }
}
