<div align="center">

# 🔌 IoT Projects — ESP32 RFID Smart Access & Attendance

**A collection of ESP32-based IoT sketches implementing an RFID-driven smart door access and attendance system, integrated with Firebase Realtime Database and the Ubidots IoT dashboard.**

[![Arduino](https://img.shields.io/badge/Arduino-C++-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-E7352C?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32)
[![Firebase](https://img.shields.io/badge/Firebase-Realtime%20DB-FFCA28?style=for-the-badge&logo=firebase&logoColor=black)](https://firebase.google.com/)
[![Ubidots](https://img.shields.io/badge/Ubidots-IoT%20Dashboard-38B6FF?style=for-the-badge)](https://ubidots.com/)

</div>

---

## ⚠️ Security Note

The `.ino` files in this repository currently contain **hardcoded, real credentials** committed directly in the source — including a Firebase API key, an Ubidots API token, and (in `sketch_apr27a`) a **real home Wi-Fi network name and password**. Since this is a **public** repository, these are visible to anyone.

**Recommended before doing anything else with this repo:**
1. **Rotate every exposed credential** — regenerate the Firebase API key/rules, revoke and reissue the Ubidots token, and change the Wi-Fi password that was committed.
2. **Remove secrets from the current files** and replace them with placeholders (e.g. `YOUR_WIFI_SSID`, `YOUR_FIREBASE_AUTH_TOKEN`), loaded instead from a local, git-ignored `secrets.h` header.
3. **Scrub git history** (e.g. with `git filter-repo` or the BFG Repo-Cleaner) — deleting the values in a new commit alone does not remove them from earlier commits, which stay publicly accessible.

This section intentionally does not repeat any of the exposed values.

## 📖 Overview

This repository tracks the iterative development of a **hardware-based access control and attendance system** built around an ESP32 microcontroller, an RFID reader, and cloud backends for identity lookup and telemetry. Each folder represents a milestone in that evolution — from a simple grant/deny door lock to a full attendance system with a real-time clock and a live IoT dashboard.

## 🧩 Projects

### 1. `sketch_apr24a` — RFID Door Access Control (v1)
A minimal access-control prototype: a card's UID is checked against a Firebase `/authorized_users/` list. Access is signaled with green/red LEDs and a buzzer, and unauthorized attempts are logged back to Firebase under `/unauthorized_attempts/`.

### 2. `sketch_apr27a` — RFID Attendance + Ubidots Telemetry (v2)
Extends v1 with **Ubidots MQTT** integration for live telemetry (`auth_access`, `id_label`), an OLED status display, and Firebase-backed lookup of student records (checking for a `name` field per UID) before recording an attendance timestamp.

### 3. `Smart_Attendance_System` — Combined System (latest)
The most complete version, adding a **DS3231 real-time clock (RTC)** for accurate calendar dates (rather than uptime), an OLED welcome message showing the recognized student's name fetched from Firebase, a running attendance counter, and continued Ubidots dashboard publishing.

## 🛠️ Hardware

| Component | Role |
|---|---|
| ESP32 | Main microcontroller (Wi-Fi + processing) |
| MFRC522 | RFID/NFC card reader module (SPI) |
| SSD1306 OLED (128×64, I²C) | On-device status display (welcome messages, access results) |
| DS3231 RTC module | Real-time clock for accurate attendance dates |
| Buzzer + LEDs (red/green/blue) | Audible & visual access-result feedback |

## ☁️ Cloud & Connectivity

- **Firebase Realtime Database** — stores authorized users, student records, and attendance/access logs
- **Ubidots (MQTT)** — real-time IoT dashboard for access events and telemetry
- **Wi-Fi (ESP32)** — connects the device to both cloud services

## 🧰 Tech Stack

| Layer | Technology |
|---|---|
| Language | C++ (Arduino framework) |
| Microcontroller | ESP32 |
| RFID | MFRC522 library (SPI) |
| Display | Adafruit GFX + Adafruit SSD1306 |
| Real-time clock | RTClib (DS3231) |
| Backend / DB | Firebase (REST-based Firebase library) |
| IoT Dashboard | UbidotsESPMQTT |

## 📁 Project Structure

```
IOT_projects/
├── sketch_apr24a/
│   └── sketch_apr24a.ino                       # v1 — basic RFID access control
├── sketch_apr27a/
│   └── sketch_apr27a.ino                       # v2 — + Ubidots telemetry
└── Smart_Attendance_System/
    └── Smart_Attendance_System.ino             # v3 — + RTC, OLED name lookup, counter
```

## 🚀 Getting Started

**Prerequisites:** [Arduino IDE](https://www.arduino.cc/en/software) (or PlatformIO) with ESP32 board support installed.

1. Install the required libraries via the Library Manager: `MFRC522`, `Adafruit GFX Library`, `Adafruit SSD1306`, `RTClib`, `UbidotsESPMQTT`, and a Firebase ESP32 client library.
2. Wire the RFID reader, OLED display, RTC module (where used), buzzer, and LEDs to your ESP32 per the pin definitions at the top of each sketch.
3. Create your own **Firebase Realtime Database** and **Ubidots** account, and supply your own Wi-Fi and API credentials — see the Security Note above for how to do this safely (a git-ignored `secrets.h`, never hardcoded values).
4. Flash the sketch of your choice to the ESP32 and open the Serial Monitor at `115200` baud to follow connection and card-read logs.

---

<div align="center">

Built by **[Mostafa Medhat](https://github.com/MostafaMedhat19)**

</div>

