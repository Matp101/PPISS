#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

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
   BL   -> often tied to VCC on these modules; brightness control not used here
   ================================================================== */

/* UI task entry */
void ui_task(void *param);

#define GC9A01_ENABLE_HSPI 1

#ifdef __cplusplus
}
#endif
