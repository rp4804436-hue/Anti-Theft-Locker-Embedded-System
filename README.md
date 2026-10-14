# Autonomous Anti-Theft Locker System with Embedded Security

[![Platform](https://img.shields.io/badge/Platform-Arduino%20%7C%20ESP32-blue.svg)](https://www.arduino.cc/)
[![Language](https://img.shields.io/badge/Language-Embedded%20C%2F%2B%2B-green.svg)](https://isocpp.org/)
[![Simulation](https://img.shields.io/badge/Simulation-Wokwi%20Verified-orange.svg)](https://wokwi.com)
[![License](https://img.shields.io/badge/License-MIT-purple.svg)](LICENSE)

An enterprise-grade electronic access-control system featuring matrix keypad scanning, encrypted-style credential masking, non-blocking finite state machine (FSM) control, automated mechanical bolt relatching, and an acoustic brute-force lockout alarm.

---

## Key Features
- **4x4 Matrix Interface:** Scans numeric and command keys using an 8-GPIO time-multiplexed layout.
- **Shoulder-Surfing Defense:** Displays asterisks (`*`) on a 16x2 I2C LCD to hide user credentials.
- **Actuator Bolt Control:** Sweeps an SG90 micro-servo through a $90^\circ$ arc to retract the locking bolt.
- **Non-Blocking Auto-Relatch:** Automatically secures the locker after 5 seconds using `millis()` timing, eliminating blocking `delay()` loops.
- **Brute-Force Protection:** Automatically locks the interface and sounds a piezo siren for 30 seconds after 3 consecutive wrong attempts.
- **Virtual Simulation Included:** Fully configured for browser-based simulation in Wokwi without physical hardware.

---

## System Architecture
```text
Keypad Inputs (8 Pins) ──► [ Arduino UNO / ATmega328P ] ──► I2C LCD (Visuals)
                           [   Deterministic FSM      ] ──► Servo (Lock/Unlock)
                           [    Security Timers       ] ──► Buzzer & LEDs (Alerts)
                           ---

## Hardware Specifications & Pin Mapping

| Component | Interface | Microcontroller Pin | Function |
| :--- | :--- | :--- | :--- |
| **Keypad Rows (1-4)** | Digital Output | Pins 9, 8, 7, 6 | Active-Low Row Multiplexing |
| **Keypad Cols (1-4)** | Digital Input | Pins 5, 4, 3, 2 | Pull-up Input Scanning |
| **16x2 LCD** | I2C (0x27) | Pins A4 (SDA), A5 (SCL) | System Status & Masked Entry |
| **SG90 Servo** | PWM (50Hz) | Pin 10 | Mechanical Bolt Actuator |
| **Green LED** | Push-Pull | Pin 11 | Access Granted Indicator |
| **Red LED** | Push-Pull | Pin 12 | Access Denied / Error Indicator |
| **Piezo Buzzer** | Digital Driver | Pin 13 | Acoustic Siren & Key Chirps |

---

## How to Test in the Wokwi Simulator
1. Clone this repository:
   ```bash
   git clone [https://github.com/rp4804436-hue/Anti-Theft-Locker-Embedded-System.git](https://github.com/rp4804436-hue/Anti-Theft-Locker-Embedded-System.git)