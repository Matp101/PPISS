/*
 * WiFi status icon — 16×12 1-bit bitmap.
 *
 * Three arcs + centre dot.  Each row is 2 bytes (16 bits, MSB left).
 * Rendered at 2× scale → 32×24 px on screen.
 */
#pragma once

#include "../driver/spi.h"

#define WIFI_ICON_W  16
#define WIFI_ICON_H  12

static const uint8_t wifi_icon[WIFI_ICON_H * 2] = {
    0x03, 0xC0,   /* row  0:     oooooo       */
    0x0F, 0xF0,   /* row  1:   oooooooooo     */
    0x1E, 0x78,   /* row  2:  oooo    oooo    */
    0x38, 0x1C,   /* row  3: ooo        ooo   */
    0x03, 0xC0,   /* row  4:     oooooo       */
    0x0F, 0xF0,   /* row  5:   oooooooooo     */
    0x0C, 0x30,   /* row  6:   oo      oo     */
    0x00, 0x00,   /* row  7:                  */
    0x01, 0x80,   /* row  8:      oooo        */
    0x03, 0xC0,   /* row  9:     oooooo       */
    0x01, 0x80,   /* row 10:      oooo        */
    0x00, 0x00,   /* row 11:                  */
};

/*
 * Draw WiFi icon centred horizontally near the bottom of the screen.
 * fg = icon colour (e.g. blue when connected, dark grey when not).
 */
static void draw_wifi_icon(uint16_t fg)
{
    const uint8_t  scale = 2;
    const uint16_t iw = WIFI_ICON_W * scale;
    const uint16_t ih = WIFI_ICON_H * scale;
    const uint16_t x0 = (TFT_W - iw) / 2;
    const uint16_t y0 = TFT_H - ih - 8;    /* 8 px margin from bottom */
    const uint16_t bg = COLOR_BLACK;

    for (uint8_t row = 0; row < WIFI_ICON_H; ++row) {
        uint16_t bits = ((uint16_t)wifi_icon[row * 2] << 8) |
                         wifi_icon[row * 2 + 1];
        for (uint8_t col = 0; col < WIFI_ICON_W; ++col) {
            uint16_t color = (bits & (1U << (15 - col))) ? fg : bg;
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
