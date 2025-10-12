#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "blink.h"

/* - -- Required C-linkage symbols for ESP8266 RTOS SDK --- */
extern "C" {

uint32_t ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
    return 1024 - 5; // 4MB flash => 1024 sectors, reserve last 5 for RF cal
}

void ICACHE_FLASH_ATTR user_init(void) {
    xTaskCreate(blink_task, (const signed char *)"blink", 512, nullptr, tskIDLE_PRIORITY + 1, nullptr);
}

}