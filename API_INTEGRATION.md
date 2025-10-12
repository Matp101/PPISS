# ISS API Integration Guide

This document explains how to integrate real ISS telemetry data into the PPISS project.

## Current Implementation

The project currently uses a **mock data approach** for demonstration:
- Fetches from `http://api.open-notify.org/iss-now.json`
- Calculates a fake percentage based on timestamp: `(timestamp % 100)`
- This allows the project to work without real ISS data

## Why Mock Data?

Real ISS urine tank telemetry is not publicly available through simple APIs. NASA's internal systems track this data, but it's not exposed through common public APIs.

## Potential Real Data Sources

### 1. NASA Open APIs
- **URL:** https://api.nasa.gov/
- **Pros:** Official NASA data
- **Cons:** May not include urine tank specifics
- **API Key:** Free, requires registration

### 2. ISS Live! Data
- **URL:** https://www.lightstreamer.com/demos/iss-live/
- **Type:** Real-time position and limited telemetry
- **Pros:** Live data, well-documented
- **Cons:** Limited to public telemetry

### 3. ISS Mimic
- **GitHub:** https://github.com/ISS-Mimic/Mimic
- **Type:** Simulation and real telemetry interface
- **Pros:** Designed for ISS data access
- **Cons:** May require setup of local server

### 4. ISS APIs (Third-party)
- **Where-the-ISS-at:** https://wheretheiss.at/w/developer
- **ISS Notify:** http://open-notify.org/
- **Pros:** Easy to use
- **Cons:** Limited data (position, crew, etc.)

## Integration Steps

### Step 1: Find the API

Research and identify an API that provides the data you need. For ISS urine tank:
1. Check NASA's official APIs
2. Look for space station telemetry services
3. Consider simulated/historical data sources

### Step 2: Understand the API Response

Example API response structure:
```json
{
  "timestamp": 1234567890,
  "iss": {
    "telemetry": {
      "water_systems": {
        "urine_tank": {
          "percentage": 42.5,
          "capacity_liters": 100,
          "current_liters": 42.5
        }
      }
    }
  }
}
```

### Step 3: Update the Code

Modify `PPISS_TFT_eSPI.ino`:

#### 3a. Update API URL
```cpp
// Change this line (around line 29)
const char* ISS_API_URL = "http://api.open-notify.org/iss-now.json";

// To your new API URL
const char* ISS_API_URL = "https://your-api-here.com/iss/telemetry";
```

#### 3b. Add API Key (if required)
```cpp
// Add after ISS_API_URL definition
const char* API_KEY = "your_api_key_here";
```

#### 3c. Modify HTTP Request
```cpp
// In fetchISSData() function, add headers if needed
http.begin(client, ISS_API_URL);
http.addHeader("Authorization", String("Bearer ") + API_KEY);  // If needed
http.addHeader("Content-Type", "application/json");           // If needed

int httpCode = http.GET();
```

#### 3d. Update JSON Parsing
```cpp
// In fetchISSData() function, replace this section:
if (!error) {
  // OLD CODE (mock data):
  long timestamp = doc["timestamp"] | 0;
  urineTankPercentage = (timestamp % 100) * 1.0;
  
  // NEW CODE (real data):
  // Adjust path based on actual API response structure
  urineTankPercentage = doc["iss"]["telemetry"]["water_systems"]["urine_tank"]["percentage"];
  
  dataAvailable = true;
  lastError = "";
  
  Serial.print("Urine Tank Percentage: ");
  Serial.print(urineTankPercentage);
  Serial.println("%");
}
```

### Step 4: Handle HTTPS (if needed)

If the API uses HTTPS, update the WiFi client:

```cpp
// Add at top of file
#include <WiFiClientSecure.h>

// Replace WiFiClient with WiFiClientSecure
WiFiClientSecure client;

// In fetchISSData() function, before http.begin():
client.setInsecure(); // For testing only! 

// For production, use certificate:
// client.setCACert(root_ca);
```

### Step 5: Test and Debug

1. Upload modified code
2. Check Serial Monitor for:
   - API response
   - Parsed values
   - Any errors
3. Verify data on display

## Example: Using NASA APIs

Here's a complete example using hypothetical NASA API:

