# PPISS
Pee Percentage of the International Space Station

Display the ISS Urine Tank Quantity on a GC9A01 round TFT display using an ESP8266 Wemos D1 Mini.

![PPISS Project](https://img.shields.io/badge/ESP8266-Compatible-blue)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-green)

## Hardware Requirements

- **ESP8266 Wemos D1 Mini** - WiFi-enabled microcontroller
- **GC9A01 1.28" TFT Display (240x240)** - Round LCD display
- **Connecting wires** - For wiring the display to the ESP8266

## Wiring Diagram

Connect the GC9A01 display to the Wemos D1 Mini as follows:

| GC9A01 Pin | Wemos D1 Mini Pin | GPIO |
|------------|-------------------|------|
| VCC        | 3.3V              | -    |
| GND        | GND               | -    |
| SCL/SCK    | D5                | GPIO14 (SCLK) |
| SDA/MOSI   | D7                | GPIO13 (MOSI) |
| RES/RST    | D3                | GPIO0  |
| DC         | D4                | GPIO2  |
| CS         | D8                | GPIO15 |
| BL (Backlight) | 3.3V (optional, can connect to PWM pin for brightness control) | - |

**Note:** Some GC9A01 displays may have slightly different pin labels. Refer to your display's documentation.

## Software Requirements

### Arduino IDE Setup

1. **Install Arduino IDE** (version 1.8.13 or later)
   - Download from [arduino.cc](https://www.arduino.cc/en/software)

2. **Install ESP8266 Board Support**
   - Open Arduino IDE
   - Go to `File > Preferences`
   - Add this URL to "Additional Boards Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to `Tools > Board > Boards Manager`
   - Search for "esp8266" and install "ESP8266 by ESP8266 Community"

3. **Select the Board**
   - Go to `Tools > Board > ESP8266 Boards`
   - Select "LOLIN(WEMOS) D1 R2 & mini"

### Required Libraries

Install the following libraries via `Tools > Manage Libraries` in Arduino IDE:

1. **ESP8266WiFi** (included with ESP8266 board package)
2. **ESP8266HTTPClient** (included with ESP8266 board package)
3. **ArduinoJson** by Benoit Blanchon (version 6.x)
   - Search for "ArduinoJson" in Library Manager
4. **Adafruit GFX Library** by Adafruit
   - Search for "Adafruit GFX" in Library Manager
5. **Arduino_GC9A01** or equivalent GC9A01 driver
   - You may need to install this manually from GitHub:
   - [Arduino_GC9A01](https://github.com/moononournation/Arduino_GFX) (part of Arduino_GFX library)
   - Alternative: Search for "GC9A01" in Library Manager

**Note:** If you can't find Arduino_GC9A01, you can use the more comprehensive **Arduino_GFX** library by moononournation, which includes GC9A01 support.

## Installation & Configuration

1. **Clone or download this repository**
   ```bash
   git clone https://github.com/Matp101/PPISS.git
   cd PPISS
   ```

2. **Configure WiFi Credentials**
   - Copy `config.h.example` to `config.h`
     ```bash
     cp config.h.example config.h
     ```
   - Edit `config.h` with your WiFi credentials:
     ```cpp
     const char* WIFI_SSID = "YourWiFiNetworkName";
     const char* WIFI_PASSWORD = "YourWiFiPassword";
     ```

3. **Upload the Sketch**
   - Connect your Wemos D1 Mini to your computer via USB
   - Open `PPISS.ino` in Arduino IDE
   - Select the correct board: `Tools > Board > LOLIN(WEMOS) D1 R2 & mini`
   - Select the correct port: `Tools > Port > [Your COM port]`
   - Click the Upload button (→)

4. **Monitor Serial Output** (Optional but recommended for debugging)
   - Open Serial Monitor: `Tools > Serial Monitor`
   - Set baud rate to `115200`
   - You should see connection status and data updates

## Features

- ✅ WiFi connectivity with status indicator
- ✅ Automatic data fetching from ISS API
- ✅ Visual percentage display on round TFT
- ✅ Update interval: 1 minute (configurable)
- ✅ Error handling and display
- ✅ Low power consumption during idle

## API Information

**Important Note:** The current implementation uses a placeholder API endpoint. The actual ISS urine tank telemetry data requires access to NASA's internal systems or specific authorized APIs.

### Current Implementation
The sketch currently uses `http://api.open-notify.org/iss-now.json` as a placeholder and generates mock data for demonstration purposes.

### For Real ISS Data
To display actual ISS urine tank data, you would need to:
1. Obtain access to NASA's ISS telemetry APIs
2. Update the `ISS_API_URL` constant in `PPISS.ino`
3. Modify the JSON parsing in `fetchISSData()` to match the actual API response structure

Potential sources for ISS data:
- [NASA Open APIs](https://api.nasa.gov/)
- [ISS Mimic API](https://iss-mimic.github.io/Mimic/)
- [Lightstreamer ISS Live](https://www.lightstreamer.com/demos/iss-live/)

## Customization

### Change Update Interval
Edit the `UPDATE_INTERVAL` constant in `PPISS.ino`:
```cpp
const unsigned long UPDATE_INTERVAL = 60000; // milliseconds (60000 = 1 minute)
```

### Adjust Display Pins
If your wiring is different, update the pin definitions:
```cpp
#define TFT_DC    D4   // Data/Command pin
#define TFT_CS    D8   // Chip Select pin
#define TFT_RST   D3   // Reset pin
```

### Modify Display Colors
Colors are defined in RGB565 format (16-bit). Common colors:
- `0x0000` - Black
- `0xFFFF` - White
- `0xF800` - Red
- `0x07E0` - Green
- `0x001F` - Blue
- `0x07FF` - Cyan
- `0xFFE0` - Yellow

## Troubleshooting

### Display shows nothing
- Check wiring connections
- Verify power supply (3.3V)
- Ensure GC9A01 library is correctly installed
- Try adjusting display rotation in `initDisplay()`

### WiFi connection fails
- Verify WiFi credentials in `config.h`
- Check if your WiFi network is 2.4GHz (ESP8266 doesn't support 5GHz)
- Ensure the ESP8266 is within range of your router

### Data not updating
- Check Serial Monitor for error messages
- Verify internet connectivity
- Ensure the API endpoint is accessible

### Compilation errors
- Make sure all required libraries are installed
- Verify ESP8266 board package is up to date
- Check that you've created `config.h` from `config.h.example`

## Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.

## License

This project is open source and available under the MIT License.

## Acknowledgments

- NASA for ISS data and inspiration
- ESP8266 Community for the excellent board support
- Adafruit for the GFX library
- The Arduino community

## Project Status

🚧 **Work in Progress** - This project currently uses mock data. Integration with actual ISS telemetry APIs is needed for real urine tank percentage data.
