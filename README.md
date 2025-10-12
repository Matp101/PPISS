# PPISS
**P**ee **P**ercentage of the **I**nternational **S**pace **S**tation

Display the ISS Urine Tank Quantity on a GC9A01 round TFT display using an ESP8266 Wemos D1 Mini.

![PPISS Project](https://img.shields.io/badge/ESP8266-Compatible-blue)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## 🎯 Start Here

**New to this project?** Follow this path:

1. 📖 **[QUICKSTART.md](QUICKSTART.md)** - Get running in 10 minutes!
2. 🔧 **[TFT_eSPI_SETUP.md](TFT_eSPI_SETUP.md)** - Configure your display
3. ❓ **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Having issues? Check here
4. 📚 Read the sections below for details

---

## 🚀 Quick Links

| Document | Purpose | When to Use |
|----------|---------|-------------|
| **[QUICKSTART.md](QUICKSTART.md)** | Step-by-step setup guide | First time setup |
| **[TFT_eSPI_SETUP.md](TFT_eSPI_SETUP.md)** | Display library configuration | Before uploading code |
| **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** | Problem diagnosis | When something doesn't work |
| **[LIBRARIES.md](LIBRARIES.md)** | Library installation details | Library issues |
| **[FILE_STRUCTURE.md](FILE_STRUCTURE.md)** | Explains all project files | Understanding the project |
| **[API_INTEGRATION.md](API_INTEGRATION.md)** | How to add real ISS data | Future development |

---

## 📺 Project Overview

This project connects an ESP8266 microcontroller to the internet to fetch and display ISS (International Space Station) telemetry data on a beautiful round TFT display. Specifically, it shows the urine tank fill percentage - a fun and educational way to learn about IoT and the ISS!

## 🛠️ Hardware Requirements

- **ESP8266 Wemos D1 Mini** - WiFi-enabled microcontroller (~$4)
- **GC9A01 1.28" TFT Display (240x240)** - Round LCD display (~$8)
- **Connecting wires** - For wiring the display to the ESP8266
- **USB cable** - For programming and power

**Total cost: ~$12-15**

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
4. **TFT_eSPI** by Bodmer ⭐ **RECOMMENDED** - [Configuration Guide](TFT_eSPI_SETUP.md)

Alternative display library:
5. **Arduino_GFX** by moononournation (if you prefer not to use TFT_eSPI)

> 💡 **Tip:** Use `PPISS_TFT_eSPI.ino` (with TFT_eSPI library) for the easiest setup!

See [LIBRARIES.md](LIBRARIES.md) for detailed installation instructions.

## 📦 Installation & Configuration

> 📖 **New to this?** Start with the [Quick Start Guide](QUICKSTART.md)!

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

3. **Configure TFT_eSPI Library** (if using TFT_eSPI)
   - See detailed guide: [TFT_eSPI_SETUP.md](TFT_eSPI_SETUP.md)
   - Quick version: Edit `User_Setup.h` in TFT_eSPI library folder

4. **Test Your Hardware** (Optional but recommended)
   - Upload `HardwareTest.ino` to verify display works
   - Should see colorful graphics and "Test OK!" message

5. **Upload the Main Sketch**
   - Connect your Wemos D1 Mini to your computer via USB
   - Open `PPISS_TFT_eSPI.ino` in Arduino IDE (recommended)
     - OR `PPISS.ino` if using Arduino_GFX library
   - Select the correct board: `Tools > Board > LOLIN(WEMOS) D1 R2 & mini`
   - Select the correct port: `Tools > Port > [Your COM port]`
   - Click the Upload button (→)

6. **Monitor Serial Output** (Optional but recommended for debugging)
   - Open Serial Monitor: `Tools > Serial Monitor`
   - Set baud rate to `115200`
   - You should see connection status and data updates

## ✨ Features

- ✅ WiFi connectivity with status indicator
- ✅ Automatic data fetching from ISS API
- ✅ Visual percentage display on round TFT with color coding
- ✅ Update interval: 1 minute (configurable)
- ✅ Error handling and display
- ✅ Low power consumption during idle
- ✅ Serial debugging output
- ✅ Hardware test sketch included

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

## 🎨 Customization

### Change Update Interval
Edit the `UPDATE_INTERVAL` constant in `PPISS_TFT_eSPI.ino`:
```cpp
const unsigned long UPDATE_INTERVAL = 60000; // milliseconds (60000 = 1 minute)
```

### Adjust Display Pins
If your wiring is different, update the pin definitions:
```cpp
#define TFT_DC    D4   // Data/Command pin
#define TFT_CS    D8   // Chip select pin
#define TFT_RST   D3   // Reset pin
```

See [QUICKSTART.md](QUICKSTART.md) for the wiring diagram.

### Modify Display Colors
Colors are defined in RGB565 format (16-bit) when using TFT_eSPI. Common colors:
- `TFT_BLACK` - Black
- `TFT_WHITE` - White
- `TFT_RED` - Red
- `TFT_GREEN` - Green
- `TFT_BLUE` - Blue
- `TFT_CYAN` - Cyan
- `TFT_YELLOW` - Yellow

### Add Real ISS Data
See **[API_INTEGRATION.md](API_INTEGRATION.md)** for detailed instructions on integrating real ISS APIs.

## 🔧 Troubleshooting

### Display shows nothing
- Check wiring connections (see **[QUICKSTART.md](QUICKSTART.md)** for wiring diagram)
- Verify power supply (3.3V, NOT 5V!)
- Run `HardwareTest.ino` to verify hardware works
- Ensure TFT_eSPI library is correctly configured (see **[TFT_eSPI_SETUP.md](TFT_eSPI_SETUP.md)**)
- Try adjusting display rotation in code
- See **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** for systematic debugging

### WiFi connection fails
- Verify WiFi credentials in `config.h`
- Check if your WiFi network is 2.4GHz (ESP8266 doesn't support 5GHz)
- Ensure the ESP8266 is within range of your router
- No MAC address filtering on your router
- See **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** WiFi section

### Data not updating
- Check Serial Monitor for error messages
- Verify internet connectivity
- Ensure the API endpoint is accessible
- See **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** data section

### Compilation errors
- Make sure all required libraries are installed (see **[LIBRARIES.md](LIBRARIES.md)**)
- Verify ESP8266 board package is up to date
- Check that you've created `config.h` from `config.h.example`
- See **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** compilation section

### More help needed?
See the comprehensive **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** checklist!

## 🤝 Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.

### Ideas for Contributions
- Integration with real ISS telemetry APIs
- Additional ISS data displays (water tank, oxygen levels, etc.)
- Power saving modes
- Web interface for configuration
- Multiple display modes
- Historical data graphing

## 📄 License

This project is open source and available under the MIT License.

## 🙏 Acknowledgments

- NASA for ISS data and inspiration
- ESP8266 Community for the excellent board support
- Bodmer for the TFT_eSPI library
- Adafruit for the GFX library
- The Arduino community

## 📊 Project Status

🚧 **Work in Progress** - This project currently uses mock data for demonstration. Integration with actual ISS telemetry APIs is needed for real urine tank percentage data.

### What Works Now ✅
- ✅ Complete hardware setup and wiring
- ✅ ESP8266 WiFi connectivity
- ✅ GC9A01 display driver and rendering
- ✅ Mock data display (demonstrates functionality)
- ✅ Comprehensive documentation
- ✅ Hardware test utilities
- ✅ Error handling and display

### Future Enhancements 🚀
- [ ] Real ISS API integration (see **[API_INTEGRATION.md](API_INTEGRATION.md)**)
- [ ] Web configuration interface
- [ ] OTA (Over-The-Air) updates
- [ ] Multiple data views (water, oxygen, etc.)
- [ ] Historical data graphing
- [ ] Power saving modes
- [ ] Sound alerts

### How to Contribute
See **[API_INTEGRATION.md](API_INTEGRATION.md)** if you have access to real ISS telemetry APIs!

## 📞 Support & Community

- **Issues:** Report bugs or request features via [GitHub Issues](https://github.com/Matp101/PPISS/issues)
- **Documentation:** Check the guides in this repository
- **Hardware Questions:** See [QUICKSTART.md](QUICKSTART.md) and [TFT_eSPI_SETUP.md](TFT_eSPI_SETUP.md)

---

**Made with ❤️ for space enthusiasts and IoT learners**
