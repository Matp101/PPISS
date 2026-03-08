/*
 * Display module — public API.
 *
 * Exposes only the UI task entry point.
 * All display internals (driver, gfx, icons) are private.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UI task — call via xTaskCreate from user_init */
void ui_task(void *param);

#ifdef __cplusplus
}
#endif
