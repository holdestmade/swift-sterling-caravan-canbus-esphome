# Swift/Sterling Caravan CANBUS ESPHome Interface

An ESPHome configuration that bridges a Swift or Sterling caravan's internal CAN bus network to Wi-Fi, exposing full monitoring and control through Home Assistant.

## Overview

Modern Swift/Sterling caravans use a CAN bus network to connect the control panel, Alde heating system, solar/battery management, and lighting. This project puts an ESP32 on that bus and makes every sensor reading and controllable output available as a Home Assistant entity — without disturbing the caravan's native systems.

## Hardware

| Component | Detail |
|-----------|--------|
| Microcontroller | ESP32 (esp32dev) |
| CAN transceiver | SN65HVD230 |
| CAN RX pin | GPIO 4 |
| CAN TX pin | GPIO 5 |
| CAN bit rate | 125 kbps |
| PWM output 1 (Floor Lights) | GPIO 16 |
| PWM output 2 (Pelmet Lights) | GPIO 17 |

## Features

### Lighting control
- Bedroom and living area dimmers (0–100 %, bidirectional sync with CAN bus)
- Interior, awning, and entry lights (on/off)
- Floor and pelmet LED strips (PWM via LEDC, 10 kHz)
- Feedback-loop prevention so local panel and Home Assistant stay in sync

### Heating (Alde) - CURRENTLY READ ONLY
- Thermostat entity with Heat / Off modes
- Default preset 19 °C, storage preset 10 °C, range 4–30 °C
- Heating mode read-out: Off / Electric 1 kW / 2 kW / 3 kW / Gas
- Hot water mode read-out: Off / Normal / Boost
- Timer vs manual control mode indicator
- Error flags for heating and hot water faults

### Power & battery monitoring
| Sensor | Unit |
|--------|------|
| Vehicle battery voltage | V |
| Leisure battery voltage | V |
| Leisure battery (EMA smoothed) | V |
| Leisure battery current | A |
| Solar current | A |
| AC mains current | A |
| Battery power | W |
| Solar power | W |
| Mains power | W |
| Leisure battery charge/discharge status | text |

### Environment
- Internal temperature (°C)
- External temperature (°C)
- Internal humidity (%)

### Solar & charging
- Solar charging active flag
- Solar charging source (text)
- Solar charge destination (text)
- Charging mode: Smart / Normal / Smart 2 / Smart 3

### Status & warnings
- Pump on/off
- Mains connected
- Car engine running
- Selected battery (Car / Leisure)
- Frost warning, voltage warning, mains warning, AC overcurrent warning
- CAN bus online indicator (5-second timeout watchdog)
- CAN message rate (msg/s)
- Software versions for control panel, EC630, and PSU

### Alko ATC (trailer stability)
- ATC status text
- Self-test pass/fail tracking
- Raw ATC CAN data

## CAN Bus Frame Map

| Frame ID | Contents |
|----------|----------|
| `0x032` | Pump, battery selection, engine, mains, interior/awning lights |
| `0x033` | Dimmer levels, solar source and charging status |
| `0x05A` | Warning flags (frost, voltage, mains, AC overcurrent) |
| `0x05B` | Heating/hot water modes, software versions, fault flags |
| `0x083` | Internal/external temperature, PSU software number |
| `0x084` | Battery voltages, currents, humidity, entry light |
| `0x085` | Heating mode selection |
| `0x086` | Solar charge destination and charging mode |
| `0x087` | Timer vs manual heating control |
| `0x0D8` | Alko ATC trailer control status |

## Home Assistant Integration

All entities are exposed over the encrypted ESPHome API. Once adopted in Home Assistant you get:

- Light entities for every lighting circuit (with dimmer support)
- Climate entity for the Alde heating system (READ ONLY)
- Select entities for heating and hot water mode read-outs (READ ONLY)
- Sensor entities for every voltage, current, power, temperature, and status value
- Binary sensor entities for all warning and status flags

OTA firmware updates are supported directly from the ESPHome dashboard.

## Configuration

Secrets are stored in a separate `secrets.yaml` file (not included). You need to provide:

```yaml
wifi_ssid: "YourSSID"
wifi_password: "YourPassword"
api_encryption_key: "your-32-byte-base64-key"
```

## Wiring Notes

- Connect the SN65HVD230 CANH/CANL lines in parallel with the existing caravan CAN bus — do **not** break the bus.
- The caravan bus already has termination resistors; do **not** add a second 120 Ω terminator.
- Power the ESP32 from a fused 5 V supply derived from the caravan's 12 V leisure battery rail.

## Compatibility

Tested on Swift/Sterling caravans that use the Al-Ko / Alde / Sargent integrated CAN bus system. The CAN frame IDs and byte layouts are specific to this manufacturer's implementation and may not match other brands.
