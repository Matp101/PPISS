#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===================== Wiring (Wemos D1 mini) =====================
   CS   -> D8  (GPIO15)
   DC   -> D2  (GPIO4)
   MOSI -> D7  (GPIO13)
   SCLK -> D5  (GPIO14)
   MISO -> D6  (GPIO12)
   RST  -> NC  (software reset)
   BL   -> VCC
   ================================================================== */

/* UI task entry */
void ui_task(void *param);

#ifdef __cplusplus
}
#endif
