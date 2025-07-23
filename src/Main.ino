/*
 * ESP32 Multi-Sensor IoT Dashboard
 * 
 * A comprehensive IoT project featuring multiple sensors, LCD display,
 * WiFi connectivity, and Bluetooth audio capabilities.
 * 
 * Author: Nikhil
 * Version: 1.0
 * Date: 2025
 */

// ============================================================================
// INCLUDES
// ============================================================================
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> 
#include <DHTesp.h>
#include <HTTPClient.h>
#include <WiFi.h> 
#include <ezButton.h>
#include <YoutubeApi.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include "config.h"  // Contains sensitive credentials

// ============================================================================
// PIN DEFINITIONS
// ============================================================================
// Sensors
#define DHT_PIN           23
#define ULTRASONIC_TRIG   4
#define ULTRASONIC_ECHO   5
#define LDR_PIN           35
#define SOIL_MOISTURE_PIN 35
#define VOLTAGE_SENSOR_PIN 35

// Controls
#define ROTARY_CLK_PIN    19
#define ROTARY_DT_PIN     18
#define ROTARY_SW_PIN     17
#define JOYSTICK_X_PIN    34
#define JOYSTICK_Y_PIN    32
#define SELECT_BUTTON_PIN 14

// LCD
#define LCD_ADDRESS       0x27
#define LCD_COLS          16
#define LCD_ROWS          2

// ============================================================================
// CONSTANTS
// ============================================================================
#define NTP_SERVER        "pool.ntp.org"
#define UTC_OFFSET        19800  // India timezone offset in seconds
#define UTC_OFFSET_DST    0

#define VOLTAGE_DIVIDER_RATIO 11.0
#define CALIBRATION_FACTOR    0.5

#define JOYSTICK_THRESHOLD_LOW  50
#define JOYSTICK_THRESHOLD_HIGH 3800
#define JOYSTICK_NEUTRAL_LOW    1500
#define JOYSTICK_NEUTRAL_HIGH   2900

#define DIRECTION_CW      0
#define DIRECTION_CCW     1

// Menu items
enum MenuItems {
  MENU_TIME = 1,
  MENU_TEMP_HUM,
  MENU_YOUTUBE,
  MENU_BLUETOOTH,
  MENU_TIMER,
  MENU_ULTRASONIC,
  MENU_LDR,
  MENU_SOIL_MOISTURE,
  MENU_VOLTAGE,
  MENU_OTA,
  MENU_MAX
};

enum TimerMenuItems {
  TIMER_COUNTDOWN = 1,
  TIMER_STOPWATCH,
  TIMER_MAX
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
// Hardware objects
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
DHTesp dhtSensor;
AnalogAudioStream audioOut;
BluetoothA2DPSink a2dpSink(audioOut);

// Menu navigation
int currentMenu = 1;
int timerMenu = 1;
int rotaryCounter = 0;
int rotaryClkState;
int prevRotaryClkState;

// Timing variables
unsigned long previousMillis = 0;
const unsigned long updateInterval = 1000;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 IoT Dashboard Starting...");
  
  initializePins();
  initializeLCD();
  initializeSensors();
  
