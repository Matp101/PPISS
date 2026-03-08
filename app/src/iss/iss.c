/*
 * ISS Live Telemetry — Lightstreamer TLCP-2.5.0 client
 *
 * Connects to NASA's public Lightstreamer server (plain HTTP, port 80)
 * using HTTP polling mode.
 *
 * Flow:
 *   1. POST create_session.txt  → CONOK,<session_id>,...
 *   2. POST control.txt         → REQOK  (subscribe each item)
 *   3. POST bind_session.txt    → U,<sub>,<item>,<val>|<ts>
 *   4. Repeat step 3 to poll for updates.
 */

#include "iss.h"
#include "../display/icons/tank.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/err.h"

#include "espressif/esp_wifi.h"
#include "espressif/esp_sta.h"
#include "espressif/esp_common.h"
#include "esp_libc.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>

#define LOG(fmt, ...) os_printf("[ISS] " fmt "\r\n", ##__VA_ARGS__)

/* ---- Configuration ---- */
#define LS_HOST           "push.lightstreamer.com"
#define LS_PORT           80
#define LS_CID            "mgQkwtwdysogQz2BJ4Ji%20kOj2Bg"
#define POLL_INTERVAL_MS  1000
#define RX_BUF_SIZE       1024
#define SESSION_ID_LEN    48

/* ---- Telemetry items ----
 *  Add a new row here to subscribe to another ISS parameter.
 *  TELEM_COUNT is computed automatically from the array size.
 */
volatile telem_item_t g_telem[] = {
    {
        .id = "NODE3000005", .label = "Poo", .label2 = NULL,
        .value = "", .valid = false,
        .image = NULL, .img_w = 0, .img_h = 0,
        .idx_img = NULL, .idx_pal = NULL, .idx_w = 0, .idx_h = 0, .idx_scale = 0,
    },
    {
        .id = "NODE3000008", .label = "Urine", .label2 = "Tank",
        .value = "", .valid = false,
        .image = NULL, .img_w = 0, .img_h = 0,
        .idx_img = tank_pixels, .idx_pal = tank_palette,
        .idx_w = TANK_IMG_W, .idx_h = TANK_IMG_H, .idx_scale = 2,
    },
};
const int TELEM_COUNT = sizeof(g_telem) / sizeof(g_telem[0]);

/* ---- Resolve LS_HOST once ---- */
static uint32_t ls_ip = 0;

static int resolve_host(void)
{
    struct hostent *he = gethostbyname(LS_HOST);
    if (!he) { LOG("DNS failed"); return -1; }
    memcpy(&ls_ip, he->h_addr, 4);
    LOG("DNS: %s -> %u.%u.%u.%u", LS_HOST,
        (ls_ip>>0)&0xFF, (ls_ip>>8)&0xFF, (ls_ip>>16)&0xFF, (ls_ip>>24)&0xFF);
    return 0;
}

