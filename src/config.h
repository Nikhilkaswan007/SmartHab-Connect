/*
 * config.h - Configuration file for ESP32 IoT Dashboard
 * 
 * Copy this file from config_template.h and update with your actual credentials
 * Add config.h to your .gitignore to keep credentials secure
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// WiFi Configuration
// ============================================================================
const char* ssid = "YOUR_WIFI_SSID";           // Replace with your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD";   // Replace with your WiFi password

// ============================================================================
// YouTube API Configuration
// ============================================================================
// Get your API key from: https://console.developers.google.com/
// Enable YouTube Data API v3 for your project
const char* apiKey = "YOUR_YOUTUBE_API_KEY";   // Replace with your YouTube API key
const char* channelId = "YOUR_CHANNEL_ID";     // Replace with your channel ID

// ============================================================================
// Hardware Configuration (Optional - modify if needed)
// ============================================================================
// Bluetooth device name
const char* bluetoothDeviceName = "ESP32_Dashboard";

// Sensor calibration values
const float TEMP_OFFSET = 0.0;         // Temperature sensor offset correction
const float HUMIDITY_OFFSET = 0.0;     // Humidity sensor offset correction
const int SOIL_DRY_VALUE = 4095;       // Sensor value when completely dry
const int SOIL_WET_VALUE = 0;          // Sensor value when completely wet

// Display settings
const int LCD_BACKLIGHT_TIMEOUT = 30000; // Auto-off after 30 seconds (0 = always on)
const int MENU_TIMEOUT = 60000;          // Return to main menu after 60 seconds

// Network settings
const int WIFI_TIMEOUT = 15000;         // WiFi connection timeout in milliseconds
const int API_REQUEST_TIMEOUT = 10000;  // API request timeout in milliseconds

#endif // CONFIG_H
