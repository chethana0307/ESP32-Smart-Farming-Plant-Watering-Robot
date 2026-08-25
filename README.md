<div align="center">

# 🌱 ESP32 Smart Farming & Plant Humidification Robot

### An autonomous line-following robot that finds plants and activates a humidifier automatically

<p>
  <img src="https://img.shields.io/badge/Controller-ESP32-1B6B4A?style=for-the-badge" alt="ESP32">
  <img src="https://img.shields.io/badge/Platform-Arduino-2F8F6B?style=for-the-badge" alt="Arduino">
  <img src="https://img.shields.io/badge/Navigation-5_IR_Sensors-C9A227?style=for-the-badge" alt="5 IR Sensors">
  <img src="https://img.shields.io/badge/Humidifier-RFID_Triggered-5A8F72?style=for-the-badge" alt="RFID-triggered humidifier">
</p>

<p>
  <a href="#-about-the-project">About</a> •
  <a href="#-features">Features</a> •
  <a href="#-how-it-works">How It Works</a> •
  <a href="#-hardware">Hardware</a> •
  <a href="#-wiring-guide">Wiring</a> •
  <a href="#-installation">Installation</a> •
  <a href="#-testing">Testing</a>
</p>

</div>

---

## 🌾 About the Project

This project is an **ESP32-powered smart farming robot** designed to automate plant humidification. It follows a black path using five IR sensors and uses RFID tags to identify plant-care locations. When a tag is detected, the robot stops, activates a relay-controlled humidifier for five seconds, and then continues along its route.

The goal is to make routine plant care more efficient by combining robotics, navigation, location detection, and automatic humidification in one affordable system.

<table>
  <tr>
    <td width="50%" valign="top">
      <h3>🎯 Mission</h3>
      <p>Build a simple, affordable robot that can navigate between plants and provide controlled humidity with minimal human effort.</p>
    </td>
    <td width="50%" valign="top">
      <h3>🔭 Vision</h3>
      <p>Develop the robot into an intelligent farming assistant that uses humidity data, plant-specific misting cycles, and wireless monitoring.</p>
    </td>
  </tr>
</table>

> [!IMPORTANT]
> The current firmware activates the **humidifier when an RFID tag is detected**. It does not yet read a humidity sensor, so each tag currently starts a fixed five-second humidification cycle.

## ✨ Features

<table>
  <tr>
    <td width="50%" valign="top">
      <h3>🛣️ Smart Navigation</h3>
      <ul>
        <li>Five-sensor black-line tracking</li>
        <li>Gentle and sharp turn correction</li>
        <li>Last-turn direction memory</li>
        <li>Five-second lost-line search</li>
      </ul>
    </td>
    <td width="50%" valign="top">
      <h3>💨 Automatic Humidification</h3>
      <ul>
        <li>RC522 RFID location detection</li>
        <li>Automatic motor stop before humidifying</li>
        <li>Relay-controlled five-second humidifier cycle</li>
        <li>Duplicate RFID-trigger prevention</li>
      </ul>
    </td>
  </tr>
  <tr>
    <td width="50%" valign="top">
      <h3>⚙️ Motor Control</h3>
      <ul>
        <li>Independent left and right motor control</li>
        <li>Variable PWM speed</li>
        <li>Forward and reverse movement</li>
        <li>Pivot turns for sharp corners</li>
      </ul>
    </td>
    <td width="50%" valign="top">
      <h3>🖥️ Live Diagnostics</h3>
      <ul>
        <li>IR values in the Serial Monitor</li>
        <li>RFID UID output</li>
        <li>Relay and humidifier status</li>
        <li>Lost-line and recovery messages</li>
      </ul>
    </td>
  </tr>
</table>

## 📊 Project at a Glance

| **5** | **5 seconds** | **700 ms** | **115200** |
|:---:|:---:|:---:|:---:|
| IR sensors | Humidifier run time | RFID reset delay | Serial baud rate |

## 🚜 How It Works

