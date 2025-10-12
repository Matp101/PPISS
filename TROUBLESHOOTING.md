# Troubleshooting Checklist

Use this checklist to systematically diagnose and fix common issues.

## 🔍 Pre-Upload Checklist

Before uploading code, verify:

- [ ] ESP8266 board support installed in Arduino IDE
- [ ] Correct board selected: `LOLIN(WEMOS) D1 R2 & mini`
- [ ] Correct COM port selected
- [ ] All required libraries installed (see LIBRARIES.md)
- [ ] TFT_eSPI configured (if using PPISS_TFT_eSPI.ino)
- [ ] `config.h` created from `config.h.example`
- [ ] WiFi credentials entered in `config.h`

## 🔌 Hardware Connection Checklist

Verify all connections:

- [ ] VCC connected to 3.3V (NOT 5V!)
- [ ] GND connected to GND
- [ ] SCL connected to D5
- [ ] SDA connected to D7
- [ ] RES connected to D3
- [ ] DC connected to D4
- [ ] CS connected to D8
- [ ] All connections are secure (not loose)
- [ ] No short circuits between wires

## 📺 Display Not Working

### Step 1: Power Check
- [ ] Wemos D1 Mini power LED is on
- [ ] Display backlight is on (screen should glow)
- [ ] Voltage at display VCC is 3.3V (use multimeter if available)

### Step 2: Library Check
- [ ] TFT_eSPI library installed
- [ ] User_Setup.h edited correctly
- [ ] `#define GC9A01_DRIVER` is uncommented
- [ ] Pin definitions match your wiring

### Step 3: Code Check
- [ ] Upload HardwareTest.ino first
- [ ] Check Serial Monitor at 115200 baud
- [ ] Look for initialization messages

### Step 4: Display Issues
If display is blank:
- [ ] Try different rotation: `tft.setRotation(0);` to `tft.setRotation(3);`
- [ ] Reduce SPI frequency in User_Setup.h to 20000000
- [ ] Check if display is GC9A01 (not ST7789 or ILI9341)

## 📡 WiFi Not Connecting

### Step 1: Credentials
- [ ] SSID is correct (case-sensitive)
- [ ] Password is correct (case-sensitive)
- [ ] No extra spaces in config.h
- [ ] Quotes are present around SSID and password

