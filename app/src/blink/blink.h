#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

void blink_task(void *arg);

#ifdef __cplusplus
}
#endif
