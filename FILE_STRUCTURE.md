# Project File Structure

This document explains the purpose of each file in the PPISS project.

## Arduino Sketches

### PPISS_TFT_eSPI.ino ⭐ RECOMMENDED
The main project sketch using the TFT_eSPI library.
- **Use this if:** You want the easiest setup experience
- **Requirements:** TFT_eSPI library (needs configuration)
- **Pros:** Well-supported, fast, lots of examples online
- **Cons:** Requires editing User_Setup.h file

### PPISS.ino
Alternative version using Arduino_GFX library.
- **Use this if:** You prefer Arduino_GFX or have issues with TFT_eSPI
- **Requirements:** Arduino_GFX library
- **Pros:** No configuration file editing needed
- **Cons:** Less common, may need manual installation

### HardwareTest.ino
Simple test sketch to verify your display is working.
- **Purpose:** Hardware validation before running main project
- **What it does:** Shows colorful graphics and test patterns
- **When to use:** First upload, or when troubleshooting display issues

## Configuration Files

### config.h.example
Template for WiFi configuration.
- **What to do:** Copy this to `config.h` and add your WiFi credentials
- **Contains:** WiFi SSID and password placeholders
- **Never commit:** config.h with real credentials!

### config.h
Your actual WiFi credentials (YOU CREATE THIS).
- **How to create:** Copy from config.h.example
- **Protected:** Listed in .gitignore so it won't be committed
- **Required:** Project won't compile without this file

## Documentation Files

### README.md
Main project documentation.
- **Contains:** 
  - Project overview
  - Hardware requirements
  - Software setup
  - Installation instructions
  - Troubleshooting
  - Customization options

### QUICKSTART.md ⭐ START HERE
Step-by-step quick start guide.
- **Best for:** First-time users
- **Timeline:** Get running in ~13 minutes
- **Contains:** Simplified setup instructions with checklist

### TFT_eSPI_SETUP.md
Detailed TFT_eSPI library configuration guide.
- **Essential for:** Users of PPISS_TFT_eSPI.ino
- **Contains:** 
  - How to configure User_Setup.h
  - Pin definitions
  - Troubleshooting display issues
  - Test sketch

### LIBRARIES.md
Complete library installation guide.
- **Contains:**
  - Required libraries list
  - Installation methods
  - Alternative libraries
  - Library sizes and versions

### FILE_STRUCTURE.md
This file! Explains what each file does.

## Project Support Files

### .gitignore
Specifies files that should not be committed to git.
- **Protects:** config.h (WiFi credentials)
- **Excludes:** Build artifacts, IDE files

### LICENSE
MIT License for the project.
- **Type:** Open source, permissive
- **Allows:** Free use, modification, distribution

## How to Use These Files

### First Time Setup
1. Read QUICKSTART.md
2. Wire hardware (see QUICKSTART.md wiring diagram)
3. Install libraries (see LIBRARIES.md)
4. Configure TFT_eSPI (see TFT_eSPI_SETUP.md)
5. Create config.h from config.h.example
6. Test with HardwareTest.ino
7. Upload PPISS_TFT_eSPI.ino

### If You Have Problems
1. Check README.md troubleshooting section
2. Verify wiring in QUICKSTART.md
3. Re-check TFT_eSPI_SETUP.md
4. Try HardwareTest.ino to isolate issues
5. Review LIBRARIES.md for installation issues

### For Development
- Modify PPISS_TFT_eSPI.ino or PPISS.ino
- Keep config.h local (never commit it!)
- Test with HardwareTest.ino after hardware changes
- Update documentation if you add features

## File Dependency Tree

```
Project Root
├── QUICKSTART.md          ← START HERE
├── HardwareTest.ino       ← Test first
├── config.h.example       ← Copy to config.h
├── config.h               ← YOU CREATE (from example)
│
├── PPISS_TFT_eSPI.ino    ← Main sketch (recommended)
│   ├── Requires: config.h
│   ├── Requires: TFT_eSPI library (configured)
│   └── See: TFT_eSPI_SETUP.md
│
├── PPISS.ino              ← Alternative sketch
│   ├── Requires: config.h
│   └── Requires: Arduino_GFX library
│
├── TFT_eSPI_SETUP.md      ← Library configuration
├── LIBRARIES.md           ← Library installation
├── README.md              ← Full documentation
└── FILE_STRUCTURE.md      ← This file
```

## Quick Reference

| Task | File to Check |
|------|---------------|
| Getting started | QUICKSTART.md |
| WiFi setup | config.h.example → config.h |
| Display not working | TFT_eSPI_SETUP.md, HardwareTest.ino |
| Library errors | LIBRARIES.md |
| Customization | README.md (Customization section) |
| Troubleshooting | README.md (Troubleshooting section) |
| Understanding files | FILE_STRUCTURE.md (this file) |

## File Sizes (Approximate)

- Arduino sketches: 3-8 KB each
- Documentation: 3-6 KB each
- Total project: ~30 KB
- No binary files or images

All files are text-based and can be viewed/edited in any text editor.