  Serial.println("Setup complete!");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  handleJoystickNavigation();
  handleButtonPress();
  delay(50); // Small delay to prevent excessive polling
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================
void initializePins() {
  // Input pins
  pinMode(ROTARY_CLK_PIN, INPUT);
  pinMode(ROTARY_DT_PIN, INPUT);
  pinMode(ROTARY_SW_PIN, INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(VOLTAGE_SENSOR_PIN, INPUT);
  
  // Output pins
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  
  // ADC configuration
  analogSetAttenuation(ADC_11db);
  
  // Read initial rotary encoder state
  prevRotaryClkState = digitalRead(ROTARY_CLK_PIN);
}

void initializeLCD() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IoT Dashboard");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  updateMainMenu();
}

void initializeSensors() {
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}

// ============================================================================
// NAVIGATION FUNCTIONS
// ============================================================================
void handleJoystickNavigation() {
  int joyXValue = analogRead(JOYSTICK_X_PIN);
  
  if (joyXValue < JOYSTICK_THRESHOLD_LOW) {
    currentMenu--;
    if (currentMenu < 1) currentMenu = MENU_MAX - 1;
    updateMainMenu();
    waitForJoystickRelease(JOYSTICK_X_PIN, JOYSTICK_NEUTRAL_LOW, true);
  }
  else if (joyXValue > JOYSTICK_THRESHOLD_HIGH) {
    currentMenu++;
    if (currentMenu >= MENU_MAX) currentMenu = 1;
    updateMainMenu();
    waitForJoystickRelease(JOYSTICK_X_PIN, JOYSTICK_NEUTRAL_HIGH, false);
  }
}

void handleButtonPress() {
  if (!digitalRead(SELECT_BUTTON_PIN)) {
    executeMenuAction();
    updateMainMenu();
    waitForButtonRelease(SELECT_BUTTON_PIN);
  }
}

void waitForJoystickRelease(int pin, int threshold, bool lessThan) {
  if (lessThan) {
    while (analogRead(pin) < threshold) delay(10);
  } else {
    while (analogRead(pin) > threshold) delay(10);
  }
  delay(200);
}

void waitForButtonRelease(int pin) {
  while (!digitalRead(pin)) delay(10);
  delay(200);
}

// ============================================================================
// MENU DISPLAY FUNCTIONS
// ============================================================================
void updateMainMenu() {
  lcd.clear();
  
  switch (currentMenu) {
    case MENU_TIME:
      lcd.print(">TIME");
      lcd.setCursor(0, 1);
      lcd.print(" HUM & TEMP");
      break;
    case MENU_TEMP_HUM:
      lcd.print(" TIME");
      lcd.setCursor(0, 1);
      lcd.print(">HUM & TEMP");
      break;
    case MENU_YOUTUBE:
      lcd.print(">YOUTUBE STATS");
      lcd.setCursor(0, 1);
      lcd.print(" BLUETOOTH");
      break;
    case MENU_BLUETOOTH:
      lcd.print(" YOUTUBE STATS");
      lcd.setCursor(0, 1);
      lcd.print(">BLUETOOTH");
      break;
    case MENU_TIMER:
      lcd.print(">TIMER");
      lcd.setCursor(0, 1);
      lcd.print(" ULTRASONIC");
      break;
    case MENU_ULTRASONIC:
      lcd.print(" TIMER");
      lcd.setCursor(0, 1);
      lcd.print(">ULTRASONIC");
      break;
    case MENU_LDR:
      lcd.print(">LIGHT SENSOR");
      lcd.setCursor(0, 1);
      lcd.print(" SOIL MOISTURE");
      break;
    case MENU_SOIL_MOISTURE:
      lcd.print(" LIGHT SENSOR");
      lcd.setCursor(0, 1);
      lcd.print(">SOIL MOISTURE");
      break;
    case MENU_VOLTAGE:
      lcd.print(">VOLTAGE");
      lcd.setCursor(0, 1);
      lcd.print(" OTA UPDATE");
      break;
    case MENU_OTA:
      lcd.print(" VOLTAGE");
      lcd.setCursor(0, 1);
      lcd.print(">OTA UPDATE");
      break;
  }
}

void updateTimerMenu() {
  lcd.clear();
  
  switch (timerMenu) {
    case TIMER_COUNTDOWN:
      lcd.print(">COUNTDOWN");
      lcd.setCursor(0, 1);
      lcd.print(" STOPWATCH");
      break;
    case TIMER_STOPWATCH:
      lcd.print(" COUNTDOWN");
      lcd.setCursor(0, 1);
      lcd.print(">STOPWATCH");
      break;
  }
}

// ============================================================================
// MENU ACTION FUNCTIONS
// ============================================================================
void executeMenuAction() {
  delay(100); // Debounce delay
  
  switch (currentMenu) {
    case MENU_TIME:
      showTimeDisplay();
      break;
    case MENU_TEMP_HUM:
      showTemperatureHumidity();
      break;
    case MENU_YOUTUBE:
      showYouTubeStats();
      break;
    case MENU_BLUETOOTH:
      activateBluetoothMode();
      break;
    case MENU_TIMER:
      showTimerMenu();
      break;
    case MENU_ULTRASONIC:
      showUltrasonicDistance();
      break;
    case MENU_LDR:
      showLightIntensity();
      break;
    case MENU_SOIL_MOISTURE:
      showSoilMoisture();
      break;
    case MENU_VOLTAGE:
      showVoltageReading();
      break;
    case MENU_OTA:
      initiateOTAMode();
      break;
  }
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================
void showTimeDisplay() {
  lcd.clear();
  
  if (!connectToWiFi()) {
    return;
  }
  
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    displayCurrentTime();
    delay(1000);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
  disconnectWiFi();
}

void showTemperatureHumidity() {
  lcd.clear();
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(data.temperature, 1);
    lcd.print(" C    ");
    
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(data.humidity, 0);
    lcd.print("%  ");
    
    delay(1000);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

void showYouTubeStats() {
  lcd.clear();
  
  if (!connectToWiFi()) {
    return;
  }
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    fetchYouTubeData();
    delay(5000); // Update every 5 seconds
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
  disconnectWiFi();
}

void showUltrasonicDistance() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    float distance = measureDistance();
    
    lcd.setCursor(0, 1);
    lcd.print(distance, 1);
    lcd.print(" cm      ");
    
    delay(200);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

void showLightIntensity() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light Level:");
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    int ldrValue = analogRead(LDR_PIN);
    int lightPercentage = map(ldrValue, 0, 4095, 0, 100);
    
    lcd.setCursor(0, 1);
    lcd.print(lightPercentage);
    lcd.print("% ");
    
    // Add descriptive text
    if (lightPercentage < 20) {
      lcd.print("Dark     ");
    } else if (lightPercentage < 40) {
      lcd.print("Dim      ");
    } else if (lightPercentage < 60) {
      lcd.print("Moderate ");
    } else if (lightPercentage < 80) {
      lcd.print("Bright   ");
    } else {
      lcd.print("Very Bright");
    }
    
    delay(500);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

void showSoilMoisture() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Moisture:");
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    int moistureRaw = analogRead(SOIL_MOISTURE_PIN);
    int moisturePercentage = map(moistureRaw, 0, 4095, 100, 0);
    
    lcd.setCursor(0, 1);
    lcd.print(moisturePercentage);
    lcd.print("%       ");
    
    delay(1000);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

void showVoltageReading() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voltage:");
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    int rawValue = analogRead(VOLTAGE_SENSOR_PIN);
    float voltage = (rawValue * 3.3 / 4095.0) * VOLTAGE_DIVIDER_RATIO * CALIBRATION_FACTOR;
    
    lcd.setCursor(0, 1);
    lcd.print(voltage, 2);
    lcd.print(" V     ");
    
    delay(500);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

// ============================================================================
// SPECIALIZED FUNCTIONS
// ============================================================================
void activateBluetoothMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32_Bluetooth");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  
  a2dpSink.start("ESP32_Bluetooth");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32_Bluetooth");
  lcd.setCursor(0, 1);
  lcd.print("Ready");
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    handleBluetoothControls();
    delay(100);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bluetooth");
  lcd.setCursor(0, 1);
  lcd.print("Stopping...");
  
  a2dpSink.end();
  delay(2000);
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

void handleBluetoothControls() {
  int joyYValue = analogRead(JOYSTICK_Y_PIN);
  
  if (joyYValue < JOYSTICK_THRESHOLD_LOW) {
    a2dpSink.next();
    waitForJoystickRelease(JOYSTICK_Y_PIN, JOYSTICK_NEUTRAL_LOW, true);
  }
  else if (joyYValue > JOYSTICK_THRESHOLD_HIGH) {
    a2dpSink.previous();
    waitForJoystickRelease(JOYSTICK_Y_PIN, JOYSTICK_NEUTRAL_HIGH, false);
  }
}

void showTimerMenu() {
  updateTimerMenu();
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    handleTimerMenuNavigation();
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
}

void handleTimerMenuNavigation() {
  int joyXValue = analogRead(JOYSTICK_X_PIN);
  
  if (joyXValue < JOYSTICK_THRESHOLD_LOW) {
    timerMenu--;
    if (timerMenu < 1) timerMenu = TIMER_MAX - 1;
    updateTimerMenu();
    waitForJoystickRelease(JOYSTICK_X_PIN, JOYSTICK_NEUTRAL_LOW, true);
  }
  else if (joyXValue > JOYSTICK_THRESHOLD_HIGH) {
    timerMenu++;
    if (timerMenu >= TIMER_MAX) timerMenu = 1;
    updateTimerMenu();
    waitForJoystickRelease(JOYSTICK_X_PIN, JOYSTICK_NEUTRAL_HIGH, false);
  }
  
  if (!digitalRead(SELECT_BUTTON_PIN)) {
    if (timerMenu == TIMER_COUNTDOWN) {
      runCountdownTimer();
    } else if (timerMenu == TIMER_STOPWATCH) {
      runStopwatch();
    }
    updateTimerMenu();
    waitForButtonRelease(SELECT_BUTTON_PIN);
  }
}

void runCountdownTimer() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Timer:");
  lcd.setCursor(0, 1);
  lcd.print(rotaryCounter);
  lcd.print(" sec");
  
  // Timer setting phase
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    handleRotaryEncoder();
    
    lcd.setCursor(0, 1);
    lcd.print("        "); // Clear line
    lcd.setCursor(0, 1);
    lcd.print(rotaryCounter);
    lcd.print(" sec");
    
    delay(50);
  }
  
  waitForButtonRelease(SELECT_BUTTON_PIN);
  
  // Countdown phase
  while (rotaryCounter > 0 && digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Countdown:");
    lcd.setCursor(0, 1);
    lcd.print(rotaryCounter);
    lcd.print(" sec");
    
    delay(1000);
    rotaryCounter--;
  }
  
  if (rotaryCounter <= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time's Up!");
    delay(3000);
  }
  
  rotaryCounter = 0;
}

void runStopwatch() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stopwatch:");
  
