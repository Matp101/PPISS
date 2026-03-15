/*
 * WiFi station mode — connects to the configured AP and maintains
 * the wifi_connected flag for use by other tasks.
 *
 * Cycles through WIFI_CREDS[] (defined in secrets.h) until one
 * succeeds.  On failure or timeout, moves to the next SSID.
 */

#include "wifi.h"

#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "espressif/c_types.h"
#include "espressif/esp_system.h"
#include "espressif/esp_wifi.h"
#include "espressif/esp_sta.h"

#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Credentials                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *ssid;
    const char *pass;
} wifi_cred_t;

#include "../secrets.h"

#ifndef HAS_WIFI_CREDS
#warning "secrets.h is missing HAS_WIFI_CREDS — using default placeholder credentials. Copy secrets.h.example to secrets.h and fill in your WiFi credentials."
static const wifi_cred_t WIFI_CREDS[] = {
    { "YourSSID", "YourPassword" },
};
#define WIFI_CRED_COUNT (sizeof(WIFI_CREDS) / sizeof(WIFI_CREDS[0]))
#endif

/* ------------------------------------------------------------------ */
/*  Tunables                                                          */
/* ------------------------------------------------------------------ */

#define POLL_MS          250          /* main-loop polling interval    */
#define CONNECT_TIMEOUT  (5000 / POLL_MS)  /* 5 s in ticks            */

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */

#define LOG(fmt, ...) os_printf("[WIFI] " fmt "\r\n", ##__VA_ARGS__)

volatile bool wifi_connected = false;
static int    current_cred   = 0;

/* Configure and connect to the given credential index. */
static void wifi_start(int idx)
{
    wifi_station_disconnect();
    wifi_station_set_auto_connect(0);

    struct station_config cfg;
    memset(&cfg, 0, sizeof(cfg));
    strncpy((char *)cfg.ssid,     WIFI_CREDS[idx].ssid, sizeof(cfg.ssid)     - 1);
    strncpy((char *)cfg.password,  WIFI_CREDS[idx].pass, sizeof(cfg.password) - 1);
    cfg.bssid_set = 0;

    wifi_set_opmode_current(STATION_MODE);
    wifi_station_set_config_current(&cfg);
    wifi_station_connect();

    LOG("Trying SSID: %s (%d/%d)",
        WIFI_CREDS[idx].ssid, idx + 1, (int)WIFI_CRED_COUNT);
}

/* Advance to the next credential set and start connecting. */
static void wifi_next(void)
{
    current_cred = (current_cred + 1) % WIFI_CRED_COUNT;
    LOG("Switching to next SSID...");
    wifi_station_disconnect();
    vTaskDelay(POLL_MS / portTICK_RATE_MS);
    wifi_start(current_cred);
}

/* Log SSID and IP address on first successful connect. */
static void wifi_log_connection(void)
{
    struct station_config sc;
    wifi_station_get_config(&sc);
    LOG("Connected to SSID: %s", sc.ssid);

    struct ip_info info;
    wifi_get_ip_info(STATION_IF, &info);
    uint32_t a = info.ip.addr;
    LOG("IP %u.%u.%u.%u",
        (a >>  0) & 0xFF, (a >>  8) & 0xFF,
        (a >> 16) & 0xFF, (a >> 24) & 0xFF);
}

/* ------------------------------------------------------------------ */
/*  Task                                                              */
/* ------------------------------------------------------------------ */

void wifi_task(void *arg)
{
    (void)arg;

    int wait_ticks = 0;   /* ticks spent in CONNECTING / IDLE  */
    int logged     = 0;   /* set after printing connection info */

    current_cred = 0;
    wifi_start(current_cred);

    for (;;) {
        STATION_STATUS st = wifi_station_get_connect_status();

        switch (st) {

        case STATION_GOT_IP:
            wifi_connected = true;
            wait_ticks = 0;
            if (!logged) {
                wifi_log_connection();
                logged = 1;
            }
            break;

        case STATION_WRONG_PASSWORD:
        case STATION_NO_AP_FOUND:
        case STATION_CONNECT_FAIL:
            wifi_connected = false;
            wait_ticks = 0;
            logged = 0;
            LOG("Failed for \"%s\"", WIFI_CREDS[current_cred].ssid);
            wifi_next();
            break;

        case STATION_IDLE:
        case STATION_CONNECTING:
            if (++wait_ticks >= CONNECT_TIMEOUT) {
                LOG("Timeout for \"%s\"", WIFI_CREDS[current_cred].ssid);
                wait_ticks = 0;
                logged = 0;
                wifi_next();
            }
            break;

        default:
            wifi_connected = false;
            wait_ticks = 0;
            logged = 0;
            wifi_station_connect();
            break;
        }

        vTaskDelay(POLL_MS / portTICK_RATE_MS);
    }
}
