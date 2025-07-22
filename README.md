# SmartHab-Connect or 
# ESP32 Multi-Sensor IoT Dashboard

A comprehensive IoT project featuring an ESP32-based multi-sensor dashboard with LCD display, rotary encoder navigation, and various sensor integrations.

## 🚀 Features

- **Time Display**: Real-time clock with NTP synchronization
- **Environmental Monitoring**: Temperature and humidity sensing (DHT22)
- **YouTube API Integration**: Display subscriber and view counts
- **Bluetooth Audio**: A2DP sink for wireless audio streaming
- **Sensor Array**:
  - Ultrasonic distance measurement
  - Light intensity monitoring (LDR)
  - Soil moisture detection
  - Voltage monitoring
- **Interactive Controls**: Rotary encoder and joystick navigation
- **Timer Functions**: Countdown timer and stopwatch
- **Over-the-Air Updates**: OTA update capability

## 🛠️ Hardware Requirements

### Core Components
- ESP32 Development Board
- 16x2 I2C LCD Display (0x27 address)
- DHT22 Temperature/Humidity Sensor
- Rotary Encoder with Push Button
- Analog Joystick Module

### Sensors
- Ultrasonic Sensor (HC-SR04)
- Light Dependent Resistor (LDR)
- Soil Moisture Sensor
- Voltage Divider Circuit for Voltage Monitoring

### Connections

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| DHT22 Data | GPIO 23 | Temperature/Humidity |
| LCD (I2C) | GPIO 21 (SDA), GPIO 22 (SCL) | Address: 0x27 |
| Rotary Encoder CLK | GPIO 19 | |
| Rotary Encoder DT | GPIO 18 | |
| Rotary Encoder SW | GPIO 17 | Push button |
| Joystick X | GPIO 34 (ADC) | |
| Joystick Y | GPIO 32 (ADC) | |
| Select Button | GPIO 14 | |
| Ultrasonic Trig | GPIO 4 | |
| Ultrasonic Echo | GPIO 5 | |
| LDR/Soil/Voltage | GPIO 35 (ADC) | Shared analog input |

## 📚 Dependencies

Add these libraries to your Arduino IDE or PlatformIO:

```cpp
// Core libraries
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Display and sensors
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <ezButton.h>

// Audio and APIs
#include <YoutubeApi.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
```

## 🔧 Configuration

### WiFi Setup
Update the following credentials in the code:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### YouTube API
1. Get API key from [Google Developers Console](https://console.developers.google.com/)
2. Update the credentials:
```cpp
const char* apiKey = "YOUR_YOUTUBE_API_KEY";
const char* channelId = "YOUR_CHANNEL_ID";
```

## 🚀 Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/esp32-iot-dashboard.git
   cd esp32-iot-dashboard
   ```

2. Open `src/main.ino` in Arduino IDE or use PlatformIO

3. Install required libraries (see Dependencies section)

4. Update configuration values (WiFi, API keys)

5. Upload to your ESP32

## 💡 Usage

### Navigation
- **Joystick**: Navigate through menu options
- **Select Button**: Confirm selection
- **Rotary Encoder**: Used in timer functions for value adjustment

### Menu Options
1. **Time**: Display current time and date
2. **Humidity & Temperature**: Environmental readings
3. **Subscriber & Views**: YouTube channel statistics
4. **Bluetooth**: Audio streaming mode
5. **Timer**: Countdown timer with rotary encoder control
6. **Ultrasonic**: Distance measurement
7. **LDR**: Light intensity monitoring
8. **Soil Moisture**: Soil humidity levels
9. **Voltage**: Voltage monitoring
10. **OTA**: Over-the-air update mode

### Bluetooth Audio
- Device name: "ESP32_Bluetooth"
- Use joystick Y-axis for track control (next/previous)
- Compatible with A2DP audio devices

## 📁 Project Structure

```
esp32-iot-dashboard/
├── src/
│   ├── main.ino                 # Main Arduino sketch
│   └── config.h                 # Configuration file (create this)
├── hardware/
│   ├── schematic.png           # Circuit diagram
│   ├── breadboard_layout.jpg   # Physical layout
│   └── components_list.md      # Detailed parts list
├── docs/
│   ├── setup_guide.md         # Detailed setup instructions
│   ├── troubleshooting.md     # Common issues and solutions
│   └── api_setup.md          # YouTube API configuration
├── examples/
│   ├── sensor_test/           # Individual sensor test codes
│   └── calibration/           # Sensor calibration utilities
├── assets/
│   ├── demo_video.mp4        # Project demonstration
│   └── screenshots/          # LCD display examples
├── README.md
├── LICENSE
└── platformio.ini            # PlatformIO configuration (optional)
```

## 🔍 Troubleshooting

### Common Issues

**WiFi Connection Failed**
- Verify SSID and password
- Check signal strength
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

**Sensor Readings Incorrect**
- Check wiring connections
- Verify power supply (3.3V/5V requirements)
- Calibrate sensors using example code

**LCD Not Displaying**
- Verify I2C address (use I2C scanner)
- Check SDA/SCL connections
- Ensure proper power supply

**YouTube API Error**
- Verify API key validity
- Check quota limits
- Ensure internet connectivity

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/new-sensor`
3. Commit changes: `git commit -am 'Add new sensor support'`
4. Push to branch: `git push origin feature/new-sensor`
5. Submit a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- ESP32 community for excellent documentation
- Arduino libraries contributors
- YouTube Data API v3 documentation

## 📧 Contact

- **Project Link**: [https://github.com/yourusername/esp32-iot-dashboard](https://github.com/yourusername/esp32-iot-dashboard)
- **Issues**: Use GitHub Issues for bug reports and feature requests

---

*Happy IoT Building! 🚀*
