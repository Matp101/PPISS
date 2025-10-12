/*
 * GC9A01 240x240 round TFT — Minimal bring-up on ESP8266 RTOS-SDK (Wemos D1 mini)
 * Transport: bit-banged SPI (MODE0, MSB-first)
 *
 * Why bit-bang? It's bullet-proof for bring-up. Once this is solid on your wiring,
 * you can swap to HSPI for speed.
 *
 * Wiring (D1 mini):
 *   MOSI  -> D7  (GPIO13)
 *   SCLK  -> D5  (GPIO14)
 *   DC    -> D2  (GPIO4)
 *   CS    -> D8  (GPIO15)
 *   MISO  -> D6  (GPIO12)  [unused by display]
 *   BL    -> (often tied to VCC; optional PWM if your module exposes BL)
 *
 * Notes:
 *  - This uses the “Adafruit” GC9A01 init table (trimmed).
 *  - If your module’s backlight (BL) is permanently tied high, brightness
 *    control won’t work — that’s normal for many GC9A01 boards.
 */

#include "gc9a01.h"

#if !GC9A01_ENABLE_HSPI

#include "gc9a01_common.h"

/* -------------------- GC9A01 command bytes we use -------------------- */
typedef enum {
    GC9_SWRESET = 0x01,
    GC9_SLPOUT  = 0x11,
    GC9_DISPON  = 0x29,
    GC9_INVON   = 0x21,
    GC9_TEON    = 0x35,
    GC9_MADCTL  = 0x36,
    GC9_COLMOD  = 0x3A,
    GC9_CASET   = 0x2A,
    GC9_RASET   = 0x2B,
    GC9_RAMWR   = 0x2C
} Gc9Cmd;

/* MADCTL bits */
enum {
    MADCTL_MY  = 0x80,
    MADCTL_MX  = 0x40,
    MADCTL_MV  = 0x20,
    MADCTL_BGR = 0x08
};

/* -------------------- GPIO helpers -------------------- */
static inline void gpio_out_set(uint32_t pin) { GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1U << pin); }
static inline void gpio_out_clr(uint32_t pin) { GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1U << pin); }

static inline void cs_high(void)  { gpio_out_set(PIN_CS);   }
static inline void cs_low(void)   { gpio_out_clr(PIN_CS);   }
static inline void dc_high(void)   { gpio_out_set(PIN_DC);   }
static inline void dc_low(void)    { gpio_out_clr(PIN_DC);   }
static inline void mosi_high(void) { gpio_out_set(PIN_MOSI); }
static inline void mosi_low(void)  { gpio_out_clr(PIN_MOSI); }
static inline void clk_high(void)  { gpio_out_set(PIN_SCLK); }
static inline void clk_low(void)   { gpio_out_clr(PIN_SCLK); }

/* Small delay to respect setup/hold on edges (tweak if needed) */
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

static void spi_write_bytes(const uint8_t *buf, uint32_t len)
{
    while (len--) spi_write_byte(*buf++);
}

/* -------------------- Command/Data wrappers -------------------- */
static void wr_cmd(uint8_t c)
{
    cs_low();
    dc_low();                 // Command
    spi_write_byte(c);
    cs_high();
}

static void wr_dat8(uint8_t d)
{
    cs_low();
    dc_high();                // Data
    spi_write_byte(d);
    cs_high();
}

static void wr_dat16(uint16_t v_be)
{
    // GC9 expects 16-bit pixels MSB-first (big-endian)
    cs_low();
    dc_high();
    spi_write_byte((uint8_t)(v_be >> 8));
    spi_write_byte((uint8_t)(v_be & 0xFF));
    cs_high();
}

/* -------------------- Address window + solid fill -------------------- */
static void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    const uint16_t x2 = x + w - 1;
    const uint16_t y2 = y + h - 1;

    wr_cmd(GC9_CASET);  // Column range
    wr_dat16(x);
    wr_dat16(x2);

    wr_cmd(GC9_RASET);  // Row range
    wr_dat16(y);
    wr_dat16(y2);

    wr_cmd(GC9_RAMWR);  // Write to RAM
}

