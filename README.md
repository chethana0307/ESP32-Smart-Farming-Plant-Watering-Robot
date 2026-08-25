# ESP32 Smart Farming & Plant Watering Robot

An autonomous smart-farming robot built with an **ESP32**. The robot follows a black line, detects plant locations using **RFID tags**, stops at each location, and activates a relay-controlled water pump for five seconds.

## Features

- Follows a black line using five IR sensors
- Adjusts motor speed for slight and sharp turns
- Remembers its last turn when the line is lost
- Searches for a lost line for up to five seconds
- Detects watering locations using an RC522 RFID reader
- Stops the robot before watering
- Activates a relay and water pump for five seconds
- Prevents one RFID card from triggering repeatedly while it remains under the reader
- Prints sensor values, RFID UIDs, and robot status to the Serial Monitor

> **Current watering method:** The supplied firmware starts watering when an RFID tag is detected. It does not currently read a humidity or soil-moisture sensor. Humidity-based watering is listed as a future improvement below.

## How It Works

1. The five-channel IR sensor detects a black path on a white surface.
2. The ESP32 controls the left and right motors through an L298N motor driver.
3. The robot follows the line and corrects its direction based on the active IR sensors.
4. An RFID tag placed near a plant marks a watering location.
5. When the RC522 detects a tag, the robot:
   - Stops both motors
   - Prints the tag UID to the Serial Monitor
   - Turns on the relay and water pump
   - Waters the plant for five seconds
   - Turns off the pump and resumes line following
6. If the robot loses the line, it searches in the direction of its most recent turn for up to five seconds.

## Hardware Required

| Component | Quantity | Purpose |
|---|---:|---|
| ESP32 development board | 1 | Main controller |
| 5-channel IR line-tracking sensor | 1 | Detects the black path |
| RC522 RFID reader | 1 | Detects plant watering points |
| RFID cards or tags | As needed | Marks each plant location |
| L298N motor driver | 1 | Controls the left and right motors |
| DC geared motors | 2 or 4 | Moves the robot |
| One-channel relay module | 1 | Switches the water pump |
| Mini DC water pump | 1 | Waters the plants |
| Water tube and container | 1 set | Stores and delivers water |
| Robot chassis and wheels | 1 set | Mechanical platform |
| Suitable external power supply | 1 | Powers the motors and pump |
| Jumper wires | As needed | Electrical connections |

## Pin Connections

### IR Line Sensor

The firmware expects **black = LOW (0)** and **white = HIGH (1)**.

| Sensor output | Position | ESP32 GPIO |
|---|---|---:|
| OUT1 | Far left | 36 |
| OUT2 | Left | 39 |
| OUT3 | Center | 34 |
| OUT4 | Right | 35 |
| OUT5 | Far right | 33 |

### L298N Motor Driver

| L298N pin | Function | ESP32 GPIO |
|---|---|---:|
| ENA | Left motor speed | 25 |
| IN1 | Left motor direction | 26 |
| IN2 | Left motor direction | 27 |
| ENB | Right motor speed | 14 |
| IN3 | Right motor direction | 17 |
| IN4 | Right motor direction | 16 |

Connect the left motor or motor pair to L298N **OUT1/OUT2** and the right motor or motor pair to **OUT3/OUT4**.

### RC522 RFID Reader

| RC522 pin | ESP32 GPIO |
|---|---:|
| SDA / SS | 5 |
| SCK | 18 |
| MOSI | 23 |
| MISO | 19 |
| RST | 4 |
| 3.3V | 3.3V |
| GND | GND |

> The RC522 is a **3.3 V device**. Do not power it from 5 V.

### Relay and Water Pump

| Relay pin | Connection |
|---|---|
| IN | ESP32 GPIO 32 |
| VCC | Module-rated supply voltage |
| GND | Common ground |

The firmware is configured for an **active-LOW relay**:

- `LOW` turns the pump on
- `HIGH` turns the pump off

Wire the pump through the relay's **COM** and **NO** contacts so it remains off until watering begins. Power the pump from a suitable external supply; never power it directly from an ESP32 GPIO pin.

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) or PlatformIO
- ESP32 board package for Arduino
- `MFRC522` library
- Built-in `SPI` library

