#include <Ultrasonic.h>
#include <LiquidCrystal_I2C.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Motor Pins
const int motorIn1 = 5;
const int motorIn2 = 6;
const int motorEnable = 9;

// Ultrasonic Sensor
const int trigPin = 10;
const int echoPin = 8;
Ultrasonic ultrasonic(trigPin, echoPin);

// LEDs
const int led1 = 13;
const int led2 = 12;

// Analog Buttons
const int btn_inc = A0;
const int btn_dec = A1;
const int btn_cancel = A2;
const int btn_set = A3;
const int btn_acc = A4;

// Encoder
volatile int pulseCount = 0;
const int pulsesPerRevolution = 20;
float actualRPM = 0;
unsigned long lastRPMCalcTime = 0;

// System Variables
int speed = 0;
int set_speed = 0;
float distance = 0;
int mode = 0; // 0: Normal, 1: Cruise, 2: ACC
int constant = 0;
bool blinkState = false;
unsigned long previousMillis = 0;
const long blinkInterval = 200;
unsigned long lastDisplayUpdate = 0;
const long displayInterval = 300;
int pwmValue = 0;

// Debounce flags
bool incFlag = false, decFlag = false, accFlag = false;
bool setFlag = false, cancelFlag = false;

void countPulse() {
  static byte divider = 0;
  if (++divider >= 8) {
    pulseCount++;
    divider = 0;
  }
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), countPulse, FALLING);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACC System Start");
  delay(1000);
  lcd.clear();

  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorEnable, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void updateDisplay() {
  lcd.clear();

  // Line 1: Mode and Speed
  lcd.setCursor(0, 0);
  lcd.print((mode == 0) ? "Normal " : (mode == 1) ? "Cruise " : "ACC    ");
  lcd.print("S:");
  lcd.print(speed);

  // Line 2: RPM, PWM, and Distance
  lcd.setCursor(0, 1);
  lcd.print("R:");
  lcd.print((int)actualRPM);
  lcd.print(" P:");
  lcd.print(pwmValue);
  lcd.print(" D:");
  lcd.print((int)distance);

  // Serial Output for debugging
  Serial.print("Mode: ");
  Serial.print((mode == 0) ? "Normal" : (mode == 1) ? "Cruise" : "ACC");
  Serial.print(" | Speed: ");
  Serial.print(speed);
  Serial.print(" | RPM: ");
  Serial.print(actualRPM);
  Serial.print(" | PWM: ");
  Serial.print(pwmValue);
  Serial.print(" | Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void checkButtons() {
  int incVal = analogRead(btn_inc);
  int decVal = analogRead(btn_dec);
  int cancelVal = analogRead(btn_cancel);
  int setVal = analogRead(btn_set);
  int accVal = analogRead(btn_acc);

  if (incVal > 600 && !incFlag) {
    speed++;
    incFlag = true;
  } else if (incVal < 100) {
    incFlag = false;
  }

  if (decVal > 600 && !decFlag) {
    speed--;
    decFlag = true;
  } else if (decVal < 100) {
    decFlag = false;
  }

  if (setVal > 600 && !setFlag) {
    mode = 1;
    set_speed = speed;
    setFlag = true;
  } else if (setVal < 100) {
    setFlag = false;
  }

  if (accVal > 600 && !accFlag) {
    mode = 2;
    constant = speed;
    set_speed = speed;
    accFlag = true;
  } else if (accVal < 100) {
    accFlag = false;
  }

  if (cancelVal > 600 && !cancelFlag) {
    mode = 0;
    cancelFlag = true;
  } else if (cancelVal < 100) {
    cancelFlag = false;
  }
}

void loop() {
  unsigned long currentMillis = millis();
  distance = ultrasonic.read(); // in cm

  checkButtons();

  // --- Mode Logic ---
  if (mode == 0) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);

  } else if (mode == 1) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    speed = set_speed;

  } else if (mode == 2) {
    if (distance <= 20) {
      if (currentMillis - previousMillis >= blinkInterval) {
        previousMillis = currentMillis;
        blinkState = !blinkState;
        digitalWrite(led1, blinkState);
        digitalWrite(led2, blinkState);
      }
      if (speed > 0) speed--;
    } else {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      if (speed < set_speed) speed++;
    }
    if (speed > constant) speed = constant;
  }

  // Clamp and Map
  speed = constrain(speed, 0, 100);
  pwmValue = map(speed, 0, 100, 0, 255);

  // Motor Control
  if (speed > 0) {
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorEnable, pwmValue);
  } else {
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorEnable, 0);
  }

  // RPM Calculation
  if (currentMillis - lastRPMCalcTime >= 1000) {
    noInterrupts();
    int pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    actualRPM = (pulses * 60.0) / pulsesPerRevolution;
    lastRPMCalcTime = currentMillis;
  }

  // LCD Update
  if (currentMillis - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = currentMillis;
    updateDisplay();
  }

  delay(10);
}