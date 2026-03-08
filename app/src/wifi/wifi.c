/*
 * WiFi station mode — connects to the configured AP and maintains
 * the wifi_connected flag for use by other tasks.
 */

#include "wifi.h"
#include "../secrets.h"

#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "espressif/c_types.h"
#include "espressif/esp_system.h"
#include "espressif/esp_wifi.h"
#include "espressif/esp_sta.h"

#include <string.h>
#include <stdio.h>

#ifndef WIFI_SSID
#define WIFI_SSID "YourSSID"
#endif

#ifndef WIFI_PASS
#define WIFI_PASS "YourPassword"
#endif

#define LOG(fmt, ...) os_printf("[WIFI] " fmt "\r\n", ##__VA_ARGS__)

volatile bool wifi_connected = false;

static void wifi_start_station(void)
{
    struct station_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    strncpy((char *)cfg.ssid, WIFI_SSID, sizeof(cfg.ssid) - 1);
    strncpy((char *)cfg.password, WIFI_PASS, sizeof(cfg.password) - 1);
    cfg.bssid_set = 0;

    wifi_set_opmode_current(STATION_MODE);
    wifi_station_set_auto_connect(1);
    wifi_station_set_config_current(&cfg);
    wifi_station_connect();
}

void wifi_task(void *arg)
{
    (void)arg;

    wifi_start_station();

    int printed_connected = 0;
    int printed_ip = 0;

    for (;;) {
        STATION_STATUS st = wifi_station_get_connect_status();

        if (st == STATION_GOT_IP) {
            wifi_connected = true;
            if (!printed_connected) {
                struct station_config sc;
                wifi_station_get_config(&sc);
                LOG("Connected to SSID: %s", sc.ssid);
                printed_connected = 1;
            }
            if (!printed_ip) {
                struct ip_info info;
                wifi_get_ip_info(STATION_IF, &info);
                uint32_t a = info.ip.addr;
                LOG("IP %u.%u.%u.%u",
                    (a >> 0) & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF);
                printed_ip = 1;
            }
        } else if (st == STATION_WRONG_PASSWORD ||
                   st == STATION_NO_AP_FOUND  ||
                   st == STATION_CONNECT_FAIL) {
            wifi_connected = false;
            LOG("Status %d, retrying...", (int)st);
            wifi_station_disconnect();
            vTaskDelay(1000 / portTICK_RATE_MS);
            wifi_station_connect();
            printed_connected = 0;
            printed_ip = 0;
        } else if (st == STATION_IDLE || st == STATION_CONNECTING) {
            /* still working */
        } else {
            /* any other state -> try to reconnect */
            wifi_connected = false;
            wifi_station_connect();
            printed_connected = 0;
            printed_ip = 0;
        }

        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