## Installation

1. Install the Arduino IDE.
2. Add ESP32 board support in **Boards Manager**.
3. Open **Library Manager** and install the library named `MFRC522`.
4. Save the project code as `smart_farming_robot.ino`.
5. Open the file in the Arduino IDE.
6. Select your ESP32 board and its serial port.
7. Compile and upload the firmware.
8. Open the Serial Monitor and set the baud rate to **115200**.

## Configuration

The main values can be adjusted near the top of the sketch:

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
| `SLOW_SPEED` | Inner-wheel speed for gentle turns |
| `TURN_SPEED` | Outer-wheel speed for gentle turns |
| `HARD_TURN_SPEED` | Speed used for pivot turns and line recovery |
| `RFID_ACTION_TIME` | Watering duration in milliseconds |
| `CARD_REMOVED_TIME` | Time the tag must be absent before it may trigger again |
| `SEARCH_TIMEOUT` | Maximum time spent searching for a lost line |

## Line Sensor Behaviour

| Sensor pattern | Robot action |
|---|---|
| `1 1 0 1 1` | Move forward |
| `1 0 1 1 1` | Turn slightly left |
| `0 1 1 1 1` | Turn hard left |
| `1 1 1 0 1` | Turn slightly right |
| `1 1 1 1 0` | Turn hard right |
| `1 1 1 1 1` | Search using the last known turn direction |
| `0 0 0 0 0` | Stop |

Other mixed patterns are handled by checking whether the line appears on the left or right side.

## Testing

Test each subsystem before running the complete robot:

1. **IR sensors:** Move a black line under each sensor and check the five values in the Serial Monitor.
2. **Motors:** Lift the wheels off the ground and verify that forward, left, and right directions are correct.
3. **RFID:** Present a card and confirm that its UID is printed only once until the card is removed.
4. **Relay:** Test the relay without water and confirm that it turns on for five seconds.
5. **Pump:** Connect the pump and verify water flow, tubing, and power stability.
6. **Full route:** Place RFID tags at plant locations and test the complete track at a low speed first.

## Safety Notes

- Use a separate suitable power source for motors and the pump.
- Connect the ESP32, motor driver, RFID reader, and relay control circuit to a common ground.
- Keep the ESP32, motor driver, battery connections, and other electronics away from water.
- Check that the relay's voltage and current ratings are suitable for the pump.
- Add a flyback diode if the pump or relay module does not already include the required protection.
- Do not connect motor or pump power directly to an ESP32 pin.

## Troubleshooting

| Problem | Things to check |
|---|---|
| Robot turns in the wrong direction | Swap the affected motor wires or correct its direction pins |
| IR readings are reversed | Calibrate the sensor board or update the black/white logic |
| Robot loses the line frequently | Reduce speed, adjust sensor height, and tune the sensor sensitivity |
| RFID card is not detected | Check 3.3 V power, SPI wiring, and the `MFRC522` library |
| Relay works in reverse | Swap the `RELAY_ON` and `RELAY_OFF` values in the sketch |
| ESP32 restarts when the pump starts | Use a separate adequate supply, common ground, and electrical noise protection |
| Pump does not run | Check relay COM/NO wiring, pump voltage, power supply current, and tubing |

## Future Improvements

- Add a capacitive soil-moisture sensor at the watering nozzle
- Water only when soil moisture is below a configured threshold
- Replace the blocking five-second delay with non-blocking timing
- Add a water-level sensor to the tank
- Add Wi-Fi monitoring and a mobile dashboard
- Record watering history and plant status
- Add battery-voltage monitoring and automatic charging
- Use individual RFID UIDs to assign different watering times to different plants

## Project Structure

```text
ESP32-Smart-Farming-Robot/
├── smart_farming_robot.ino
├── README.md
└── images/                 # Optional project photos and wiring diagram
```

## License

You may add an open-source license such as the MIT License before publishing or accepting contributions.

---

Built for automated plant care using robotics, RFID, and the ESP32.