1. **Scan the path** — five IR sensors continuously check for a black line on a white surface.
2. **Follow the line** — the ESP32 adjusts the left and right motor speeds through the L298N driver.
3. **Find a plant** — an RFID tag placed near each plant marks its humidification location.
4. **Stop safely** — RFID detection receives priority and immediately stops both motors.
5. **Humidify the plant area** — GPIO 32 activates the relay and humidifier for five seconds.
6. **Resume the route** — the humidifier switches off and the robot continues line following.
7. **Recover when lost** — if all sensors see white, the robot searches using its last known turn direction for up to five seconds.

## 🔩 Hardware

| Component | Quantity | Purpose |
|---|:---:|---|
| ESP32 development board | 1 | Main controller |
| Five-channel IR sensor array | 1 | Detects the black path |
| RC522 RFID reader | 1 | Detects plant-care locations |
| RFID cards or tags | As needed | Marks plant locations |
| L298N motor driver | 1 | Drives the left and right motors |
| DC geared motors | 2 or 4 | Moves the robot |
| One-channel relay module | 1 | Switches the humidifier |
| Low-voltage humidifier or mist maker | 1 | Produces moisture around the plant |
| Humidifier reservoir | 1 | Holds the water used to create mist |
| Robot chassis and wheels | 1 set | Supports the system |
| External power supply | 1 | Powers the motors and humidifier |
| Jumper wires | As needed | Makes electrical connections |

## 🔌 Wiring Guide

### IR Line Sensor → ESP32

The program expects **black = LOW (`0`)** and **white = HIGH (`1`)**.

| Sensor output | Position | ESP32 GPIO |
|---|---|:---:|
| OUT1 | Far left | 36 |
| OUT2 | Left | 39 |
| OUT3 | Center | 34 |
| OUT4 | Right | 35 |
| OUT5 | Far right | 33 |

### L298N Motor Driver → ESP32

| L298N pin | Function | ESP32 GPIO |
|---|---|:---:|
| ENA | Left motor speed | 25 |
| IN1 | Left motor direction | 26 |
| IN2 | Left motor direction | 27 |
| ENB | Right motor speed | 14 |
| IN3 | Right motor direction | 17 |
| IN4 | Right motor direction | 16 |

- Connect the left motor or motor pair to **OUT1/OUT2**.
- Connect the right motor or motor pair to **OUT3/OUT4**.

### RC522 RFID Reader → ESP32

| RC522 pin | ESP32 connection |
|---|:---:|
| SDA / SS | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| RST | GPIO 4 |
| 3.3V | 3.3V |
| GND | GND |

> [!WARNING]
> The RC522 is a **3.3 V device**. Do not connect its power pin to 5 V.

### Relay and Humidifier

| Relay pin | Connection |
|---|---|
| IN | ESP32 GPIO 32 |
| VCC | Supply voltage required by the relay module |
| GND | Common ground |

The firmware is configured for an **active-LOW relay**:

```cpp
const int RELAY_ON  = LOW;
const int RELAY_OFF = HIGH;
```

Connect the humidifier through the relay's **COM** and **NO** contacts so it remains off until an RFID tag is detected. Use a suitable external supply for the humidifier—never power a motor or humidifier directly from an ESP32 GPIO pin.

> [!CAUTION]
> This guide assumes a **low-voltage DC humidifier or mist maker**. Do not wire a mains-powered appliance unless the system is designed and checked by a qualified person.

