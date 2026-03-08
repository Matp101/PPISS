#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Set by wifi_task — true when station has an IP */
extern volatile bool wifi_connected;

void wifi_task(void *arg);

#ifdef __cplusplus
}
#endif
