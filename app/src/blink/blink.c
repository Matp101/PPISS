#include "blink.h"

#include "esp8266/pin_mux_register.h"
#include "gpio.h"


#define LED_GPIO 2

static inline void led_on()  { GPIO_OUTPUT_SET(LED_GPIO, 0); }
static inline void led_off() { GPIO_OUTPUT_SET(LED_GPIO, 1); }

static void led_init() {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    GPIO_AS_OUTPUT(1U << LED_GPIO);
    led_off();
}

void blink_task(void *arg) {
    (void)arg;

    led_init();

    for (;;) {
        led_on();
        vTaskDelay(250 / portTICK_RATE_MS);
        led_off();
        vTaskDelay(250 / portTICK_RATE_MS);
    }
}