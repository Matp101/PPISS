/*
 * PPISS - Pee Percentage of the International Space Station
 * Alternative version using TFT_eSPI library
 * 
 * This version uses TFT_eSPI which is more widely supported and easier to configure
 * 
 * Hardware:
 * - ESP8266 Wemos D1 Mini
 * - GC9A01 240x240 TFT Display (1.28")
 * 
 * Required Libraries:
 * - ESP8266WiFi
 * - ESP8266HTTPClient
 * - ArduinoJson
 * - TFT_eSPI (with GC9A01 configuration)
 * 
 * TFT_eSPI Configuration:
 * Edit the User_Setup.h file in the TFT_eSPI library folder with these settings:
 * 
 * #define GC9A01_DRIVER
 * #define TFT_WIDTH  240
 * #define TFT_HEIGHT 240
 * #define TFT_MISO -1
 * #define TFT_MOSI D7
 * #define TFT_SCLK D5
 * #define TFT_CS   D8
 * #define TFT_DC   D4
 * #define TFT_RST  D3
 * #define LOAD_GLCD
 * #define LOAD_FONT2
 * #define LOAD_FONT4
 * #define LOAD_FONT6
 * #define LOAD_FONT7
 * #define LOAD_FONT8
 * #define SPI_FREQUENCY  40000000
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>

// Include config file for WiFi credentials
#include "config.h"

// Create display object
TFT_eSPI tft = TFT_eSPI();

// ISS API endpoint
const char* ISS_API_URL = "http://api.open-notify.org/iss-now.json";

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
  lastUpdate = millis();
}

void loop() {
  // Check if it's time to update
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    fetchISSData();
    lastUpdate = millis();
  }
  
  // Update display
  updateDisplay();
  
  delay(5000); // Update display every 5 seconds
}

void initDisplay() {
  Serial.println("Initializing display...");
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Display startup message
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(80, 100);
  tft.print("PPISS");
  tft.setCursor(40, 130);
  tft.setTextSize(1);
  tft.print("Initializing...");
  
  Serial.println("Display initialized");
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(40, 110);
  tft.print("Connecting WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Update display with dots
    tft.setCursor(40 + (attempts % 20) * 8, 130);
    tft.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(50, 110);
    tft.setTextColor(TFT_GREEN);
    tft.print("WiFi Connected!");
    delay(2000);
  } else {
    Serial.println("\nWiFi connection failed!");
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(60, 110);
    tft.setTextColor(TFT_RED);
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
  
  http.begin(client, ISS_API_URL);
  http.setTimeout(10000); // 10 second timeout
  
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Response received");
      
      // Parse JSON response
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        // Mock calculation based on timestamp (for demo purposes)
        // Replace with actual API parsing when real ISS data is available
        long timestamp = doc["timestamp"] | 0;
        urineTankPercentage = (timestamp % 100) * 1.0;
        
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
  tft.fillScreen(TFT_BLACK);
  
  // Draw title
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(90, 20);
  tft.print("ISS");
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(60, 45);
  tft.print("Urine Tank");
  
  // Draw main percentage display
  if (dataAvailable) {
    int centerX = 120;
    int centerY = 130;
    int radius = 70;
    
    // Draw outer circle
    tft.drawCircle(centerX, centerY, radius, TFT_BLUE);
    tft.drawCircle(centerX, centerY, radius - 1, TFT_BLUE);
    
    // Draw filled arc based on percentage (simple visualization)
    // For a more sophisticated arc, you'd need to draw segments
    uint16_t color = TFT_GREEN;
    if (urineTankPercentage > 75) {
      color = TFT_RED;
    } else if (urineTankPercentage > 50) {
      color = TFT_YELLOW;
    }
    
    // Draw inner filled circle for percentage indicator
    int fillRadius = (int)(radius * 0.6);
    tft.fillCircle(centerX, centerY, fillRadius, color);
    
    // Draw percentage text
    tft.setTextSize(4);
    tft.setTextColor(TFT_BLACK);
    
    String percentText = String((int)urineTankPercentage);
    int textX = centerX - (percentText.length() * 12);
    tft.setCursor(textX, centerY - 15);
    tft.print(percentText);
    
    tft.setTextSize(2);
    tft.setCursor(centerX + 5, centerY + 20);
    tft.print("%");
    
    // Status text
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(70, 210);
    tft.print("Data Updated");
    
  } else {
    // Error display
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);
    tft.setCursor(70, 120);
    tft.print("ERROR");
    
    tft.setTextSize(1);
    tft.setCursor(80, 145);
    tft.print(lastError);
  }
  
  // Display WiFi status
  tft.setTextSize(1);
  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(5, 5);
    tft.print("WiFi");
  } else {
    tft.setTextColor(TFT_RED);
    tft.setCursor(5, 5);
    tft.print("No WiFi");
  }
  
  // Display last update time
  tft.setTextColor(TFT_DARKGREY);
  tft.setCursor(150, 230);
  unsigned long seconds = millis() / 1000;
  unsigned long minutes = seconds / 60;
  tft.print(minutes);
  tft.print("m");
}
