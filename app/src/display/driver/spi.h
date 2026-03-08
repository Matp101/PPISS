/*
 * Bit-banged SPI transport for GC9A01.
 *
 * MODE0, MSB-first.  All functions are static — included from
 * gc9a01.c only (single translation-unit build).
 */
#pragma once

#include "registers.h"

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

/* -------------------- SPI byte write -------------------- */
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

/* -------------------- Command / Data wrappers -------------------- */
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
