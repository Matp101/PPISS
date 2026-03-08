/*
 * GC9A01 240x240 round TFT driver — ESP8266 RTOS-SDK (Wemos D1 mini)
 * Transport: bit-banged SPI (MODE0, MSB-first)
 *
 * Wiring (D1 mini):
 *   MOSI  -> D7  (GPIO13)
 *   SCLK  -> D5  (GPIO14)
 *   DC    -> D2  (GPIO4)
 *   CS    -> D8  (GPIO15)
 *   BL    -> tied to VCC (or optional PWM)
 */

#include "gc9a01.h"
#include "gc9a01_common.h"

#define LOG(fmt, ...) os_printf("[GC9A01] " fmt "\r\n", ##__VA_ARGS__)

/* -------------------- GPIO helpers -------------------- */
static inline void gpio_out_set(uint32_t pin) { GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1U << pin); }
static inline void gpio_out_clr(uint32_t pin) { GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1U << pin); }

static inline void cs_high(void)   { gpio_out_set(PIN_CS);   }
static inline void cs_low(void)    { gpio_out_clr(PIN_CS);   }
static inline void dc_high(void)   { gpio_out_set(PIN_DC);   }
static inline void dc_low(void)    { gpio_out_clr(PIN_DC);   }
static inline void mosi_high(void) { gpio_out_set(PIN_MOSI); }
static inline void mosi_low(void)  { gpio_out_clr(PIN_MOSI); }
static inline void clk_high(void)  { gpio_out_set(PIN_SCLK); }
static inline void clk_low(void)   { gpio_out_clr(PIN_SCLK); }

/* Small delay to respect setup/hold on edges */
static inline void spi_delay(void) { __asm__ __volatile__("nop;nop;nop;nop"); }

/* -------------------- Bit-banged SPI (MODE0, MSB-first) -------------------- */
static void spi_write_byte(uint8_t b)
{
    for (int i = 7; i >= 0; --i) {
        if (b & (1 << i)) mosi_high(); else mosi_low();
        spi_delay();
        clk_high();
        spi_delay();
        clk_low();
    }
}

/* -------------------- Command/Data wrappers -------------------- */
static void wr_cmd(uint8_t c)
{
    cs_low();
    dc_low();
    spi_write_byte(c);
    cs_high();
}

static void wr_dat8(uint8_t d)
{
    cs_low();
    dc_high();
    spi_write_byte(d);
    cs_high();
}

static void wr_dat16(uint16_t v)
{
    cs_low();
    dc_high();
    spi_write_byte((uint8_t)(v >> 8));
    spi_write_byte((uint8_t)(v & 0xFF));
    cs_high();
}

/* -------------------- Address window -------------------- */
static void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    const uint16_t x2 = x + w - 1;
    const uint16_t y2 = y + h - 1;

    wr_cmd(GC9A01A_CASET);
    wr_dat16(x);
    wr_dat16(x2);

    wr_cmd(GC9A01A_RASET);
    wr_dat16(y);
    wr_dat16(y2);

    wr_cmd(GC9A01A_RAMWR);
}

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

