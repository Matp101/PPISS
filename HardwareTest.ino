/*
 * Hardware Test Sketch for PPISS Project
 * 
 * This simple test verifies that your GC9A01 display is wired correctly
 * and the TFT_eSPI library is properly configured.
 * 
 * Upload this sketch FIRST before trying the full PPISS project.
 * 
 * Expected Result:
 * - Black background
 * - White "PPISS" text at top
 * - Colored circles
 * - Rainbow gradient
 * - Text showing "Test OK!"
 * 
 * If you see this, your hardware is working!
 */

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n=== PPISS Hardware Test ===");
  Serial.println("Initializing display...");
  
  // Initialize display
  tft.init();
  tft.setRotation(0);  // Portrait mode
  
  Serial.println("Display initialized");
  Serial.println("Running tests...");
  
  // Test 1: Fill screen with colors
  testColorFill();
  delay(2000);
  
  // Test 2: Draw shapes
  testShapes();
  delay(2000);
  
  // Test 3: Draw text
  testText();
  delay(2000);
  
  // Test 4: Final display
  testFinal();
  
  Serial.println("=== Test Complete ===");
  Serial.println("If you see colored graphics on the display, hardware is OK!");
  Serial.println("If display is blank, check wiring and TFT_eSPI configuration.");
}

void loop() {
  // Animation - rotating colors
  static int hue = 0;
  
  // Draw a small animated circle
  int x = 120 + (int)(50 * cos(millis() / 1000.0));
  int y = 120 + (int)(50 * sin(millis() / 1000.0));
  
  tft.fillCircle(x, y, 5, tft.color565(hue, 255, 255));
  
  hue = (hue + 1) % 256;
  delay(50);
}

void testColorFill() {
  Serial.println("Test 1: Color Fill");
  
  tft.fillScreen(TFT_BLACK);
  delay(500);
  
  tft.fillScreen(TFT_RED);
  delay(500);
  
  tft.fillScreen(TFT_GREEN);
  delay(500);
  
  tft.fillScreen(TFT_BLUE);
  delay(500);
  
  tft.fillScreen(TFT_BLACK);
}

void testShapes() {
  Serial.println("Test 2: Shapes");
  
  tft.fillScreen(TFT_BLACK);
  
  // Draw circles
  tft.drawCircle(120, 120, 70, TFT_RED);
  delay(200);
  tft.drawCircle(120, 120, 60, TFT_GREEN);
  delay(200);
  tft.drawCircle(120, 120, 50, TFT_BLUE);
  delay(200);
  tft.drawCircle(120, 120, 40, TFT_YELLOW);
  delay(200);
  tft.drawCircle(120, 120, 30, TFT_CYAN);
  delay(200);
  tft.drawCircle(120, 120, 20, TFT_MAGENTA);
  delay(200);
  
  // Fill center
  tft.fillCircle(120, 120, 10, TFT_WHITE);
}

void testText() {
  Serial.println("Test 3: Text");
  
  tft.fillScreen(TFT_BLACK);
  
  // Different text sizes
  tft.setTextColor(TFT_WHITE);
  
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.print("Size 1: PPISS Test");
  
  tft.setTextSize(2);
  tft.setCursor(10, 30);
  tft.print("Size 2");
  
  tft.setTextSize(3);
  tft.setCursor(10, 60);
  tft.print("Size 3");
  
  // Colored text
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED);
  tft.setCursor(10, 110);
  tft.print("RED");
  
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 135);
  tft.print("GREEN");
  
  tft.setTextColor(TFT_BLUE);
  tft.setCursor(10, 160);
  tft.print("BLUE");
  
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 185);
  tft.print("YELLOW");
}

void testFinal() {
  Serial.println("Test 4: Final Display");
  
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(50, 30);
  tft.print("PPISS");
  
  // Circle indicator
  tft.drawCircle(120, 130, 60, TFT_CYAN);
  tft.drawCircle(120, 130, 59, TFT_CYAN);
  
  // Check mark
  tft.fillCircle(120, 130, 40, TFT_GREEN);
  
  // Success text
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(45, 200);
  tft.print("Test OK!");
  
  // Info text
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(20, 220);
  tft.print("Hardware Working!");
}