```cpp
// Configuration
const char* ISS_API_URL = "https://api.nasa.gov/iss/telemetry";
const char* NASA_API_KEY = "YOUR_NASA_API_KEY";

void fetchISSData() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    lastError = "No WiFi";
    dataAvailable = false;
    return;
  }
  
  Serial.println("Fetching ISS data from NASA...");
  
  WiFiClientSecure client;
  client.setInsecure(); // For testing
  
  HTTPClient http;
  
  // Build URL with API key
  String url = String(ISS_API_URL) + "?api_key=" + NASA_API_KEY;
  http.begin(client, url);
  
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Response:");
    Serial.println(payload);
    
    DynamicJsonDocument doc(2048); // Increase if response is large
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      // Parse based on actual API structure
      urineTankPercentage = doc["data"]["urine_tank_percentage"];
      
      // Validate data
      if (urineTankPercentage < 0 || urineTankPercentage > 100) {
        Serial.println("Invalid percentage value");
        lastError = "Bad Data";
        dataAvailable = false;
      } else {
        dataAvailable = true;
        lastError = "";
        Serial.printf("Urine Tank: %.1f%%\n", urineTankPercentage);
      }
    } else {
      Serial.printf("JSON error: %s\n", error.c_str());
      lastError = "Parse Error";
      dataAvailable = false;
    }
  } else {
    Serial.printf("HTTP error: %d\n", httpCode);
    lastError = "HTTP " + String(httpCode);
    dataAvailable = false;
  }
  
  http.end();
}
```

## Data Validation

Always validate data from external APIs:

```cpp
// After parsing
if (urineTankPercentage < 0 || urineTankPercentage > 100) {
  Serial.println("Invalid data range");
  dataAvailable = false;
  return;
}

// Check for null/missing values
if (!doc.containsKey("urine_tank_percentage")) {
  Serial.println("Missing required field");
  dataAvailable = false;
  return;
}
```

## Error Handling

Implement robust error handling:

```cpp
// Track consecutive errors
static int errorCount = 0;

if (httpCode != HTTP_CODE_OK) {
  errorCount++;
  if (errorCount > 5) {
    // After 5 failures, maybe restart or show warning
    Serial.println("Too many errors, check API");
  }
} else {
  errorCount = 0; // Reset on success
}
```

## Rate Limiting

Respect API rate limits:

```cpp
// Adjust update interval based on API limits
// Most free APIs: 1000 requests/day = ~1 request/90 seconds

const unsigned long UPDATE_INTERVAL = 120000; // 2 minutes (safe margin)
```

## Memory Considerations

ESP8266 has limited RAM. For large JSON responses:

```cpp
// Use StaticJsonDocument for known size
StaticJsonDocument<1024> doc;

// Or DynamicJsonDocument with appropriate size
DynamicJsonDocument doc(2048);

// Check memory usage
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

## Alternative: Using Multiple Data Sources

Fetch from multiple sources for redundancy:

```cpp
const char* PRIMARY_API = "https://api1.example.com/iss";
const char* FALLBACK_API = "https://api2.example.com/iss";

bool success = fetchFromAPI(PRIMARY_API);
if (!success) {
  Serial.println("Primary failed, trying fallback");
  success = fetchFromAPI(FALLBACK_API);
}
```

## Testing with Mock Server

For development, create a local mock server:

```python
# mock_server.py
from flask import Flask, jsonify
app = Flask(__name__)

@app.route('/iss/telemetry')
def telemetry():
    return jsonify({
        'timestamp': 1234567890,
        'urine_tank_percentage': 42.5
    })

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
```

Then use local IP in code:
```cpp
const char* ISS_API_URL = "http://192.168.1.100:5000/iss/telemetry";
```

## Next Steps

1. Research available ISS APIs
2. Test API endpoints using browser or Postman
3. Understand JSON response structure
4. Modify code as shown above
5. Test with Serial Monitor
6. Deploy and monitor

## Resources

- NASA APIs: https://api.nasa.gov/
- ArduinoJson Assistant: https://arduinojson.org/v6/assistant/
- ESP8266 HTTP Client: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266HTTPClient
- JSON Test: https://jsonlint.com/

## Contributing

If you successfully integrate a real ISS API:
1. Document the API source
2. Share your code modifications
3. Submit a pull request
4. Help others use real data!

---

**Note:** The ISS urine tank data may not be publicly available. This guide prepares you for when/if such data becomes accessible, or for integrating other ISS telemetry data.
