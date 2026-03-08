/*
 * GC9A01 display driver — public API.
 *
 * Wiring (Wemos D1 mini):
 *   CS   -> D8  (GPIO15)
 *   DC   -> D2  (GPIO4)
 *   MOSI -> D7  (GPIO13)
 *   SCLK -> D5  (GPIO14)
 *   RST  -> NC  (software reset)
 *   BL   -> VCC
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialise GPIO pins for SPI */
void gc9a01_gpio_init(void);

/* Run the GC9A01 power-on init sequence */
void gc9a01_init(void);

#ifdef __cplusplus
}
#endif