  bool isRunning = false;
  unsigned long startTime = 0;
  unsigned long elapsedTime = 0;
  unsigned long pausedTime = 0;
  
  while (digitalRead(SELECT_BUTTON_PIN) == HIGH) {
    // Toggle start/stop with rotary encoder click
    rotaryClkState = digitalRead(ROTARY_CLK_PIN);
    if (rotaryClkState != prevRotaryClkState && rotaryClkState == HIGH) {
      isRunning = !isRunning;
      
      if (isRunning) {
        if (pausedTime > 0) {
          startTime = millis() - pausedTime;
        } else {
          startTime = millis();
        }
      } else {
        pausedTime = elapsedTime;
      }
    }
    prevRotaryClkState = rotaryClkState;
    
    // Update display
    if (isRunning) {
      elapsedTime = millis() - startTime;
    }
    
    unsigned long seconds = elapsedTime / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    seconds %= 60;
    minutes %= 60;
    
    lcd.setCursor(0, 1);
    if (hours < 10) lcd.print("0");
    lcd.print(hours);
    lcd.print(":");
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0");
    lcd.print(seconds);
    
    delay(100);
  }
}

void initiateOTAMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OTA Mode");
  lcd.setCursor(0, 1);
  lcd.print("Not implemented");
  
  delay(3000);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================
