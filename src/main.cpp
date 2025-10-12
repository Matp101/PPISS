#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
#include "esp8266/pin_mux_register.h"   // PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2, PIN_FUNC_SELECT
#include "gpio.h"                       // GPIO_OUTPUT_SET, GPIO_AS_OUTPUT
}

#define LED_GPIO 2

static inline void led_on()  { GPIO_OUTPUT_SET(LED_GPIO, 0); } // active-low
static inline void led_off() { GPIO_OUTPUT_SET(LED_GPIO, 1); }

static void led_init() {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    GPIO_AS_OUTPUT(1U << LED_GPIO);
    led_off();
}

static void blink_task(void *arg) {
    (void)arg;
    for (;;) {
        led_on();
        vTaskDelay(250 / portTICK_RATE_MS);
        led_off();
        vTaskDelay(250 / portTICK_RATE_MS);
    }
}

/* - -- Required C-linkage symbols for ESP8266 RTOS SDK --- */
extern "C" {

uint32_t ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
    return 1024 - 5; // 4MB flash => 1024 sectors, reserve last 5 for RF cal
}

void ICACHE_FLASH_ATTR user_init(void) {
    led_init();
    xTaskCreate(blink_task, (const signed char *)"blink", 512, nullptr, tskIDLE_PRIORITY + 1, nullptr);
}

}