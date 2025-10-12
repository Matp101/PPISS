#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "blink/blink.h"
#include "wifi/wifi.h"
#include "gc9a01/gc9a01.h"

extern "C" {

uint32_t ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
    return 1024 - 5;
}

void ICACHE_FLASH_ATTR user_init(void) {
    xTaskCreate(blink_task, (const signed char *)"blink", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(wifi_task,  (const signed char *)"wifi",  1024, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(ui_task, (const signed char *)"ui", 1024, NULL, tskIDLE_PRIORITY + 3, NULL);
}

}
