Perfect! Here's a complete draft for your `docs/setup_guide.md` file:

---

### 📘 `docs/setup_guide.md`

# ESP32 IoT Dashboard – Setup Guide

This guide explains how to assemble, configure, and use the **ESP32 Multi-Sensor IoT Dashboard**.
It combines sensors, display, WiFi, Bluetooth, and user controls for a rich embedded interface.

---

## 🧰 Required Hardware

| Component                   | Quantity | Notes                            |
| --------------------------- | -------- | -------------------------------- |
| ESP32 Dev Board             | 1        | ESP32-WROOM-32 recommended       |
| DHT22 Sensor                | 1        | Temperature & humidity sensor    |
| Ultrasonic Sensor (HC-SR04) | 1        | Trig/Echo pins                   |
| LDR                         | 1        | Analog sensor                    |
| Soil Moisture Sensor        | 1        | Analog output type               |
| Voltage Divider Module      | 1        | Or use 10k/100k resistor divider |
| I2C LCD (16x2)              | 1        | With backpack, address `0x27`    |
| Joystick Module             | 1        | With X, Y, SW pins               |
| Rotary Encoder Module       | 1        | With CLK, DT, SW pins            |
| Push Button (Select)        | 1        | For menu interaction             |
| Breadboard + Jumper Wires   | \~20     | Male-to-male                     |
| Micro-USB Cable + 5V Source | 1        | For power and programming        |

---

## 🔌 Wiring Guide

| Sensor / Module   | ESP32 Pin | Notes               |
| ----------------- | --------- | ------------------- |
| DHT22             | GPIO 23   | Data pin only       |
| Ultrasonic Trig   | GPIO 4    | Output pin          |
| Ultrasonic Echo   | GPIO 5    | Input pin           |
| LDR / Soil / Volt | GPIO 35   | Shared analog input |
| Joystick X        | GPIO 34   | Analog input        |
| Joystick Y        | GPIO 32   | Analog input        |
| Joystick SW       | GPIO 14   | Digital button      |
| Rotary CLK        | GPIO 19   | Digital input       |
| Rotary DT         | GPIO 18   | Digital input       |
| Rotary SW         | GPIO 17   | Digital input       |
| I2C LCD SDA       | GPIO 21   | I2C data            |
| I2C LCD SCL       | GPIO 22   | I2C clock           |

> ✅ **Note:** GPIO 35 is shared — connect only one analog sensor at a time (LDR, Soil, Voltage).

---

## 🛠️ Software Setup

### 1. Install the Required Libraries

Open Arduino IDE and install the following libraries via **Library Manager**:

* `DHT sensor library for ESPx` by `beegee_tokyo`
* `LiquidCrystal_I2C` by `Marco Schwartz` or `johnrickman`
* `WiFi` (built-in for ESP32)
* `WiFiClientSecure`
* `HTTPClient`
* `ArduinoJson` by `Benoit Blanchon`
* `ezButton` by `ArduinoGetStarted`
* `YoutubeApi`
* `AudioTools`
* `BluetoothA2DPSink`

---

### 2. Configure `config.h`

Copy `src/config_template.h` to `src/config.h` and edit:

```cpp
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

const char* apiKey = "Your_YouTube_API_Key";
const char* channelId = "Your_Channel_ID";
```

> 💡 Get your YouTube API key from [Google Cloud Console](https://console.developers.google.com/). Enable **YouTube Data API v3**.

---

### 3. Upload Code to ESP32

* Open `src/main.ino` in Arduino IDE.
* Select **Board**: `ESP32 Dev Module`
* Select correct **COM Port**
* Click **Upload**

---

## 📲 Using the Dashboard

### Navigation

* Use the **joystick left/right** to move through the main menu.
* Press **Select button** (SW pin) to open a feature.
* Use the **rotary encoder** in the Timer menu.

### Features

* **Time Sync** – Shows real-time clock via NTP server.
* **Humidity & Temp** – From DHT22.
* **YouTube Stats** – Subscribers + views.
* **Bluetooth Audio** – ESP32 becomes speaker.
* **Timer / Stopwatch** – Interactive countdown or stopwatch.
* **Ultrasonic** – Shows distance.
* **Light Intensity** – LDR reading in %.
* **Soil Moisture** – Moisture level in %.
* **Voltage Sensor** – Reads voltage via divider circuit.
* **OTA** – Placeholder (future implementation).

---

## 📦 Folder Structure

```bash
esp32-iot-dashboard/
├── src/
│   ├── main.ino                 # Main logic
│   └── config.h                 # Config file (keep private)
├── hardware/
│   ├── schematic.png            # Circuit diagram
│   ├── breadboard_layout.jpg    # Breadboard layout
│   └── components_list.md       # Bill of Materials
├── docs/
│   └── setup_guide.md           # You're here
├── assets/
│   ├── demo_video.mp4           # Demo footage
│   └── screenshots/             # LCD output images
├── README.md
├── LICENSE
└── platformio.ini               # (Optional)
```

---

## 🧪 Troubleshooting

| Issue                         | Solution                                   |
| ----------------------------- | ------------------------------------------ |
| LCD shows nothing             | Check I2C address (default is `0x27`)      |
| WiFi fails                    | Check `ssid`/`password`, signal strength   |
| YouTube data not loading      | Check API key, enable YT Data API          |
| Wrong sensor reading (analog) | Only plug one sensor into GPIO35 at a time |

