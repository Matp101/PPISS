#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "blink/blink.h"
#include "wifi/wifi.h"

extern "C" {

uint32_t ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
    return 1024 - 5;
}

void ICACHE_FLASH_ATTR user_init(void) {
    xTaskCreate(blink_task, (const signed char *)"blink", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(wifi_task,  (const signed char *)"wifi",  1024, NULL, tskIDLE_PRIORITY + 2, NULL);
}

}