### Step 2: Network
- [ ] WiFi is 2.4GHz (ESP8266 doesn't support 5GHz)
- [ ] WiFi network is active and reachable
- [ ] No MAC address filtering blocking ESP8266
- [ ] Router is within range

### Step 3: Code
- [ ] Check Serial Monitor for connection attempts
- [ ] Look for error messages
- [ ] Verify WiFi.begin() is called with correct credentials

## ⚙️ Compilation Errors

### "config.h: No such file or directory"
- [ ] Create config.h from config.h.example
- [ ] Place it in the same folder as the .ino file

### "TFT_eSPI.h: No such file or directory"
- [ ] Install TFT_eSPI library via Library Manager
- [ ] Restart Arduino IDE after installation

### "ArduinoJson.h: No such file or directory"
- [ ] Install ArduinoJson library via Library Manager
- [ ] Use version 6.x (not 5.x or 7.x)

### "ESP8266WiFi.h: No such file or directory"
- [ ] Install ESP8266 board support
- [ ] Restart Arduino IDE
- [ ] Verify board is selected

### Conflicting library errors
- [ ] Check for duplicate libraries in Arduino/libraries folder
- [ ] Remove old or conflicting versions

## 🌐 Data Not Updating

### Step 1: WiFi
- [ ] WiFi connected successfully (check Serial Monitor)
- [ ] "WiFi OK" shown on display (top left)

### Step 2: API
- [ ] Serial Monitor shows "Fetching ISS data..."
- [ ] HTTP response code is 200
- [ ] JSON parsing successful

### Step 3: Display
- [ ] Check if percentage value changes
- [ ] Look for error messages on display
- [ ] Verify update interval hasn't been set too long

## 🐛 Serial Monitor Issues

### No output in Serial Monitor
- [ ] Correct COM port selected
- [ ] Baud rate set to 115200
- [ ] Serial.begin(115200) in setup()
- [ ] USB cable supports data (not just power)

### Garbage characters
- [ ] Baud rate is 115200 (not 9600)
- [ ] Try different baud rates to find correct one

## ⚡ Random Resets/Crashes

### Power Issues
- [ ] USB cable is good quality
- [ ] USB port provides enough current
- [ ] No other high-current devices on same USB hub
- [ ] Try different USB port or power supply

### Code Issues
- [ ] Enough delay in loop() (at least 100ms)
- [ ] Not overwhelming Serial Monitor with prints
- [ ] Watchdog timer not timing out

## 🎨 Display Problems

### Colors are wrong
- [ ] Try uncommenting `#define TFT_RGB_ORDER TFT_BGR` in User_Setup.h
- [ ] Or try `#define TFT_RGB_ORDER TFT_RGB`

### Display upside down
- [ ] Change rotation: `tft.setRotation(0);` to `tft.setRotation(2);`

### Flickering
- [ ] Reduce update frequency
- [ ] Check power supply stability
- [ ] Reduce SPI frequency

### Partial display update
- [ ] Increase delay between updates
- [ ] Check if fillScreen() is called

## 📋 Systematic Debugging Process

1. **Start Simple**
   - [ ] Upload HardwareTest.ino
   - [ ] Verify display works with test patterns

2. **Add Complexity Gradually**
   - [ ] Test WiFi connection separately
   - [ ] Test HTTP request separately
   - [ ] Finally combine everything

3. **Use Serial Monitor**
   - [ ] Add Serial.print() statements
   - [ ] Track program flow
   - [ ] Identify where it fails

4. **Isolate the Problem**
   - [ ] Comment out sections of code
   - [ ] Enable one feature at a time
   - [ ] Find exactly what causes the issue

## 🆘 Getting Help

If you've tried everything:

1. **Gather Information**
   - [ ] Note exact error message
   - [ ] Copy Serial Monitor output
   - [ ] List hardware versions
   - [ ] Note library versions

2. **Check Documentation**
   - [ ] README.md troubleshooting section
   - [ ] TFT_eSPI_SETUP.md
   - [ ] LIBRARIES.md

3. **Search Online**
   - [ ] Search error message on Google
   - [ ] Check TFT_eSPI GitHub issues
   - [ ] Check ESP8266 Arduino forums

4. **Ask for Help**
   - [ ] Post issue on GitHub with full details
   - [ ] Include error messages and Serial output
   - [ ] Describe what you've tried

## ✅ Success Indicators

You know it's working when:

- [ ] Serial Monitor shows "WiFi connected!"
- [ ] Serial Monitor shows "Fetching ISS data..."
- [ ] Serial Monitor shows "Urine Tank Percentage: X%"
- [ ] Display shows "ISS" title
- [ ] Display shows percentage number
- [ ] Display shows "WiFi OK" in corner
- [ ] Percentage updates every minute

## 🔄 Reset to Defaults

If all else fails:

1. **Clean Installation**
   - [ ] Uninstall all libraries
   - [ ] Delete Arduino/libraries folder contents
   - [ ] Reinstall libraries one by one
   - [ ] Follow QUICKSTART.md exactly

2. **Hardware Reset**
   - [ ] Disconnect all wires
   - [ ] Reconnect following QUICKSTART.md wiring diagram
   - [ ] Double-check each connection

3. **Test Hardware**
   - [ ] Test Wemos D1 Mini with simple Blink sketch
   - [ ] Test display with HardwareTest.ino
   - [ ] Verify each component works individually

---

**Remember:** Most issues are simple wiring mistakes or library configuration! Work through the checklist systematically.
