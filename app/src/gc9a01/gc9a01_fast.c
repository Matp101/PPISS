#include "gc9a01.h"

#ifdef GC9A01_ENABLE_HSPI

#include "gc9a01_common.h"

#include "gpio.h"
#include "spi_interface.h"

#ifndef os_printf
#define os_printf printf
#endif

/* ===== Small GPIO helpers ===== */
static inline void cs_low (void){ GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1U << PIN_CS); }
static inline void cs_high(void){ GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1U << PIN_CS); }
static inline void dc_low (void){ GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1U << PIN_DC); }
static inline void dc_high(void){ GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1U << PIN_DC); }

/* ===== SDK compatibility (older RTOS-SDKs don’t define these) ===== */
#ifndef FUNC_HSPID     // MTCK -> HSPI MOSI
#define FUNC_HSPID 2
#endif
#ifndef FUNC_HSPIQ     // MTDI -> HSPI MISO
#define FUNC_HSPIQ 2
#endif
#ifndef FUNC_HSPI_CLK  // MTMS -> HSPI CLK
#define FUNC_HSPI_CLK 2
#endif
#ifndef FUNC_HSPI_CS0  // MTDO -> HSPI CS0
#define FUNC_HSPI_CS0 2
#endif

/* ===== HSPI: init and raw write ===== */
static void hspi_init(void)
{
    LOG("mux HSPI pins");
    
    // Route the JTAG/HSPI pad set to HSPI
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_HSPID);    // GPIO13 MOSI
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_HSPI_CLK); // GPIO14 SCLK
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_HSPIQ);    // GPIO12 MISO (unused)

    // CS as plain GPIO (held LOW – GC9A01 doesn’t need MISO, so CS can be fixed)
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
    GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, 1U << PIN_CS);
    cs_low();

    // DC as GPIO output, idle = HIGH (data mode)
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, 1U << PIN_DC);
    dc_high();

    // Configure HSPI
    SpiAttr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mode     = SpiMode_Master;
    attr.subMode  = SpiSubMode_0;          // MODE0 (CPOL=0, CPHA=0)
    attr.bitOrder = SpiBitOrder_MSBFirst;
    attr.speed    = SpiSpeed_20MHz;        // Try 20MHz; drop to 10MHz if flaky
    SPIInit(SpiNum_HSPI, &attr);

    LOG("HSPI ready: MODE0, MSB, ~20MHz");
}

/* Pack bytes into 32-bit words and transmit (HW limit: ≤64 bytes per call) */
static void hspi_write_bytes(const uint8_t *buf, uint32_t len)
{
    while (len) {
        uint32_t chunk = (len > 64U) ? 64U : len;

        // HSPI expects 32-bit words; pack little-endian within each word
        uint32_t w[16]; // 16 * 4 = 64 bytes
        memset(w, 0, sizeof(w));
        for (uint32_t i = 0; i < chunk; ++i) {
            uint32_t wi = i >> 2;
            uint32_t bi = i & 3;
            w[wi] |= ((uint32_t)buf[i]) << (bi * 8U);
        }

        SpiData d;
        memset(&d, 0, sizeof(d));
        d.dataLen = (uint8_t)chunk; // bytes
        d.data    = w;

        SPIMasterSendData(SpiNum_HSPI, &d);

        buf += chunk;
        len -= chunk;
    }
}

/* ===== GC9A01 command helpers ===== */
static inline void wr_cmd(uint8_t c)  { dc_low();  hspi_write_bytes(&c, 1); }
static inline void wr_dat8(uint8_t d) { dc_high(); hspi_write_bytes(&d, 1); }
static inline void wr_dat16(uint16_t d){
    uint8_t b[2] = { (uint8_t)(d >> 8), (uint8_t)(d & 0xFF) };
    dc_high(); hspi_write_bytes(b, 2);
}

/* ===== Address window & simple fill ===== */
static void set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint16_t x2 = x + w - 1, y2 = y + h - 1;
    wr_cmd(0x2A); wr_dat16(x);  wr_dat16(x2); // CASET
    wr_cmd(0x2B); wr_dat16(y);  wr_dat16(y2); // RASET
    wr_cmd(0x2C);                              // RAMWR
}

static void fill_screen(uint16_t color)
{
    set_addr_window(0, 0, TFT_W, TFT_H);

    // 64-byte block with repeated color
    uint8_t blk[64];
    for (uint32_t i = 0; i < sizeof(blk); i += 2) {
        blk[i]   = (uint8_t)(color >> 8);
        blk[i+1] = (uint8_t)(color & 0xFF);
    }

    uint32_t pixels = (uint32_t)TFT_W * (uint32_t)TFT_H;
    while (pixels) {
        uint32_t bytes = (pixels * 2U > sizeof(blk)) ? sizeof(blk) : pixels * 2U;
        dc_high();
        hspi_write_bytes(blk, bytes);
        pixels -= bytes / 2U;
    }
}

/* ===== Minimal GC9A01 init (Adafruit table subset) ===== */
#define MADCTL_MX  0x40
#define MADCTL_BGR 0x08

static void gc9a01_init(void)
{
    LOG("GC9A01 init");
    wr_cmd(0x01);                // SWRESET
    ets_delay_us(150000);

    // Manufacturer-ish sequence (short subset that is known-good)
    wr_cmd(0xFE);                // INREGEN1
    wr_cmd(0xEF);                // INREGEN2

    wr_cmd(0xEB); wr_dat8(0x14);
    wr_cmd(0x84); wr_dat8(0x40);
    wr_cmd(0x85); wr_dat8(0xFF);
    wr_cmd(0x86); wr_dat8(0xFF);
    wr_cmd(0x87); wr_dat8(0xFF);
    wr_cmd(0x88); wr_dat8(0x0A);
    wr_cmd(0x89); wr_dat8(0x21);
    wr_cmd(0x8A); wr_dat8(0x00);
    wr_cmd(0x8B); wr_dat8(0x80);
    wr_cmd(0x8C); wr_dat8(0x01);
    wr_cmd(0x8D); wr_dat8(0x01);
    wr_cmd(0x8E); wr_dat8(0xFF);
    wr_cmd(0x8F); wr_dat8(0xFF);

    wr_cmd(0xB6); wr_dat8(0x00); wr_dat8(0x00);
    wr_cmd(0x36); wr_dat8(MADCTL_MX | MADCTL_BGR); // Memory access control
    wr_cmd(0x3A); wr_dat8(0x05);                   // 16bpp

    wr_cmd(0x21);                // INVON
    wr_cmd(0x11);                // SLPOUT
    ets_delay_us(120000);
    wr_cmd(0x29);                // DISPON
    ets_delay_us(20000);

    LOG("GC9A01 init done");
}

/* ===== UI task: cycle RED->GREEN->BLUE forever ===== */
void ui_task(void *param)
{
    (void)param;
    LOG("ui_task start (HSPI)");

    hspi_init();
    gc9a01_init();

    for (;;) {
        LOG("fill RED");   fill_screen(0xF800); vTaskDelay(400 / portTICK_RATE_MS);
        LOG("fill GREEN"); fill_screen(0x07E0); vTaskDelay(400 / portTICK_RATE_MS);
        LOG("fill BLUE");  fill_screen(0x001F); vTaskDelay(400 / portTICK_RATE_MS);
    }
}

#endif // GC9A01_ENABLE_HSPI