bool connectToWiFi() {
  WiFi.disconnect(true);
  delay(100);
  
  WiFi.begin(ssid, password);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  
  unsigned long startTime = millis();
  int dotCount = 0;
  
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > 15000) { // 15 second timeout
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Failed");
      delay(2000);
      return false;
    }
    
    lcd.setCursor(0, 1);
    for (int i = 0; i < dotCount; i++) {
      lcd.print(".");
    }
    for (int i = dotCount; i < 16; i++) {
      lcd.print(" ");
    }
    
    dotCount = (dotCount + 1) % 17;
    delay(200);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1000);
  
  return true;
}

void disconnectWiFi() {
  WiFi.disconnect(true);
}

void displayCurrentTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time Error");
    return;
  }
  
  lcd.setCursor(0, 0);
  lcd.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  lcd.print("       ");
  
  lcd.setCursor(0, 1);
  lcd.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  lcd.print("    ");
}

void fetchYouTubeData() {
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient https;
  String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=";
  url += channelId;
  url += "&key=";
  url += apiKey;
  
  https.begin(client, url);
  int httpCode = https.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, https.getString());
    
    if (!error && doc["items"] && doc["items"][0]["statistics"]) {
      long subscriberCount = doc["items"][0]["statistics"]["subscriberCount"].as<long>();
      long viewCount = doc["items"][0]["statistics"]["viewCount"].as<long>();
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Subs: ");
      lcd.print(subscriberCount);
      lcd.setCursor(0, 1);
      lcd.print("Views: ");
      lcd.print(viewCount);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("API Error");
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HTTP Error:");
    lcd.setCursor(0, 1);
    lcd.print(httpCode);
  }
  
  https.end();
}

float measureDistance() {
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  float duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  float distance = duration * 0.0344 / 2;
  
  return distance;
}

void handleRotaryEncoder() {
  rotaryClkState = digitalRead(ROTARY_CLK_PIN);
  
  if (rotaryClkState != prevRotaryClkState && rotaryClkState == HIGH) {
    if (digitalRead(ROTARY_DT_PIN) == HIGH) {
      rotaryCounter--;
      if (rotaryCounter < 0) rotaryCounter = 0;
    } else {
      rotaryCounter++;
      if (rotaryCounter > 999) rotaryCounter = 999; // Reasonable limit
    }
  }
  
  prevRotaryClkState = rotaryClkState;
}
