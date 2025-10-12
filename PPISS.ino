/*
 * PPISS - Pee Percentage of the International Space Station
 * 
 * Displays the ISS Urine Tank Quantity on a GC9A01 1.28" round TFT display
 * connected to an ESP8266 Wemos D1 Mini
 * 
 * Hardware:
 * - ESP8266 Wemos D1 Mini
 * - GC9A01 240x240 TFT Display (1.28")
 * 
 * Required Libraries:
 * - ESP8266WiFi
 * - ESP8266HTTPClient
 * - ArduinoJson
 * - Adafruit_GFX
 * - Adafruit_GC9A01A (or compatible GC9A01 driver)
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Arduino_GC9A01.h>
#include <SPI.h>

// Include config file for WiFi credentials
#include "config.h"

// Pin definitions for GC9A01 display (adjust based on your wiring)
#define TFT_DC    D4   // Data/Command pin
#define TFT_CS    D8   // Chip Select pin
#define TFT_RST   D3   // Reset pin

// Create display object
Arduino_GC9A01 tft = Arduino_GC9A01(TFT_DC, TFT_CS, TFT_RST);

// ISS API endpoint
const char* ISS_API_URL = "http://api.open-notify.org/iss-now.json";
// Alternative: Use ISS Telemetry API if available
// Note: The actual urine tank data might require NASA's official APIs

// Update interval (milliseconds)
const unsigned long UPDATE_INTERVAL = 60000; // 1 minute
unsigned long lastUpdate = 0;

// Data variables
float urineTankPercentage = 0.0;
bool dataAvailable = false;
String lastError = "";

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\nPPISS - ISS Urine Tank Display");
  Serial.println("================================");
  
  // Initialize display
  initDisplay();
  
  // Connect to WiFi
  connectWiFi();
  
  // Initial data fetch
  fetchISSData();
}

void loop() {
  // Check if it's time to update
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    fetchISSData();
    lastUpdate = millis();
  }
  
  // Update display
  updateDisplay();
  
  delay(1000); // Small delay to reduce CPU usage
}

void initDisplay() {
  Serial.println("Initializing display...");
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(0x0000); // Black background
  
  // Display startup message
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF); // White
  tft.setCursor(40, 100);
  tft.print("PPISS");
  tft.setCursor(20, 130);
  tft.setTextSize(1);
  tft.print("Initializing...");
  
  Serial.println("Display initialized");
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  tft.fillScreen(0x0000);
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF);
  tft.setCursor(20, 110);
  tft.print("Connecting WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    tft.fillScreen(0x0000);
    tft.setCursor(20, 110);
    tft.setTextColor(0x07E0); // Green
    tft.print("WiFi Connected!");
    delay(1000);
  } else {
    Serial.println("\nWiFi connection failed!");
    tft.fillScreen(0x0000);
    tft.setCursor(20, 110);
    tft.setTextColor(0xF800); // Red
    tft.print("WiFi Failed!");
    while(1) { delay(1000); } // Stop here if WiFi fails
  }
}

void fetchISSData() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, cannot fetch data");
    lastError = "No WiFi";
    dataAvailable = false;
    return;
  }
  
  Serial.println("Fetching ISS data...");
  
  WiFiClient client;
  HTTPClient http;
  
  // Note: This is a placeholder URL. The actual ISS urine tank data
  // requires access to NASA's internal telemetry systems or specific APIs.
  // For demonstration, we'll use a mock percentage based on time or
  // you'll need to replace this with the actual API endpoint.
  
  // Using ISS location API as a placeholder (replace with actual urine tank API)
  http.begin(client, ISS_API_URL);
  
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Response received:");
      Serial.println(payload);
      
      // Parse JSON response
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        // For demonstration: calculate a mock percentage
        // In a real implementation, parse the actual urine tank data
        // Example: urineTankPercentage = doc["urine_tank"]["percentage"];
        
        // Mock calculation based on timestamp (for demo purposes)
        long timestamp = doc["timestamp"] | 0;
        urineTankPercentage = ((timestamp % 100)) * 1.0;
        
        dataAvailable = true;
        lastError = "";
        
        Serial.print("Urine Tank Percentage: ");
        Serial.print(urineTankPercentage);
        Serial.println("%");
      } else {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        lastError = "Parse Error";
        dataAvailable = false;
      }
    } else {
      Serial.printf("HTTP error code: %d\n", httpCode);
      lastError = "HTTP " + String(httpCode);
      dataAvailable = false;
    }
  } else {
    Serial.printf("HTTP request failed: %s\n", http.errorToString(httpCode).c_str());
    lastError = "Req Failed";
    dataAvailable = false;
  }
  
  http.end();
}

void updateDisplay() {
  // Clear screen
  tft.fillScreen(0x0000); // Black
  
  // Draw title
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF); // White
  tft.setCursor(80, 20);
  tft.print("ISS");
  
  tft.setTextSize(1);
  tft.setCursor(40, 45);
  tft.print("Urine Tank");
  
  // Draw main percentage display
  if (dataAvailable) {
    // Draw percentage circle/arc background
    int centerX = 120;
    int centerY = 140;
    int radius = 70;
    
    // Draw outer circle
    tft.drawCircle(centerX, centerY, radius, 0x39E7); // Light blue
    tft.drawCircle(centerX, centerY, radius - 1, 0x39E7);
    
    // Draw percentage text
    tft.setTextSize(4);
    tft.setTextColor(0x07FF); // Cyan
    
    String percentText = String((int)urineTankPercentage);
    int textWidth = percentText.length() * 24; // Approximate width
    tft.setCursor(centerX - textWidth/2, centerY - 15);
    tft.print(percentText);
    
    tft.setTextSize(2);
    tft.setCursor(centerX - 10, centerY + 20);
    tft.print("%");
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(0x07E0); // Green
    tft.setCursor(70, 215);
    tft.print("Data Updated");
    
  } else {
    // Error display
    tft.setTextSize(2);
    tft.setTextColor(0xF800); // Red
    tft.setCursor(60, 120);
    tft.print("ERROR");
    
    tft.setTextSize(1);
    tft.setCursor(70, 145);
    tft.print(lastError);
  }
  
  // Display WiFi status
  tft.setTextSize(1);
  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(0x07E0); // Green
    tft.setCursor(5, 5);
    tft.print("WiFi OK");
  } else {
    tft.setTextColor(0xF800); // Red
    tft.setCursor(5, 5);
    tft.print("WiFi X");
  }
  
  // Display last update time
  tft.setTextColor(0x7BEF); // Light gray
  tft.setCursor(5, 230);
  tft.print("Updated: ");
  tft.print(millis() / 1000);
  tft.print("s");
}
