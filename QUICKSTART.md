# Quick Start Guide

Get your PPISS project running in 10 minutes!

## Prerequisites Checklist

- [ ] ESP8266 Wemos D1 Mini
- [ ] GC9A01 1.28" TFT Display (240x240)
- [ ] USB cable for programming
- [ ] Jumper wires or soldered connections
- [ ] Arduino IDE installed
- [ ] 2.4GHz WiFi network credentials

## Step-by-Step Setup

### 1. Hardware Assembly (5 minutes)

Connect the display to your Wemos D1 Mini:

```
GC9A01 Display          Wemos D1 Mini
┌─────────────┐        ┌─────────────┐
│ VCC    ○────┼────────┼──○ 3.3V     │
│ GND    ○────┼────────┼──○ GND      │
│ SCL    ○────┼────────┼──○ D5       │
│ SDA    ○────┼────────┼──○ D7       │
│ RES    ○────┼────────┼──○ D3       │
│ DC     ○────┼────────┼──○ D4       │
│ CS     ○────┼────────┼──○ D8       │
│ BL     ○────┼────────┼──○ 3.3V     │
└─────────────┘        └─────────────┘
```

**Wiring Table:**
| Display | Wemos | Color Suggestion |
|---------|-------|------------------|
| VCC     | 3.3V  | Red              |
| GND     | GND   | Black            |
| SCL     | D5    | Yellow           |
| SDA     | D7    | Green            |
| RES     | D3    | Blue             |
| DC      | D4    | Orange           |
| CS      | D8    | Purple           |
| BL      | 3.3V  | White            |

### 2. Software Setup (3 minutes)

#### Install Arduino IDE
If not already installed:
1. Download from [arduino.cc](https://www.arduino.cc/en/software)
2. Install and launch

#### Add ESP8266 Board Support
1. `File > Preferences`
2. Add to "Additional Boards Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. `Tools > Board > Boards Manager`
4. Search "esp8266" → Install

#### Install Required Libraries
1. `Sketch > Include Library > Manage Libraries`
2. Install these libraries:
   - **ArduinoJson** (by Benoit Blanchon)
   - **TFT_eSPI** (by Bodmer) - RECOMMENDED

### 3. Configure TFT_eSPI Library (2 minutes)

**Important:** TFT_eSPI needs configuration before use!

See detailed instructions in `TFT_eSPI_SETUP.md`, or quick version:

1. Find TFT_eSPI library folder:
   - Windows: `Documents\Arduino\libraries\TFT_eSPI`
   - Mac/Linux: `~/Documents/Arduino/libraries/TFT_eSPI`

2. Edit `User_Setup.h`:
   - Uncomment: `#define GC9A01_DRIVER`
   - Set pins as shown in `TFT_eSPI_SETUP.md`
   - Save and close

### 4. Configure WiFi (1 minute)

1. Copy `config.h.example` to `config.h`
2. Edit `config.h` with your WiFi details:
   ```cpp
   const char* WIFI_SSID = "YourNetworkName";
   const char* WIFI_PASSWORD = "YourPassword";
   ```
3. Save

### 5. Upload Code (2 minutes)

1. Connect Wemos D1 Mini via USB
2. Open `PPISS_TFT_eSPI.ino` in Arduino IDE
3. `Tools > Board` → Select "LOLIN(WEMOS) D1 R2 & mini"
4. `Tools > Port` → Select your COM port
5. Click Upload (→) button
6. Wait for "Done uploading" message

### 6. Enjoy! (30 seconds)

The display should show:
1. "Initializing..." message
2. "Connecting WiFi..." with progress
3. "WiFi Connected!" confirmation
4. Main display with ISS urine tank percentage

## Troubleshooting

### "Board not found" error
- Install ESP8266 board support (Step 2)

### "Library not found" error
- Install missing library via Library Manager

### Display is blank
- Check wiring, especially VCC and GND
- Verify TFT_eSPI is configured (Step 3)

### WiFi won't connect
- Check credentials in config.h
- Ensure 2.4GHz WiFi (ESP8266 doesn't support 5GHz)
- Move closer to router

### Compilation errors
- Ensure config.h exists (copy from config.h.example)
- Verify all libraries installed
- Check TFT_eSPI configuration

## Expected Behavior

**Serial Monitor Output:**
```
PPISS - ISS Urine Tank Display
================================
Initializing display...
Display initialized
Connecting to WiFi: YourNetwork
..........
WiFi connected!
IP address: 192.168.1.xxx
Fetching ISS data...
Response received
Urine Tank Percentage: 42%
```

**Display Shows:**
- Title: "ISS"
- Subtitle: "Urine Tank"
- Large percentage number in center
- WiFi status indicator (top left)
- Update timestamp (bottom)
- Color-coded fill indicator

## What's Next?

### Current Limitations
- Uses mock data (timestamp-based percentage)
- Real ISS urine tank data requires NASA API access

### Customization Ideas
- Change update interval (default: 1 minute)
- Modify colors
- Add more ISS telemetry data
- Add sound alerts for full tank

### Get Real ISS Data
To display actual ISS telemetry:
1. Research NASA API access
2. Find urine tank data endpoint
3. Update `ISS_API_URL` in code
4. Modify JSON parsing to match API response

## Support

- Check README.md for detailed documentation
- See TFT_eSPI_SETUP.md for display configuration
- See LIBRARIES.md for library details

## Files Overview

| File | Purpose |
|------|---------|
| `PPISS_TFT_eSPI.ino` | Main sketch (RECOMMENDED) |
| `PPISS.ino` | Alternative using Arduino_GFX |
| `config.h.example` | WiFi config template |
| `config.h` | Your WiFi credentials (you create this) |
| `README.md` | Full documentation |
| `TFT_eSPI_SETUP.md` | Display configuration guide |
| `LIBRARIES.md` | Library installation details |

**Start with PPISS_TFT_eSPI.ino - it's easier to configure!**

---

**Estimated Total Time: ~13 minutes**
(5 min hardware + 3 min software + 2 min TFT config + 1 min WiFi + 2 min upload)
