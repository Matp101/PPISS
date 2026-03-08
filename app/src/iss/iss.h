/*
 * ISS Live Telemetry — shared types and data.
 *
 * To add a new telemetry item just append to g_telem[] in iss.c.
 * TELEM_COUNT is computed automatically from the array size.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* One telemetry item displayed on one UI page */
typedef struct {
    const char     *id;         /* Lightstreamer item ID, e.g. "NODE3000005" */
    const char     *label;      /* Page title, e.g. "Poo"                   */
    const char     *label2;     /* Optional second line (NULL = none)        */
    char            value[24];  /* Latest value string from server           */
    bool            valid;      /* true after first update received          */

    /* Optional RGB565 image (NULL = unused) */
    const uint16_t *image;
    uint16_t        img_w;
    uint16_t        img_h;

    /* Optional indexed-colour image (NULL = unused) */
    const uint8_t  *idx_img;
    const uint16_t *idx_pal;
    uint16_t        idx_w;
    uint16_t        idx_h;
    uint8_t         idx_scale;
} telem_item_t;

/* Telemetry items array — defined in iss.c */
extern volatile telem_item_t g_telem[];
extern const int TELEM_COUNT;

/* ISS telemetry polling task — call via xTaskCreate from user_init */
void iss_task(void *arg);

#ifdef __cplusplus
}
#endif
