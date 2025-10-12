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

/* ===================== Wiring (Wemos D1 mini) =====================
   CS   -> D8  (GPIO15) [we hold this LOW as a plain GPIO]
   DC   -> D2  (GPIO4)  [manual GPIO]
   MOSI -> D7  (GPIO13) [HSPI MOSI or GPIO (bit-bang)]
   SCLK -> D5  (GPIO14) [HSPI SCLK or GPIO (bit-bang)]
   MISO -> D6  (GPIO12) [unused by display]
   RST  -> NC  (we do software reset)
   ================================================================== */

#define GC9A01_ENABLE_HSPI 1

#define LOG(fmt, ...) os_printf("[GC9A01] " fmt "\r\n", ##__VA_ARGS__)

/* -------------------- Display geometry -------------------- */
enum { TFT_W = 240, TFT_H = 240 };

/* -------------------- Pin selection (D1 mini) -------------------- */
enum {
    PIN_CS   = 15,  // D8  - Chip Select
    PIN_DC   = 4,   // D2  - Data/Command
    PIN_MOSI = 13,  // D7  - SPI MOSI
    PIN_SCLK = 14,  // D5  - SPI CLK

};

/* -------------------- 565 color helpers -------------------- */
typedef enum {
    COLOR_BLACK = 0x0000,
    COLOR_RED   = 0xF800,
    COLOR_GREEN = 0x07E0,
    COLOR_BLUE  = 0x001F,
    COLOR_WHITE = 0xFFFF
} Color565;

#ifdef __cplusplus
}
#endif
