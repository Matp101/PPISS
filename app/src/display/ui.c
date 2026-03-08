/*
 * UI task — page-cycling telemetry display.
 *
 * Draws labels, images, values, and the WiFi status icon.
 * Each telemetry item gets its own page, cycling every
 * PAGE_CYCLE_MS milliseconds.
 */

#include "display.h"
#include "driver/gc9a01.h"
#include "gfx.h"
#include "icons/wifi.h"
#include "icons/tank.h"
#include "../iss/iss.h"
#include "../wifi/wifi.h"

#define LOG(fmt, ...) os_printf("[UI] " fmt "\r\n", ##__VA_ARGS__)

/* How long each page stays on screen (milliseconds) */
#define PAGE_CYCLE_MS   5000

void ui_task(void *param)
{
    (void)param;
    LOG("ui_task start");

    gc9a01_gpio_init();
    gc9a01_init();

    fill_screen(COLOR_BLACK);

    int  page           = -1;    /* current page index (-1 = none yet) */
    char prev_value[24] = "";    /* last value drawn on this page      */
    bool prev_wifi      = false; /* last WiFi state drawn              */

    const uint16_t poll_ms = 250;
    const uint16_t ticks_per_page = PAGE_CYCLE_MS / poll_ms;
    uint16_t tick = 0;

    for (;;) {
        /* ---- Page advance ---- */
        if (tick == 0 || tick >= ticks_per_page) {
            tick = 0;
            prev_value[0] = '\0';
            prev_wifi = !wifi_connected;

            if (TELEM_COUNT == 0) {
                vTaskDelay(poll_ms / portTICK_RATE_MS);
                continue;
            }
            page = (page + 1) % TELEM_COUNT;

            fill_screen(COLOR_BLACK);

            volatile telem_item_t *item = &g_telem[page];

            if (item->idx_img && item->idx_w > 0 && item->idx_h > 0) {
                /* ---- Indexed-colour image page ---- */
                uint16_t text_y = 15;
                draw_string_centered(text_y, (const char *)item->label, 2,
                                     COLOR_GREEN, COLOR_BLACK);
                text_y += 7 * 2 + 3;

                if (item->label2) {
                    draw_string_centered(text_y, (const char *)item->label2, 2,
                                         COLOR_GREEN, COLOR_BLACK);
                    text_y += 7 * 2 + 3;
                }

                uint16_t img_w_px = item->idx_w * item->idx_scale;
                uint16_t img_h_px = item->idx_h * item->idx_scale;
                uint16_t img_x = (TFT_W > img_w_px) ? (TFT_W - img_w_px) / 2 : 0;
                uint16_t img_y = text_y + 2;
                draw_indexed_image(item->idx_img,
                    item->idx_w, item->idx_h,
                    item->idx_pal, item->idx_scale, COLOR_BLACK,
                    img_x, img_y);

            } else if (item->image && item->img_w > 0 && item->img_h > 0) {
                /* ---- RGB565 image page ---- */
                draw_image_centered(item->image, item->img_w, item->img_h);

            } else {
                /* ---- Text-only page ---- */
                draw_string_centered(40, (const char *)item->label, 3,
                                     COLOR_GREEN, COLOR_BLACK);
                if (item->label2) {
                    draw_string_centered(40 + 7 * 3 + 4, (const char *)item->label2, 3,
                                         COLOR_GREEN, COLOR_BLACK);
                }
            }
        }

        /* ---- WiFi icon (only on state change) ---- */
        bool cur_wifi = wifi_connected;
        if (cur_wifi != prev_wifi) {
            prev_wifi = cur_wifi;
            draw_wifi_icon(cur_wifi ? COLOR_BLUE : COLOR_DARKGREY);
        }

        /* ---- Value update ---- */
        if (page >= 0 && page < TELEM_COUNT && g_telem[page].valid) {
            if (strcmp(prev_value, (const char *)g_telem[page].value) != 0) {
                strncpy(prev_value, (const char *)g_telem[page].value,
                        sizeof(prev_value) - 1);
                prev_value[sizeof(prev_value) - 1] = '\0';

                uint16_t val_y = 110;
                volatile telem_item_t *itm = &g_telem[page];
                if (itm->idx_img && itm->idx_w > 0 && itm->idx_h > 0) {
                    uint16_t ty = 15 + 7 * 2 + 3;
                    if (itm->label2) ty += 7 * 2 + 3;
                    uint16_t img_y = ty + 2;
                    uint16_t img_h_px = itm->idx_h * itm->idx_scale;
                    val_y = img_y + img_h_px + 12;
                } else if (itm->image && itm->img_w > 0 && itm->img_h > 0) {
                    uint16_t img_top = (TFT_H > itm->img_h) ? (TFT_H - itm->img_h) / 2 : 0;
                    val_y = img_top + itm->img_h + 4;
                }

                /* Clear value area */
                set_addr_window(0, val_y, TFT_W, 40);
                cs_low();
                dc_high();
                for (uint32_t px = 0; px < (uint32_t)TFT_W * 40; ++px) {
                    spi_write_byte(0);
                    spi_write_byte(0);
                }
                cs_high();

                draw_string_centered(val_y, prev_value, 4,
                                     COLOR_WHITE, COLOR_BLACK);
                LOG("page %d: %s = %s", page,
                    g_telem[page].label, prev_value);
            }
        }

        ++tick;
        vTaskDelay(poll_ms / portTICK_RATE_MS);
    }
}