/* ---- Low-level: HTTP POST, return body in buf ---- */
static int http_post(const char *path, const char *post_body,
                     char *buf, int buf_size)
{
    int sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { LOG("socket fail"); return -1; }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LS_PORT);
    memcpy(&addr.sin_addr, &ls_ip, 4);

    if (lwip_connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOG("connect fail"); lwip_close(sock); return -1;
    }
    LOG("connected");

    /* Set receive timeout — blocking mode with timeout */
    int timeout_ms = 25000;
    lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                    &timeout_ms, sizeof(timeout_ms));

    /* Build HTTP POST request */
    char req[512];
    int body_len = (int)strlen(post_body);
    int rlen = snprintf(req, sizeof(req),
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        path, LS_HOST, body_len, post_body);

    int wr = lwip_write(sock, req, (size_t)rlen);
    if (wr < 0) { LOG("write fail errno=%d", errno); lwip_close(sock); return -1; }
    LOG("wrote %d bytes", wr);

    /* Read response — blocking reads until connection closes or timeout */
    int total = 0;
    int reads = 0;
    for (;;) {
        int space = buf_size - total - 1;
        if (space <= 0) break;
        int n = lwip_read(sock, buf + total, space);
        reads++;
        if (n > 0) {
            total += n;
            LOG("read[%d] +%d (total %d)", reads, n, total);
            if (total >= buf_size - 1) break;
        } else {
            LOG("read[%d] returned %d errno=%d", reads, n, errno);
            break;
        }
    }
    buf[total] = '\0';
    lwip_close(sock);

    LOG("total received: %d bytes", total);

    if (total <= 0) { LOG("empty resp"); return -1; }

    /* Skip HTTP headers → find body after \r\n\r\n or \n\n */
    char *body = strstr(buf, "\r\n\r\n");
    int hdr_skip = 4;
    if (!body) {
        body = strstr(buf, "\n\n");
        hdr_skip = 2;
    }
    if (!body) { LOG("no body sep"); return -1; }
    body += hdr_skip;

    int blen = total - (int)(body - buf);
    memmove(buf, body, (size_t)blen);
    buf[blen] = '\0';
    return blen;
}

/* ---- Create Lightstreamer session, return session ID ---- */
static int ls_create_session(char *sid, int sid_size)
{
    char body[256];
    snprintf(body, sizeof(body),
        "LS_polling=true&LS_polling_millis=0&LS_idle_millis=0"
        "&LS_adapter_set=ISSLIVE&LS_cid=%s", LS_CID);

    char rx[RX_BUF_SIZE];
    int len = http_post(
        "/lightstreamer/create_session.txt?LS_protocol=TLCP-2.5.0",
        body, rx, sizeof(rx));
    if (len <= 0) return -1;

    /* Parse CONOK,<session_id>,... */
    if (strncmp(rx, "CONOK,", 6) != 0) {
        LOG("create_session: %s", rx);
        return -1;
    }
    char *comma = strchr(rx + 6, ',');
    if (!comma) return -1;
    int slen = (int)(comma - (rx + 6));
    if (slen >= sid_size) slen = sid_size - 1;
    memcpy(sid, rx + 6, (size_t)slen);
    sid[slen] = '\0';
    LOG("session: %s", sid);
    return 0;
}

/* ---- Subscribe to one telemetry item ---- */
static int ls_subscribe(const char *sid, int req_id, int sub_id,
                        const char *item_name)
{
    char path[128];
    snprintf(path, sizeof(path),
        "/lightstreamer/control.txt?LS_protocol=TLCP-2.5.0&LS_session=%s",
        sid);

    char body[256];
    snprintf(body, sizeof(body),
        "LS_reqId=%d&LS_op=add&LS_subId=%d&LS_mode=MERGE"
        "&LS_group=%s&LS_schema=Value%%20TimeStamp&LS_snapshot=true",
        req_id, sub_id, item_name);

    char rx[RX_BUF_SIZE];
    int len = http_post(path, body, rx, sizeof(rx));
    if (len <= 0) return -1;

    if (strncmp(rx, "REQOK", 5) != 0) {
        LOG("subscribe %s: %s", item_name, rx);
        return -1;
    }
    LOG("subscribed: %s (sub %d)", item_name, sub_id);
    return 0;
}