static void fill_screen(Color565 color)
{
    set_addr_window(0, 0, TFT_W, TFT_H);

    // Stream all pixels in one go, CS low while sending
    cs_low();
    dc_high();

    for (uint32_t i = 0; i < (uint32_t)TFT_W * TFT_H; ++i) {
        spi_write_byte((uint8_t)(color >> 8));
        spi_write_byte((uint8_t)(color & 0xFF));
    }

    cs_high();
}

/* -------------------- GC9A01 init (trimmed Adafruit sequence) -------------------- */
static void gc9a01_init(void)
{
    LOG("GC9A01 init");

    // Software reset (if no dedicated RST pin)
    wr_cmd(GC9_SWRESET);
    ets_delay_us(150000);

    // Same order/values Adafruit uses (some vendor-private regs are undocumented)
    const uint8_t seq[] = {
        0xEF, 0,   0xEB, 1, 0x14, 0xFE, 0,   0xEF, 0,   0xEB, 1, 0x14,
        0x84,1,0x40, 0x85,1,0xFF, 0x86,1,0xFF, 0x87,1,0xFF,
        0x88,1,0x0A, 0x89,1,0x21, 0x8A,1,0x00, 0x8B,1,0x80, 0x8C,1,0x01,
        0x8D,1,0x01, 0x8E,1,0xFF, 0x8F,1,0xFF,
        0xB6,2,0x00,0x00,
        GC9_MADCTL,1,(MADCTL_MX | MADCTL_BGR),   // Landscape-ish, BGR order
        GC9_COLMOD,1,0x05,                       // 16bpp (RGB565)
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
        GC9_TEON,0,
        GC9_INVON,0
    };

    const uint8_t *p = seq;
    while (p < seq + sizeof(seq)) {
        uint8_t cmd = *p++;
        uint8_t n   = *p++;
        wr_cmd(cmd);
        for (uint8_t i = 0; i < n; ++i) wr_dat8(*p++);
    }

    wr_cmd(GC9_SLPOUT);                 // Exit sleep
    vTaskDelay(150 / portTICK_RATE_MS); // datasheet-ish delay
    wr_cmd(GC9_DISPON);                 // Display on
    vTaskDelay(20 / portTICK_RATE_MS);
}

/* -------------------- GPIO & pin mux setup -------------------- */
static void gpio_init(void)
{
    // Route pins to GPIO function (the MT* pins default to JTAG/SPI functions)
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15); // CS
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13); // MOSI
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14); // SCLK
    // DC (GPIO4) is already a GPIO

    // Enable as outputs: CS, DC, MOSI, SCLK
    GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS,
                   (1U << PIN_CS) | (1U << PIN_DC) | (1U << PIN_MOSI) | (1U << PIN_SCLK));

    // Idle levels (MODE0): CS=H, CLK=L, MOSI=L, DC=H
    cs_high();
    clk_low();
    mosi_low();
    dc_high();
}

/* -------------------- UI task entry -------------------- */
void ui_task(void *param)
{
    (void)param;
    LOG("ui_task start (bit-banged SPI)");

    gpio_init();
    gc9a01_init();

    // Simple smoke loop: solid RED -> GREEN -> BLUE
    for (;;) {
        LOG("fill RED");
        fill_screen(COLOR_RED);
        vTaskDelay(600 / portTICK_RATE_MS);

        LOG("fill GREEN");
        fill_screen(COLOR_GREEN);
        vTaskDelay(600 / portTICK_RATE_MS);

        LOG("fill BLUE");
        fill_screen(COLOR_BLUE);
        vTaskDelay(600 / portTICK_RATE_MS);
    }
}

#endif // !GC9A01_ENABLE_HSPI