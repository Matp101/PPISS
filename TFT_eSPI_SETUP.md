# TFT_eSPI Library Configuration Guide

This guide helps you configure the TFT_eSPI library for the GC9A01 display.

## Why TFT_eSPI?

TFT_eSPI is a popular, well-maintained library with excellent performance and broad display support. It's often easier to get working than other libraries.

## Installation

1. Open Arduino IDE
2. Go to `Sketch > Include Library > Manage Libraries`
3. Search for "TFT_eSPI"
4. Install "TFT_eSPI by Bodmer"

## Configuration Steps

The TFT_eSPI library needs to be configured for your specific display and wiring. Follow these steps:

### Step 1: Locate the Library Folder

Find where Arduino libraries are installed:
- **Windows:** `Documents\Arduino\libraries\TFT_eSPI`
- **Mac:** `~/Documents/Arduino/libraries/TFT_eSPI`
- **Linux:** `~/Arduino/libraries/TFT_eSPI`

### Step 2: Edit User_Setup.h

Open `User_Setup.h` in a text editor and make the following changes:

#### 2a. Comment out existing driver

Find and comment out any existing driver definitions (add `//` at the start):
```cpp
// #define ILI9341_DRIVER
// #define ST7735_DRIVER
```

#### 2b. Enable GC9A01 driver

Find and uncomment (or add) the GC9A01 driver:
```cpp
#define GC9A01_DRIVER
```

#### 2c. Set display dimensions

```cpp
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
```

#### 2d. Configure pins for ESP8266

Find the ESP8266 section and configure pins:
```cpp
// For ESP8266 (Wemos D1 Mini)
#define TFT_MISO -1      // Not used for display
#define TFT_MOSI D7      // GPIO13
#define TFT_SCLK D5      // GPIO14
#define TFT_CS   D8      // GPIO15 - Chip select
#define TFT_DC   D4      // GPIO2  - Data/Command
#define TFT_RST  D3      // GPIO0  - Reset
```

**Note:** D-numbers (D3, D4, etc.) are specific to Wemos D1 Mini. They map to GPIO numbers.

#### 2e. Enable fonts

Uncomment these font definitions:
```cpp
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font
#define LOAD_FONT2  // Font 2. Small 16 pixel font
#define LOAD_FONT4  // Font 4. Medium 26 pixel font
#define LOAD_FONT6  // Font 6. Large 48 pixel font
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font
#define LOAD_FONT8  // Font 8. Large 75 pixel font
```

#### 2f. Set SPI frequency

```cpp
#define SPI_FREQUENCY  40000000  // 40MHz
// Reduce to 27000000 or 20000000 if you have display issues
```

### Step 3: Save and Close

Save `User_Setup.h` and close the text editor.

### Step 4: Restart Arduino IDE

Close and reopen Arduino IDE to ensure the changes take effect.

## Alternative: Using User_Setup_Select.h

Instead of editing `User_Setup.h`, you can use `User_Setup_Select.h`:

1. Open `User_Setup_Select.h`
2. Comment out the default setup line
3. Uncomment or add a setup for ESP8266 with GC9A01
4. Create a custom setup file (e.g., `Setup_ESP8266_GC9A01.h`) in the `User_Setups` folder

## Complete User_Setup.h Example

Here's a complete minimal configuration:

```cpp
// User_Setup.h for GC9A01 with ESP8266 Wemos D1 Mini

#define GC9A01_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ESP8266 pins (Wemos D1 Mini)
#define TFT_MISO -1
#define TFT_MOSI D7  // GPIO13
#define TFT_SCLK D5  // GPIO14
#define TFT_CS   D8  // GPIO15
#define TFT_DC   D4  // GPIO2
#define TFT_RST  D3  // GPIO0

// Fonts to load
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8

#define SPI_FREQUENCY  40000000
```

## Pin Reference

| Display Pin | Wemos Pin | GPIO | Function |
|-------------|-----------|------|----------|
| MOSI/SDA    | D7        | 13   | Data out |
| SCLK/SCL    | D5        | 14   | Clock    |
| CS          | D8        | 15   | Chip Select |
| DC/RS       | D4        | 2    | Data/Command |
| RST/RESET   | D3        | 0    | Reset    |
| VCC         | 3.3V      | -    | Power    |
| GND         | GND       | -    | Ground   |

## Troubleshooting

### Display is blank/white
- Check wiring connections
- Try reducing SPI_FREQUENCY to 20000000
- Verify power supply is stable 3.3V
- Make sure GC9A01_DRIVER is defined

### Wrong colors
- Check if you need to uncomment `#define TFT_RGB_ORDER TFT_BGR`

### Display upside down
- Change rotation in code: `tft.setRotation(2);` (values 0-3)

### Compile errors
- Make sure only ONE driver is uncommented
- Verify all required fonts are loaded
- Check that pin definitions match your wiring

### Display shows garbage/noise
- Reduce SPI frequency
- Add a small delay after tft.init()
- Check for loose connections

## Testing the Configuration

Upload this simple test sketch:

```cpp
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(50, 100);
  tft.print("Hello!");
  Serial.println("Test complete");
}

void loop() {
  // Nothing
}
```

If you see "Hello!" on the display, your configuration is correct!

## Resources

- [TFT_eSPI GitHub](https://github.com/Bodmer/TFT_eSPI)
- [TFT_eSPI Documentation](https://github.com/Bodmer/TFT_eSPI/blob/master/docs/README.md)
- [ESP8266 Pinout](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)

## Using the PPISS Project

Once TFT_eSPI is configured:

1. Use `PPISS_TFT_eSPI.ino` instead of `PPISS.ino`
2. Configure WiFi in `config.h`
3. Upload and enjoy!
