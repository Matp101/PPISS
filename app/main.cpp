/*
 * PPISS — Pee Percentage of ISS
 *
 * ESP8266 (Wemos D1 mini) + GC9A01 240×240 round TFT.
 * Fetches real-time ISS waste-tank telemetry via Lightstreamer
 * and displays the values on screen.
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "blink/blink.h"
#include "wifi/wifi.h"
#include "display/display.h"
#include "iss/iss.h"

extern "C" {

uint32_t ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
    return 1024 - 5;
}

void ICACHE_FLASH_ATTR user_init(void) {
    xTaskCreate(blink_task, (const signed char *)"blink", 128,  NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(wifi_task,  (const signed char *)"wifi",  1024, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(iss_task,   (const signed char *)"iss",   4096, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(ui_task,    (const signed char *)"ui",    1024, NULL, tskIDLE_PRIORITY + 3, NULL);
}

}
