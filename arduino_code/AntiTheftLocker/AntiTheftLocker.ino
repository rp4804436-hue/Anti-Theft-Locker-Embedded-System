/**
 * @file AntiTheftLocker.ino
 * @brief Self-contained firmware executable for Arduino IDE / Wokwi
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// Hardware Pin Definitions
const uint8_t PIN_SERVO     = 10;
const uint8_t PIN_BUZZER    = 13;
const uint8_t PIN_LED_RED   = 12;
const uint8_t PIN_LED_GREEN = 11;

// Security and Timing Settings
const char MASTER_PASSWORD[]        = "1234";
const uint8_t PASSWORD_MAX_LEN       = 6;
const uint8_t MAX_FAILED_ATTEMPTS   = 3;
const unsigned long AUTO_LOCK_MS    = 5000;
const unsigned long LOCKOUT_MS      = 30000;

// LCD & Servo Instantiations
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockerServo;

// Keypad Configuration
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// State Engine
enum SystemState {
  STATE_LOCKED,
  STATE_ENTERING,
  STATE_GRANTED,
  STATE_DENIED,
  STATE_LOCKOUT
};

SystemState currentState = STATE_LOCKED;
char enteredPassword[PASSWORD_MAX_LEN + 1];
uint8_t charIndex = 0;
uint8_t failedAttempts = 0;
unsigned long timerAnchor = 0;

void resetEntryBuffer() {
  memset(enteredPassword, 0, sizeof(enteredPassword));
  charIndex = 0;
}

void printIdleScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER PIN:");
  lcd.setCursor(0, 1);
}

void lockActuator() {
  lockerServo.write(0);
}

void unlockActuator() {
  lockerServo.write(90);
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_RED, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");
  lcd.setCursor(0, 1);
  lcd.print("LOCKER OPEN");
}

void showWrongPassword() {
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(150);
  digitalWrite(PIN_BUZZER, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WRONG PASSWORD!");
  lcd.setCursor(0, 1);
  lcd.print("ATTEMPTS LEFT: ");
  lcd.print(MAX_FAILED_ATTEMPTS - failedAttempts);

  timerAnchor = millis();
  currentState = STATE_DENIED;
}

void enterLockout() {
  Serial.println(F("[ALERT] 3 Failed attempts registered. Lockout started."));
  currentState = STATE_LOCKOUT;
  timerAnchor = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEM LOCKOUT!");
  lcd.setCursor(0, 1);
  lcd.print("PLEASE WAIT...");

  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
}

void executeVerification() {
  Serial.print(F("[AUTH] Comparing input: "));
  Serial.println(enteredPassword);

  if (strcmp(enteredPassword, MASTER_PASSWORD) == 0) {
    Serial.println(F("[AUTH] PIN verified successfully."));
    failedAttempts = 0;
    unlockActuator();
    timerAnchor = millis();
    currentState = STATE_GRANTED;
  } else {
    failedAttempts++;
    Serial.print(F("[AUTH] Verification failed. Attempt: "));
    Serial.println(failedAttempts);

    if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
      enterLockout();
    } else {
      showWrongPassword();
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);

  lockerServo.attach(PIN_SERVO);
  lockActuator();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ANTI-THEFT LOCK");
  lcd.setCursor(0, 1);
  lcd.print("INITIALIZING...");
  delay(1200);

  resetEntryBuffer();
  printIdleScreen();
  Serial.println(F("[INIT] Firmware execution initialized."));
}

void loop() {
  char key = keypad.getKey();
  unsigned long now = millis();

  switch (currentState) {
    case STATE_LOCKED:
    case STATE_ENTERING:
      if (key) {
        Serial.print(F("[KEYPAD] "));
        Serial.println(key);

        if (key == '#') {
          if (charIndex > 0) {
            executeVerification();
          }
        } else if (key == '*') {
          resetEntryBuffer();
          printIdleScreen();
          currentState = STATE_LOCKED;
        } else if (charIndex < PASSWORD_MAX_LEN) {
          enteredPassword[charIndex++] = key;
          enteredPassword[charIndex] = '\0';

          lcd.setCursor(charIndex - 1, 1);
          lcd.print('*');
          currentState = STATE_ENTERING;
        }
      }
      break;

    case STATE_GRANTED:
      if (now - timerAnchor >= AUTO_LOCK_MS) {
        Serial.println(F("[LOCK] Auto-lock interval reached. Re-engaging."));
        lockActuator();
        digitalWrite(PIN_LED_GREEN, LOW);
        resetEntryBuffer();
        printIdleScreen();
        currentState = STATE_LOCKED;
      }
      break;

    case STATE_DENIED:
      if (now - timerAnchor >= 2000) {
        digitalWrite(PIN_LED_RED, LOW);
        resetEntryBuffer();
        printIdleScreen();
        currentState = STATE_LOCKED;
      }
      break;

    case STATE_LOCKOUT:
      unsigned long elapsed = now - timerAnchor;
      if ((elapsed / 250) % 2 == 0) {
        digitalWrite(PIN_BUZZER, HIGH);
        digitalWrite(PIN_LED_RED, HIGH);
      } else {
        digitalWrite(PIN_BUZZER, LOW);
        digitalWrite(PIN_LED_RED, LOW);
      }

      if (elapsed >= LOCKOUT_MS) {
        digitalWrite(PIN_BUZZER, LOW);
        digitalWrite(PIN_LED_RED, LOW);
        failedAttempts = 0;
        resetEntryBuffer();
        printIdleScreen();
        currentState = STATE_LOCKED;
      }
      break;
  }
}