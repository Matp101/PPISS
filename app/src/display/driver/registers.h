/*
 * GC9A01 register definitions, colours, and pin assignments.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "espressif/esp8266/eagle_soc.h"
#include "espressif/esp8266/ets_sys.h"
#include "espressif/esp8266/pin_mux_register.h"
#include "espressif/esp_common.h"
#include "esp_libc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------- Display geometry -------------------- */
#define GC9A01A_TFTWIDTH  240
#define GC9A01A_TFTHEIGHT 240

enum {
    TFT_W = GC9A01A_TFTWIDTH,
    TFT_H = GC9A01A_TFTHEIGHT,
};

/* -------------------- Pin selection -------------------- */
enum {
    PIN_CS   = 15,  /* D8 */
    PIN_DC   = 0,   /* D3 */
    PIN_MOSI = 13,  /* D7 */
    PIN_SCLK = 14,  /* D5 */
};

/* -------------------- Register commands -------------------- */
#define GC9A01A_SWRESET 0x01
#define GC9A01A_RDDID 0x04
#define GC9A01A_RDDST 0x09
#define GC9A01A_SLPIN 0x10
#define GC9A01A_SLPOUT 0x11
#define GC9A01A_PTLON 0x12
#define GC9A01A_NORON 0x13
#define GC9A01A_INVOFF 0x20
#define GC9A01A_INVON 0x21
#define GC9A01A_DISPOFF 0x28
#define GC9A01A_DISPON 0x29
#define GC9A01A_CASET 0x2A
#define GC9A01A_RASET 0x2B
#define GC9A01A_RAMWR 0x2C
#define GC9A01A_PTLAR 0x30
#define GC9A01A_VSCRDEF 0x33
#define GC9A01A_TEOFF 0x34
#define GC9A01A_TEON 0x35
#define GC9A01A_MADCTL 0x36
#define GC9A01A_VSCRSADD 0x37
#define GC9A01A_IDLEOFF 0x38
#define GC9A01A_IDLEON 0x39
#define GC9A01A_COLMOD 0x3A
#define GC9A01A_CONTINUE 0x3C
#define GC9A01A_TEARSET 0x44
#define GC9A01A_GETLINE 0x45
#define GC9A01A_SETBRIGHT 0x51
#define GC9A01A_SETCTRL 0x53
#define GC9A01A1_POWER7 0xA7
#define GC9A01A_TEWC 0xBA
#define GC9A01A1_POWER1 0xC1
#define GC9A01A1_POWER2 0xC3
#define GC9A01A1_POWER3 0xC4
#define GC9A01A1_POWER4 0xC9
#define GC9A01A_RDID1 0xDA
#define GC9A01A_RDID2 0xDB
#define GC9A01A_RDID3 0xDC
#define GC9A01A_FRAMERATE 0xE8
#define GC9A01A_SPI2DATA 0xE9
#define GC9A01A_INREGEN2 0xEF
#define GC9A01A_GAMMA1 0xF0
#define GC9A01A_GAMMA2 0xF1
#define GC9A01A_GAMMA3 0xF2
#define GC9A01A_GAMMA4 0xF3
#define GC9A01A_IFACE 0xF6
#define GC9A01A_INREGEN1 0xFE

/* -------------------- MADCTL bits -------------------- */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

/* -------------------- Display rotation -------------------- */
/*
 * Set DISPLAY_ROTATION to 0, 90, 180, or 270.
 */
#ifndef DISPLAY_ROTATION
#define DISPLAY_ROTATION 0
#endif

#if   DISPLAY_ROTATION == 0
#define MADCTL_ROTATION (MADCTL_MX | MADCTL_BGR)
#elif DISPLAY_ROTATION == 90
#define MADCTL_ROTATION (MADCTL_MV | MADCTL_MX | MADCTL_BGR)
#elif DISPLAY_ROTATION == 180
#define MADCTL_ROTATION (MADCTL_MY | MADCTL_BGR)
#elif DISPLAY_ROTATION == 270
#define MADCTL_ROTATION (MADCTL_MV | MADCTL_MY | MADCTL_BGR)
#else
#error "DISPLAY_ROTATION must be 0, 90, 180, or 270"
#endif

/* -------------------- RGB565 colour constants -------------------- */
#define COLOR_BLACK       0x0000
#define COLOR_NAVY        0x000F
#define COLOR_DARKGREEN   0x03E0
#define COLOR_DARKCYAN    0x03EF
#define COLOR_MAROON      0x7800
#define COLOR_PURPLE      0x780F
#define COLOR_OLIVE       0x7BE0
#define COLOR_LIGHTGREY   0xC618
#define COLOR_DARKGREY    0x7BEF
#define COLOR_BLUE        0x001F
#define COLOR_GREEN       0x07E0
#define COLOR_CYAN        0x07FF
#define COLOR_RED         0xF800
#define COLOR_MAGENTA     0xF81F
#define COLOR_YELLOW      0xFFE0
#define COLOR_WHITE       0xFFFF
#define COLOR_ORANGE      0xFD20
#define COLOR_GREENYELLOW 0xAFE5
#define COLOR_PINK        0xFC18

#ifdef __cplusplus
}
#endif
