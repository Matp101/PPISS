#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_task(void *arg);

#ifdef __cplusplus
}
#endif