/* ---- Poll for updates via bind_session ---- */
static int ls_poll(const char *sid)
{
    char body[128];
    snprintf(body, sizeof(body),
        "LS_session=%s&LS_polling=true&LS_polling_millis=0&LS_idle_millis=19000",
        sid);

    char rx[RX_BUF_SIZE];
    int len = http_post(
        "/lightstreamer/bind_session.txt?LS_protocol=TLCP-2.5.0",
        body, rx, sizeof(rx));
    if (len <= 0) { LOG("poll: empty/error"); return -1; }

    /* Check for session errors */
    if (strncmp(rx, "CONERR", 6) == 0) {
        LOG("poll error: %s", rx);
        return -2;  /* session lost */
    }

    /* Parse each line for U,<subId>,<itemPos>,<value>|<timestamp> */
    char *line = rx;
    while (line && *line) {
        char *next = strstr(line, "\n");
        if (next) { *next = '\0'; next++; }

        /* Trim \r */
        int llen = (int)strlen(line);
        if (llen > 0 && line[llen-1] == '\r') line[llen-1] = '\0';

        if (line[0] == 'U' && line[1] == ',') {
            /* U,<subId>,<itemPos>,<value>|<timestamp> */
            int sub_id = 0, item_pos = 0;
            char *p = line + 2;
            sub_id = atoi(p);
            p = strchr(p, ',');
            if (p) { p++; item_pos = atoi(p); }
            (void)item_pos;
            p = strchr(p, ',');
            if (p) {
                p++;  /* p now points to value|timestamp */
                /* Extract just the value (before |) */
                char *pipe = strchr(p, '|');
                if (pipe) *pipe = '\0';

                /* sub_id is 1-based, maps to g_telem[sub_id-1] */
                int idx = sub_id - 1;
                if (idx >= 0 && idx < TELEM_COUNT) {
                    /* Truncate value for display (e.g. "80.0500030517578125" → "80.05") */
                    char val_short[24];
                    /* Try to shorten floats to 1 decimal */
                    char *dot = strchr(p, '.');
                    if (dot && strlen(dot) > 2) {
                        int whole_len = (int)(dot - p);
                        snprintf(val_short, sizeof(val_short), "%.*s.%c",
                                 whole_len, p, dot[1]);
                    } else {
                        strncpy(val_short, p, sizeof(val_short) - 1);
                        val_short[sizeof(val_short) - 1] = '\0';
                    }

                    strncpy((char *)g_telem[idx].value, val_short,
                            sizeof(g_telem[idx].value) - 1);
                    g_telem[idx].valid = true;
                    LOG("%s = %s", g_telem[idx].id, val_short);
                }
            }
        }
        line = next;
    }
    return 0;
}

/* ---- Wait for WiFi ---- */
static void wait_for_ip(void)
{
    LOG("waiting for Wi-Fi IP...");
    for (;;) {
        STATION_STATUS st = wifi_station_get_connect_status();
        if (st == STATION_GOT_IP) break;
        vTaskDelay(500 / portTICK_RATE_MS);
    }
    LOG("Wi-Fi connected");
}

/* ---- Main telemetry task ---- */
void iss_task(void *arg)
{
    (void)arg;
    LOG("iss_task start");
    wait_for_ip();

    if (resolve_host() < 0) {
        LOG("DNS failed, retrying in 5s...");
        vTaskDelay(5000 / portTICK_RATE_MS);
    }

    for (;;) {
        /* Step 1: Create session */
        char sid[SESSION_ID_LEN];
        if (ls_create_session(sid, sizeof(sid)) < 0) {
            LOG("session create failed, retry in 5s");
            vTaskDelay(5000 / portTICK_RATE_MS);
            continue;
        }

        /* Step 2: Subscribe to all items */
        int sub_ok = 1;
        for (int i = 0; i < TELEM_COUNT; ++i) {
            if (ls_subscribe(sid, i + 1, i + 1, g_telem[i].id) < 0) {
                LOG("subscribe failed for %s", g_telem[i].id);
                sub_ok = 0;
                break;
            }
        }
        if (!sub_ok) {
            vTaskDelay(5000 / portTICK_RATE_MS);
            continue;
        }

        /* Step 3: Poll loop */
        for (;;) {
            int rc = ls_poll(sid);
            if (rc == -2) {
                LOG("session lost, reconnecting...");
                break;  /* outer loop will create new session */
            }
            vTaskDelay(POLL_INTERVAL_MS / portTICK_RATE_MS);
        }
    }
}