/* -------------------- GC9A01 init (Adafruit sequence) -------------------- */
static void gc9a01_init(void)
{
    LOG("GC9A01 init");

    wr_cmd(GC9A01A_SWRESET);
    ets_delay_us(150000);

    const uint8_t seq[] = {
        0xEF, 0,   0xEB, 1, 0x14, 0xFE, 0,   0xEF, 0,   0xEB, 1, 0x14,
        0x84,1,0x40, 0x85,1,0xFF, 0x86,1,0xFF, 0x87,1,0xFF,
        0x88,1,0x0A, 0x89,1,0x21, 0x8A,1,0x00, 0x8B,1,0x80, 0x8C,1,0x01,
        0x8D,1,0x01, 0x8E,1,0xFF, 0x8F,1,0xFF,
        0xB6,2,0x00,0x00,
        GC9A01A_MADCTL,1,(MADCTL_MX | MADCTL_BGR),
        GC9A01A_COLMOD,1,0x05,
        0x90,4,0x08,0x08,0x08,0x08,
        0xBD,1,0x06, 0xBC,1,0x00, 0xFF,3,0x60,0x01,0x04,
        0xC3,1,0x13, 0xC4,1,0x13, 0xC9,1,0x22, 0xBE,1,0x11,
        0xE1,2,0x10,0x0E, 0xDF,3,0x21,0x0C,0x02,
        0xF0,6,0x45,0x09,0x08,0x08,0x26,0x2A,
        0xF1,6,0x43,0x70,0x72,0x36,0x37,0x6F,
        0xF2,6,0x45,0x09,0x08,0x08,0x26,0x2A,
        0xF3,6,0x43,0x70,0x72,0x36,0x37,0x6F,
        0xED,2,0x1B,0x0B, 0xAE,1,0x77, 0xCD,1,0x63,
        0xE8,1,0x34,
        0x62,12,0x18,0x0D,0x71,0xED,0x70,0x70,0x18,0x0F,0x71,0xEF,0x70,0x70,
        0x63,12,0x18,0x11,0x71,0xF1,0x70,0x70,0x18,0x13,0x71,0xF3,0x70,0x70,
        0x64,7,0x28,0x29,0xF1,0x01,0xF1,0x00,0x07,
        0x66,10,0x3C,0x00,0xCD,0x67,0x45,0x45,0x10,0x00,0x00,0x00,
        0x67,10,0x00,0x3C,0x00,0x00,0x00,0x01,0x54,0x10,0x32,0x98,
        0x74,7,0x10,0x85,0x80,0x00,0x00,0x4E,0x00,
        0x98,2,0x3E,0x07,
        GC9A01A_TEON,0,
        GC9A01A_INVON,0
    };

    const uint8_t *p = seq;
    while (p < seq + sizeof(seq)) {
        uint8_t cmd = *p++;
        uint8_t n   = *p++;
        wr_cmd(cmd);
        for (uint8_t i = 0; i < n; ++i) wr_dat8(*p++);
    }

    wr_cmd(GC9A01A_SLPOUT);
    vTaskDelay(150 / portTICK_RATE_MS);
    wr_cmd(GC9A01A_DISPON);
    vTaskDelay(20 / portTICK_RATE_MS);

    LOG("GC9A01 init done");
}

/* -------------------- GPIO & pin mux setup -------------------- */
static void gpio_init(void)
{
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15); // CS
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13); // MOSI
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14); // SCLK

    GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS,
                   (1U << PIN_CS) | (1U << PIN_DC) | (1U << PIN_MOSI) | (1U << PIN_SCLK));

    cs_high();
    clk_low();
    mosi_low();
    dc_high();
}

/* -------------------- 5x7 bitmap font for digits 0-9 -------------------- */
static const uint8_t font5x7_digits[10][7] = {
    { 0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E }, // 0
    { 0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E }, // 1
    { 0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F }, // 2
    { 0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E }, // 3
    { 0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02 }, // 4
    { 0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E }, // 5
    { 0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E }, // 6
    { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08 }, // 7
    { 0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E }, // 8
    { 0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C }, // 9
};

/* Draw a single digit at (x,y) with scale factor.
   Each font pixel becomes scale x scale display pixels. */
static void draw_digit(uint16_t x, uint16_t y, uint8_t digit, uint8_t scale,
                        uint16_t fg, uint16_t bg)
{
    if (digit > 9) return;
    const uint8_t *glyph = font5x7_digits[digit];

    for (uint8_t row = 0; row < 7; ++row) {
        for (uint8_t col = 0; col < 5; ++col) {
            uint16_t color = (glyph[row] & (0x10 >> col)) ? fg : bg;
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
}

/* Draw an integer centred on the 240x240 display. */
static void draw_number(int32_t num, uint8_t scale, uint16_t fg, uint16_t bg)
{
    char buf[12];
    int len = snprintf(buf, sizeof(buf), "%ld", (long)num);
    if (len <= 0) return;

    uint16_t char_w = 5 * scale;
    uint16_t gap    = scale;
    uint16_t total_w = (uint16_t)(len * char_w + (len - 1) * gap);
    uint16_t total_h = 7 * scale;

    uint16_t x0 = (TFT_W > total_w) ? (TFT_W - total_w) / 2 : 0;
    uint16_t y0 = (TFT_H > total_h) ? (TFT_H - total_h) / 2 : 0;

    for (int i = 0; i < len; ++i) {
        char c = buf[i];
        if (c >= '0' && c <= '9') {
            draw_digit(x0, y0, (uint8_t)(c - '0'), scale, fg, bg);
        }
        x0 += char_w + gap;
    }
}

/* -------------------- UI task entry -------------------- */
void ui_task(void *param)
{
    (void)param;
    LOG("ui_task start");

    gpio_init();
    gc9a01_init();

    LOG("fill_screen BLACK");
    fill_screen(GC9A01A_BLACK);

    LOG("draw_number 99");
    draw_number(99, 8, GC9A01A_WHITE, GC9A01A_BLACK);
    LOG("draw_number done");

    for (;;) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
