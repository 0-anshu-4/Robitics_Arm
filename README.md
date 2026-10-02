# Robotic Arm Control

Arduino firmware for a 4-joint robotic arm (base, shoulder, elbow, gripper) driven by two analog joysticks through a **PCA9685** 16-channel PWM servo driver.

## Features

- Dual-joystick manual control of all four joints
- Mixed servo support: one positional servo (shoulder) and three continuous-rotation servos (base, elbow, gripper)
- Configurable joystick **deadzone** to prevent drift and jitter
- Calibratable neutral point for continuous-rotation servos
- One-directional gripper control (push forward to close)
- Live pulse values printed to the Serial Monitor for debugging

## Hardware

| Component | Qty | Notes |
|---|---|---|
| Arduino Uno / Nano | 1 | Any board with 4 analog inputs and I2C |
| PCA9685 PWM servo driver | 1 | Default I2C address `0x40` |
| Analog joystick module | 2 | X/Y axes used |
| Positional servo (e.g. MG996R) | 1 | Shoulder |
| Continuous-rotation servo | 3 | Base, elbow, gripper |
| External 5–6 V supply | 1 | Powers the servos via the PCA9685 V+ terminal |

## Wiring

**Joysticks → Arduino**

| Joystick axis | Arduino pin | Controls |
|---|---|---|
| Joystick 1 – X | A0 | Base rotation |
| Joystick 1 – Y | A1 | Shoulder |
| Joystick 2 – X | A2 | Elbow |
| Joystick 2 – Y | A3 | Gripper |

Joystick VCC → 5 V, GND → GND.

**PCA9685 → Arduino**

| PCA9685 | Arduino Uno/Nano |
|---|---|
| VCC | 5 V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

**Servos → PCA9685 channels**

| Channel | Joint | Servo type |
|---|---|---|
| 0 | Gripper | Continuous |
| 1 | Shoulder | Positional |
| 2 | Elbow | Continuous |
| 3 | Base | Continuous |

> ⚠️ Power the servos from an external supply on the PCA9685's V+ terminal, not from the Arduino 5 V pin. Connect all grounds together.

## Software Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the library: **Sketch → Include Library → Manage Libraries…** → search for **Adafruit PWM Servo Driver Library** → Install.
3. Open `SAEV_Robotic_Arm_Control.ino`, select your board and port, and upload.
4. Open the Serial Monitor at **9600 baud** to view live pulse values.

## Configuration

All tunable values are at the top of the sketch:

| Constant | Default | Purpose |
|---|---|---|
| `servoMin` / `servoMax` | 150 / 600 | Pulse limits for the positional (shoulder) servo |
| `contNeutral` | 350 | Pulse at which continuous servos stop — **calibrate this** |
| `contRange` | 175 | Speed range either side of neutral |
| `deadzone` | 50 | Joystick dead band around center (ADC counts) |
| `joyCenter` | 512 | Joystick rest value |

### Calibrating continuous servos

If a continuous servo creeps while the joystick is centered, adjust `contNeutral` up or down a few counts until it stays still, then re-upload. Watch the Serial Monitor to see the pulse being sent.

## How It Works

- **Positional servo (shoulder):** inside the deadzone the servo holds the mid position; outside it, the joystick position maps directly to an angle between `servoMin` and `servoMax`.
- **Continuous servos (base, elbow):** inside the deadzone the servo stops; pushing the stick sets speed and direction proportionally.
- **Gripper:** stays stopped unless the stick is pushed forward, which drives it in the closing direction.
- PWM runs at **50 Hz**, and the loop updates every 100 ms.

## Project Structure

```
SAEV_Robotic_Arm_Control/
├── SAEV_Robotic_Arm_Control.ino
└── README.md
```

> The Arduino IDE requires the `.ino` file to sit inside a folder with the same name.

## Future Improvements

- Add a gripper-open direction (pull-back on joystick 2 Y)
- Software limits for continuous joints using encoders or limit switches
- Smoother motion with acceleration ramping
- Wireless control via Bluetooth (HC-05) or NRF24L01

## License

MIT — free to use and modify.
