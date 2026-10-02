/*
 * SAEV Robotic Arm Control
 * ------------------------
 * Controls a 4-joint robotic arm (base, shoulder, elbow, gripper) using two
 * analog joysticks and a PCA9685 16-channel PWM servo driver over I2C.
 *
 * Hardware:
 *   - Arduino Uno / Nano (or any board with 4 analog inputs + I2C)
 *   - PCA9685 PWM servo driver (default I2C address 0x40)
 *   - 2x analog joysticks (X/Y axes)
 *   - 1x positional servo (shoulder), 3x continuous-rotation servos
 *     (base, elbow, gripper)
 *
 * Library: Adafruit PWM Servo Driver Library
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// ---- Joystick pins ----
const int joy1_X = A0;   // Base rotation
const int joy1_Y = A1;   // Shoulder
const int joy2_X = A2;   // Elbow
const int joy2_Y = A3;   // Gripper

// ---- PCA9685 channels ----
const uint8_t CH_GRIPPER  = 0;
const uint8_t CH_SHOULDER = 1;
const uint8_t CH_ELBOW    = 2;
const uint8_t CH_BASE     = 3;

// ---- Positional servo limits (PCA9685 ticks out of 4096) ----
const int servoMin = 150;
const int servoMax = 600;

// ---- Continuous rotation servo calibration ----
int contNeutral = 350;      // pulse at which the servo stops (calibrate per servo)
const int contRange = 175;  // full-speed range either side of neutral

// ---- Joystick settings ----
const int joyCenter = 512;  // ADC mid-point of a 10-bit reading
const int deadzone  = 50;   // ignore small stick movements around center

// ---- Mapping Functions ----

// Positional servo: hold center inside deadzone, otherwise map full stick range.
int mapWithDeadzone(int val, int center, int dz, int minOut, int maxOut) {
  if (abs(val - center) <= dz) {
    return (minOut + maxOut) / 2;  // stay centered
  }
  return map(val, 0, 1023, minOut, maxOut);
}

// Continuous servo: stop inside deadzone, forward/reverse speed outside it.
int mapContinuous(int val, int center, int dz) {
  if (abs(val - center) <= dz) {
    return contNeutral;  // stop
  } else if (val > center) {  // forward
    return map(val, center + dz, 1023,
               contNeutral, contNeutral + contRange);
  } else {  // reverse
    return map(val, 0, center - dz,
               contNeutral - contRange, contNeutral);
  }
}

// ---- Setup ----
void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz standard servo frequency
  delay(10);
}

// ---- Main Loop ----
void loop() {
  // Read joystick values
  int baseVal     = analogRead(joy1_X);
  int shoulderVal = analogRead(joy1_Y);
  int elbowVal    = analogRead(joy2_X);
  int gripperVal  = analogRead(joy2_Y);

  // Map joystick -> servo outputs
  int basePulse     = mapContinuous(baseVal, joyCenter, deadzone);
  int shoulderPulse = mapWithDeadzone(shoulderVal, joyCenter, deadzone,
                                      servoMin, servoMax);
  int elbowPulse    = mapContinuous(elbowVal, joyCenter, deadzone);

  // Gripper only closes when joystick is pushed forward
  int gripperPulse = contNeutral;  // default = neutral (stopped)
  if (gripperVal > (joyCenter + deadzone)) {
    // Flipped direction: push forward = close
    gripperPulse = map(gripperVal, joyCenter + deadzone, 1023,
                       contNeutral, contNeutral - contRange);
  }
  // If joystick is centered or pulled back, gripper stays neutral

  // Send pulses to PCA9685 channels
  pwm.setPWM(CH_GRIPPER,  0, gripperPulse);
  pwm.setPWM(CH_SHOULDER, 0, shoulderPulse);
  pwm.setPWM(CH_ELBOW,    0, elbowPulse);
  pwm.setPWM(CH_BASE,     0, basePulse);

  // Debugging values in Serial Monitor
  Serial.print("Base: ");
  Serial.print(basePulse);
  Serial.print(" | Shoulder: ");
  Serial.print(shoulderPulse);
  Serial.print(" | Elbow: ");
  Serial.print(elbowPulse);
  Serial.print(" | Gripper: ");
  Serial.println(gripperPulse);

  delay(100);
}
