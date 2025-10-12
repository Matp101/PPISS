# Required Arduino Libraries

This document lists all the libraries needed for the PPISS project and how to install them.

## Installation Methods

### Method 1: Arduino IDE Library Manager (Recommended)

1. Open Arduino IDE
2. Go to `Sketch > Include Library > Manage Libraries...`
3. Search for each library name and click "Install"

### Method 2: Manual Installation

Download the library ZIP file and install via `Sketch > Include Library > Add .ZIP Library...`

## Library List

### 1. ESP8266WiFi
- **Description:** WiFi connectivity for ESP8266
- **Source:** Built-in with ESP8266 board package
- **Installation:** Automatically installed with ESP8266 board support
- **Required Version:** Any (comes with board package)

### 2. ESP8266HTTPClient
- **Description:** HTTP client for making web requests
- **Source:** Built-in with ESP8266 board package
- **Installation:** Automatically installed with ESP8266 board support
- **Required Version:** Any (comes with board package)

### 3. ArduinoJson
- **Author:** Benoit Blanchon
- **Description:** JSON parsing and serialization
- **Source:** Arduino Library Manager
- **Installation:** Search "ArduinoJson" in Library Manager
- **Required Version:** 6.x (latest stable)
- **GitHub:** https://github.com/bblanchon/ArduinoJson

### 4. Adafruit GFX Library
- **Author:** Adafruit
- **Description:** Core graphics library for displays
- **Source:** Arduino Library Manager
- **Installation:** Search "Adafruit GFX" in Library Manager
- **Required Version:** Latest
- **GitHub:** https://github.com/adafruit/Adafruit-GFX-Library

### 5. GC9A01 Display Driver

You have **two options** for the GC9A01 driver:

#### Option A: Arduino_GFX (Recommended)
- **Author:** moononournation
- **Description:** Comprehensive graphics library with GC9A01 support
- **Source:** Arduino Library Manager or GitHub
- **Installation:** Search "GFX Library for Arduino" in Library Manager
- **GitHub:** https://github.com/moononournation/Arduino_GFX
- **Note:** After installation, you'll need to modify the include in PPISS.ino:
  ```cpp
  #include <Arduino_GFX_Library.h>
  #include <Arduino_DataBus.h>
  #include <Arduino_GC9A01.h>
  ```

#### Option B: Adafruit_GC9A01A
- **Author:** Adafruit (community fork)
- **Description:** Dedicated GC9A01 driver
- **Installation:** May need manual installation from GitHub
- **GitHub:** Search for "Adafruit_GC9A01A" or similar forks

## Code Modifications Based on Library Choice

### If using Arduino_GFX library:

Replace the includes in `PPISS.ino`:
```cpp
// Old includes
#include <Adafruit_GFX.h>
#include <Arduino_GC9A01.h>

// New includes
#include <Arduino_GFX_Library.h>
#include <Arduino_DataBus.h>
#include <Arduino_GC9A01.h>
```

And update the display initialization:
```cpp
// Create a hardware SPI data bus
Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);
// Create display with the data bus
Arduino_GC9A01 *tft = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */);
```

## Verification

After installing all libraries, verify by:
1. Opening `PPISS.ino`
2. Clicking `Sketch > Verify/Compile`
3. Check for any "library not found" errors

If you encounter errors, ensure:
- All libraries are installed
- ESP8266 board package is installed
- You're using Arduino IDE 1.8.13 or later (or Arduino IDE 2.x)

## Library Sizes (Approximate)

- ArduinoJson: ~100 KB
- Adafruit GFX: ~50 KB
- Arduino_GFX: ~200 KB
- ESP8266 Libraries: ~500 KB (part of board package)

**Total additional download:** ~350-850 KB depending on driver choice

## Alternative Libraries

If you have issues with the recommended libraries, here are alternatives:

### For GC9A01:
- TFT_eSPI (with custom setup) - https://github.com/Bodmer/TFT_eSPI
- LovyanGFX - https://github.com/lovyan03/LovyanGFX

### For JSON:
- ArduinoJson is the standard and highly recommended

### For HTTP:
- ESP8266WiFi and ESP8266HTTPClient are standard for ESP8266

## Support

If you encounter library-related issues:
1. Check the library's GitHub page for documentation
2. Ensure you have the latest version
3. Verify compatibility with ESP8266
4. Check Arduino IDE version (recommend 1.8.13+)
