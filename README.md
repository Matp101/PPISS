# PPISS — Pee Percentage of the International Space Station

A tiny desk gadget that shows **real-time ISS waste-tank levels** on a round LCD. Built with a Wemos D1 Mini (ESP8266) and a GC9A01 240×240 TFT, housed in a 3D-printed enclosure.

The data comes straight from NASA's public [ISS Live telemetry](https://demos.lightstreamer.com/ISSLive/) feed via the Lightstreamer protocol.

---

## Features

- **Live telemetry** — polls NASA's Lightstreamer server for urine tank, waste water, and clean water percentages
- **Page-cycling display** — each tank gets its own screen, rotating every 5 seconds
- **Multi-SSID WiFi** — configure multiple networks in `secrets.h`; the device cycles through them automatically
- **3D-printable case** — top and bottom STL files included

## Hardware

| Part | Notes |
|------|-------|
| **Wemos D1 Mini** | ESP8266-based, any clone works |
| **GC9A01 240×240 round TFT** | SPI, 1.28″ |

### Wiring

| GC9A01 Pin | D1 Mini Pin | GPIO |
|------------|-------------|------|
| CS | D8 | GPIO15 |
| DC | D3 | GPIO0 |
| MOSI | D7 | GPIO13 |
| SCLK | D5 | GPIO14 |
| RST | NC | Software reset |
| BL | VCC | Always on |

## Enclosure

The `models/` folder contains the 3D-printable enclosure:

| File | Description |
|------|-------------|
| `PPISS.skp` | Master SketchUp source file |
| `PPISS_top.stl` | Top half of the case |
| `PPISS_bottom.stl` | Bottom half of the case |

## Project Structure

```
├── platformio.ini          PlatformIO config (ESP8266 RTOS SDK)
├── models/                 3D-printable enclosure files
│   ├── PPISS.skp
│   ├── PPISS_top.stl
│   └── PPISS_bottom.stl
└── app/
    ├── main.cpp            Entry point — creates all FreeRTOS tasks
    └── src/
        ├── secrets.h           WiFi credentials (git-ignored)
        ├── secrets.h.example   Template for secrets.h
        ├── blink/              Heartbeat LED task
        ├── wifi/               WiFi station management (multi-SSID)
        ├── iss/                Lightstreamer client + telemetry data
        └── display/
            ├── ui.c            Page-cycling UI task
            ├── gfx.h           Drawing primitives
            ├── font.h          Bitmap font
            ├── driver/         GC9A01 SPI driver (bit-banged)
            └── icons/          Tank and WiFi status icons
```

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)

### Setup

1. **Clone the repo**
   ```
   git clone https://github.com/Matp101/PPISS.git
   cd PPISS
   ```

2. **Configure WiFi** — copy the example and fill in your credentials:
   ```
   cp app/src/secrets.h.example app/src/secrets.h
   ```
   Edit `secrets.h` and add one or more SSIDs:
   ```c
   static const wifi_cred_t WIFI_CREDS[] = {
       { "MyNetwork",     "MyPassword"     },
       { "BackupNetwork", "BackupPassword"  },
   };
   ```

3. **Build & upload**
   ```
   pio run -t upload -t monitor
   ```

### Nix / NixOS

If you're on NixOS or have Nix with flakes enabled, you can build and develop without installing PlatformIO globally.

> **Prerequisites:** [Nix](https://nixos.org/download/) with `nix-command` and `flakes` enabled.
> Add `experimental-features = nix-command flakes` to `~/.config/nix/nix.conf` if you haven't already.

**Build the firmware:**
```bash
nix run .
```
The firmware is output to `.pio/build/d1_mini/firmware.bin`.

**Upload to board:**
```bash
nix run .#upload
```

**Serial monitor:**
```bash
nix run .#monitor
```

**Interactive dev shell**
```bash
nix develop
```
Then use PlatformIO commands directly:
```bash
pio run
pio run -t upload
pio device monitor
```

> **Note:** On NixOS, you may need to add udev rules for USB upload access:
> ```nix
> services.udev.packages = [ pkgs.platformio-core pkgs.openocd ];
> ```
> Or add your user to the `dialout` group.

## Telemetry Items

The device tracks these ISS parameters (defined in `app/src/iss/iss.c`):

| Item ID | Label | Description |
|---------|-------|-------------|
| `NODE3000005` | Urine Tank | Urine tank fill percentage |
| `NODE3000008` | Waste Water Tank | Waste water tank fill percentage |
| `NODE3000009` | Clean Water Tank | Clean water tank fill percentage |

To add more items, append to the `g_telem[]` array in `iss.c`.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgements

- Inspired by [pISSStream](https://github.com/Jaennaet/pISSStream)
- [NASA ISS Live](https://demos.lightstreamer.com/ISSLive/) for the public telemetry data
- [Lightstreamer](https://lightstreamer.com/) for the streaming protocol