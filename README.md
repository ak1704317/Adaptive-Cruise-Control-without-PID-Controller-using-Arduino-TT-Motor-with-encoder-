# Adaptive-Cruise-Control-without-PID-Controller-using-Arduino-TT-Motor-with-encoder-


This project implements a complete **Adaptive Cruise Control (ACC)** system for a small vehicle using **Arduino Uno**, **DC motor with encoder**, **ultrasonic sensor**, **5-button interface**, **LEDs**, and **I2C LCD**.

It supports:
- **Normal mode**: Manual speed control
- **Cruise mode**: Maintain a fixed speed
- **ACC mode**: Dynamically adjust speed based on distance to obstacles

---

##  Components Used

| Component                 | Description                          |
|--------------------------|--------------------------------------|
| Arduino Uno              | Main microcontroller                 |
| TT Motor + Encoder       | DC motor with encoder (20 pulses/rev)|
| L298N / Motor Driver     | Controls motor direction + PWM       |
| Ultrasonic Sensor (HC-SR04) | For distance sensing              |
| I2C 16x2 LCD Display     | Shows mode, speed, RPM, distance     |
| Push Buttons (x5)        | For control (speed, mode switching)  |
| LEDs (x2)                | Show active mode/status              |

---

## Pin Mapping

| Function          | Arduino Pin |
|------------------|-------------|
| Motor IN1        | D5          |
| Motor IN2        | D6          |
| Motor ENA (PWM)  | D9          |
| Encoder Signal A | D2 (Interrupt) |
| Trig (Ultrasonic)| D10         |
| Echo (Ultrasonic)| D8          |
| I2C SDA          | A4          |
| I2C SCL          | A5          |
| LED1             | D13         |
| LED2             | D12         |
| Button: Increase | A0          |
| Button: Decrease | A1          |
| Button: Cancel   | A2          |
| Button: Set      | A3          |
| Button: ACC Mode | A4          |

---

##  Operating Modes

###  Mode 0 – Normal Mode
- Default mode (after cancel)
- Manual control of speed using **Increase (A0)** and **Decrease (A1)** buttons
- LED1 = OFF, LED2 = ON

###  Mode 1 – Cruise Mode
- Activated with **Set (A3)** button
- Locks current speed as `set_speed`
- Speed remains constant unless buttons are pressed
- LED1 = ON, LED2 = OFF

###  Mode 2 – ACC Mode
- Activated with **ACC (A4)** button
- Sets speed = `constant` but slows down if object detected within 20 cm
- Increases speed again when path is clear
- LEDs blink when object is too close, otherwise steady ON

---


## Key Features in Code

- **Encoder-based RPM Measurement**  
  Uses interrupt on D2 to count pulses and calculate real RPM every second.

- **PWM-based Motor Speed Control**  
  Maps `speed (0–100)` to `PWM (0–255)` for motor.

- **Debounced Analog Button Input**  
  Reads A0–A4 analog values and uses thresholds to detect button presses without false triggering.

- **Adaptive Distance Control**  
  Uses `ultrasonic.read()` to slow/stop motor based on proximity.

- **LCD Status Updates**  
  Updated every 300 ms for performance + readability.

---



##  Installation

1. Open Arduino IDE
2. Install Libraries:
   - `Ultrasonic by Erick Simões`
   - `LiquidCrystal_I2C by Frank de Brabander`
3. Select board: **Arduino Uno**
4. Upload code to the board
5. Open Serial Monitor @ 9600 baud

---

##  Notes

- Encoder pulses are divided in software to reduce false counts (`divider = 8`)
- You may adjust `pulsesPerRevolution` for your motor (default = 20)
- Ensure motor power is handled externally if current draw is high
- Pull-down resistors or debounce caps may be used for cleaner encoder signal

---

##  Future Work

- Add EEPROM to save last mode
- Use OLED instead of LCD
- Show set speed and real-time distance trend
- Enable PID control for more accurate speed locking

---

##  Author

Developed by **Akash Kumar**  
Final year Embedded Systems Project – ACC with Encoder Feedback, Distance Sensing & Real-time Display

---


