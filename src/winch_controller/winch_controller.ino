#include <Servo.h>
#include <EEPROM.h>

Servo myServo;

// Pins
const int potPin = A0;   
const int servoPin = 9;  

// Settings
struct Settings {
  int potMin;
  int potMax;
  int servoMin;
  int servoMax;
  int maxSpeed;  // deg / tick
};

Settings config;

// Address in EEPROM
const int EEPROM_ADDR = 0;

// Timer
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 20; // 20 ms ≈ 50 Hz

// Curent and target position
int currentAngle = 90;  
int targetAngle = 90;   

void loadSettings() {
  EEPROM.get(EEPROM_ADDR, config);

  if (config.potMin == 0xFFFF || config.potMax == 0xFFFF) {
    config.potMin = 100;
    config.potMax = 900;
    config.servoMin = 20;
    config.servoMax = 160;
    config.maxSpeed = 2;
    saveSettings();
  }
}

void saveSettings() {
  EEPROM.put(EEPROM_ADDR, config);
}

void updateServoFromPot() {
  int potValue = analogRead(potPin);
  potValue = constrain(potValue, config.potMin, config.potMax);

  targetAngle = map(potValue, config.potMin, config.potMax, config.servoMin, config.servoMax);

  int delta = targetAngle - currentAngle;

  if (abs(delta) <= config.maxSpeed) {
    currentAngle = targetAngle; 
  } else {
    currentAngle += (delta > 0 ? config.maxSpeed : -config.maxSpeed);
  }

  myServo.write(currentAngle);
}

void setup() {
  myServo.attach(servoPin);

  loadSettings();

  currentAngle = (config.servoMin + config.servoMax) / 2;
  targetAngle = currentAngle;
  myServo.write(currentAngle);
}

void loop() {
  unsigned long now = millis();

  if (now - lastUpdate >= updateInterval) {
    lastUpdate = now;
    updateServoFromPot();  
  }
}