## 💻 Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) or PlatformIO
- ESP32 board package for Arduino
- [`MFRC522`](https://github.com/miguelbalboa/rfid) library
- Built-in Arduino `SPI` library

## 🚀 Installation

1. Install the Arduino IDE.
2. Install ESP32 board support from **Boards Manager**.
3. Open **Library Manager** and install `MFRC522`.
4. Save the program as `smart_farming_robot.ino`.
5. Open it in the Arduino IDE.
6. Select the correct ESP32 board and serial port.
7. Compile and upload the firmware.
8. Open the Serial Monitor at **115200 baud**.

## 🧰 Configuration

Adjust these values near the top of the sketch to tune the robot:

```cpp
const int BASE_SPEED = 160;
const int SLOW_SPEED = 100;
const int TURN_SPEED = 200;
const int HARD_TURN_SPEED = 250;

const unsigned long RFID_ACTION_TIME = 5000;
const unsigned long CARD_REMOVED_TIME = 700;
const unsigned long SEARCH_TIMEOUT = 5000;
```

| Setting | Purpose |
|---|---|
| `BASE_SPEED` | Normal forward speed |
| `SLOW_SPEED` | Inner-wheel speed during a gentle turn |
| `TURN_SPEED` | Outer-wheel speed during a gentle turn |
| `HARD_TURN_SPEED` | Pivot-turn and line-recovery speed |
| `RFID_ACTION_TIME` | Humidifier operating time in milliseconds |
| `CARD_REMOVED_TIME` | Required tag absence before another trigger |
| `SEARCH_TIMEOUT` | Maximum lost-line search time |

## 🧭 Line Sensor Behaviour

| Sensor pattern | Robot action |
|:---:|---|
| `1 1 0 1 1` | Move forward |
| `1 0 1 1 1` | Turn slightly left |
| `0 1 1 1 1` | Turn hard left |
| `1 1 1 0 1` | Turn slightly right |
| `1 1 1 1 0` | Turn hard right |
| `1 1 1 1 1` | Search using the last turn direction |
| `0 0 0 0 0` | Stop |

Other mixed patterns are handled by checking whether the line appears on the left or right side.

## 🧪 Testing

| Test | Expected result |
|---|---|
| Move a black line under each IR sensor | Serial Monitor values change correctly |
| Lift the wheels and test movement | Both sides move in the correct direction |
| Present an RFID card | Its UID prints once and the motors stop |
| Keep the RFID card under the reader | It does not continuously retrigger |
| Trigger the relay with the humidifier disconnected | Relay stays on for five seconds |
| Connect the humidifier | Mist is produced and the ESP32 remains stable |
| Run the complete route slowly | Robot stops and activates the humidifier at every tag |

## 🛡️ Safety

- Use a suitable separate power source for the motors and humidifier.
- Connect the ESP32, motor driver, RFID reader, and relay control circuit to a common ground.
- Keep all electronics and battery connections protected from water and mist.
- Confirm that the relay's voltage and current ratings support the humidifier.
- Keep the humidifier outlet pointed away from the ESP32, RFID reader, motor driver, and battery.
- Test the robot with its wheels lifted before placing it on the track.

## 🩺 Troubleshooting

<details>
<summary><strong>The robot moves or turns in the wrong direction</strong></summary>

Swap the affected motor wires or correct that motor's direction pins in the firmware.
</details>

<details>
<summary><strong>The IR readings are reversed</strong></summary>

Calibrate the sensor board or update the firmware's black/white logic.
</details>

<details>
<summary><strong>The robot frequently loses the line</strong></summary>

Reduce the motor speeds, adjust the IR sensor height, and tune the sensor sensitivity.
</details>

<details>
<summary><strong>The RC522 cannot detect a tag</strong></summary>

Check the 3.3 V supply, SPI wiring, and `MFRC522` library installation.
</details>

<details>
<summary><strong>The relay operates in reverse</strong></summary>

Swap the `RELAY_ON` and `RELAY_OFF` values in the firmware.
</details>

<details>
<summary><strong>The ESP32 restarts when the humidifier starts</strong></summary>

Use a properly rated separate supply, connect the grounds, and add electrical-noise protection.
</details>

## 🌟 Future Improvements

- Add a DHT11, DHT22, or another humidity sensor
- Activate the humidifier only when humidity is below a configured threshold
- Give different RFID tags different humidification durations
- Replace the blocking five-second delay with non-blocking timing
- Add a humidifier reservoir level sensor
- Add Wi-Fi monitoring and a mobile dashboard
- Store humidity history and plant health information
- Monitor battery voltage and support automatic charging

## 📁 Repository Structure

```text
ESP32-Smart-Farming-Humidifier-Robot/
├── smart_farming_robot.ino
├── README.md
└── assets/
    └── images/              # Optional project photos and wiring diagrams
```

## 🤝 Contributing

Suggestions and improvements are welcome. Fork the repository, create a feature branch, commit your changes, and open a pull request.

## 📄 License

Add an open-source license such as the [MIT License](https://choosealicense.com/licenses/mit/) before accepting contributions.

---

<div align="center">

### Built to make plant care smarter, simpler, and more reliable. 🌿

**ESP32 • Robotics • RFID • Automated Humidification**

</div>
