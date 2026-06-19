# Forge-ESP-Code

A small ESP-NOW-based remote servo controller using an ESP32 (transmitter) and an ESP8266 (receiver) with a PCA9685 PWM driver.

![Circuit](circuit_image.svg)

**Overview**

- Transmitter (`Code/32/32.ino`): Reads a potentiometer on the ESP32 and sends an angle value over ESP-NOW to the receiver.
- Receiver (`Code/mcu/mcu.ino`): Runs on an ESP8266, receives angle packets via ESP-NOW and updates up to five servos via an Adafruit PCA9685 (I2C PWM driver).

**Features**

- Low-latency ESP-NOW communication (no WiFi AP required).
- Change-detection on the transmitter to avoid spamming updates.
- PCA9685-driven servos for stable PWM and multi-servo control.

Hardware Required

- ESP32 development board (transmitter)
- ESP8266 development board (NodeMCU / Wemos D1) (receiver)
- Adafruit PCA9685 16-channel 12-bit PWM driver (I2C)
- Up to 5 hobby servos
- 10k potentiometer
- Wires, breadboard, and suitable servo power supply (do not power servos from the ESP board 3.3V regulator)

Suggested Wiring

- Transmitter (ESP32)
	- Potentiometer middle pin → `GPIO32`
	- Potentiometer outer pins → `3.3V` and `GND`

- Receiver (ESP8266 + PCA9685)
	- PCA9685 VCC → 3.3V or 5V (follow PCA9685 and servo power guidelines)
	- PCA9685 GND → common GND with ESP8266 and servo power
	- PCA9685 SDA → `D2` (Wire SDA pin used in `Wire.begin(D2, D1)`)
	- PCA9685 SCL → `D1` (Wire SCL pin used in `Wire.begin(D2, D1)`)
	- Servos → PCA9685 channels 0..4 (and servo power to external supply)

Note: The included `circuit_image.svg` in the repository root shows the expected connections. Replace it with your detailed circuit diagram if you have a specific layout.

Software & Libraries

- Transmitter (`Code/32/32.ino`): Uses `WiFi.h` and `esp_now.h` (ESP32 core)
- Receiver (`Code/mcu/mcu.ino`): Uses `ESP8266WiFi.h`, `espnow.h`, `Wire.h`, and `Adafruit_PWMServoDriver.h`

Install these libraries in the Arduino IDE or your build system before compiling the sketches.

How it works (brief)

- The transmitter reads an ADC value from the potentiometer on `POT_PIN` (ESP32 `GPIO32`), maps the 0–4095 ADC range to an angle range (0–160) and sends a compact `DataPacket` struct via ESP-NOW to the receiver MAC address specified in the sketch.
- The receiver maps the incoming angle to PCA9685 PWM pulse range (`SERVOMIN`..`SERVOMAX`) and writes the pulse to the first five PCA9685 channels.

Files

- `Code/32/32.ino` — ESP32 transmitter sketch (reads potentiometer, sends ESP-NOW packets).
- `Code/mcu/mcu.ino` — ESP8266 receiver sketch (receives ESP-NOW packets, updates PCA9685 PWM channels).
- `circuit_image.svg` — circuit diagram referenced by this README (in repo root).

Quick Setup (Arduino IDE)

1. Install the ESP32 and ESP8266 board definitions in the Arduino Boards Manager.
2. Install the `Adafruit PWM Servo Driver` library and any required ESP-NOW support libraries.
3. Open `Code/32/32.ino`, select your ESP32 board, compile and upload.
4. Open `Code/mcu/mcu.ino`, select your ESP8266 board, compile and upload.
5. Power the PCA9685 and servos from a suitable external supply. Verify I2C address and wiring.

Tips and Troubleshooting

- Ensure both boards are powered and share a common ground with the servo power supply.
- Verify the receiver MAC address in the transmitter `receiverMAC` array matches the receiver board (or set to broadcast/testing values).
- If servos jitter, check servo power, wiring, and PCA9685 PWM frequency (set to 50 Hz in the sketch).

License

This project is released under the MIT License — see the `LICENSE` file.

Contributions

Contributions, issues and feature requests are welcome. Please open a ticket or submit a PR.
