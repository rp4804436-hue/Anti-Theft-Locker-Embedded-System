#include "LockerFSM.h"

LockerSystem::LockerSystem(LiquidCrystal_I2C &lcdRef, Servo &servoRef)
  : lcd(lcdRef), servo(servoRef), state(STATE_LOCKED),
    bufferIndex(0), failedAttempts(0), stateTimer(0) {}

void LockerSystem::init() {
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);

  servo.attach(PIN_SERVO);
  lock();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ANTI-THEFT LOCK");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM SECURE");
  delay(1200);

  resetBuffer();
  showPrompt();
}

void LockerSystem::resetBuffer() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  bufferIndex = 0;
}

void LockerSystem::showPrompt() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER PIN:");
  lcd.setCursor(0, 1);
}

void LockerSystem::lock() {
  servo.write(SERVO_POS_LOCKED);
}

void LockerSystem::unlock() {
  servo.write(SERVO_POS_UNLOCKED);
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_RED, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");
  lcd.setCursor(0, 1);
  lcd.print("LOCKER OPEN");
}

void LockerSystem::handleAccessDenied() {
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(200);
  digitalWrite(PIN_BUZZER, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WRONG PASSWORD!");
  lcd.setCursor(0, 1);
  lcd.print("LEFT: ");
  lcd.print(MAX_FAILED_ATTEMPTS - failedAttempts);

  stateTimer = millis();
  state = STATE_ACCESS_DENIED;
}

void LockerSystem::triggerLockout() {
  state = STATE_SECURITY_LOCKOUT;
  stateTimer = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEM LOCKOUT!");
  lcd.setCursor(0, 1);
  lcd.print("SECURITY ALERT");

  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
}

void LockerSystem::runLockoutAlert() {
  unsigned long elapsed = millis() - stateTimer;

  // Modulate siren and LED every 250ms
  if ((elapsed / 250) % 2 == 0) {
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_LED_RED, HIGH);
  } else {
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED_RED, LOW);
  }

  if (elapsed >= LOCKOUT_DELAY_MS) {
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    failedAttempts = 0;
    resetBuffer();
    showPrompt();
    state = STATE_LOCKED;
  }
}

void LockerSystem::verifyCredentials() {
  if (strcmp(inputBuffer, MASTER_PIN) == 0) {
    failedAttempts = 0;
    unlock();
    stateTimer = millis();
    state = STATE_ACCESS_GRANTED;
  } else {
    failedAttempts++;
    if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
      triggerLockout();
    } else {
      handleAccessDenied();
    }
  }
}

void LockerSystem::processKey(char key) {
  if (!key) return;

  if (state == STATE_LOCKED || state == STATE_ENTERING_PASSWORD) {
    if (key == '#') {
      if (bufferIndex > 0) {
        verifyCredentials();
      }
    } else if (key == '*') {
      resetBuffer();
      showPrompt();
      state = STATE_LOCKED;
    } else if (bufferIndex < PIN_MAX_LEN) {
      inputBuffer[bufferIndex++] = key;
      inputBuffer[bufferIndex] = '\0';

      lcd.setCursor(bufferIndex - 1, 1);
      lcd.print('*');
      state = STATE_ENTERING_PASSWORD;
    }
  }
}

void LockerSystem::update() {
  unsigned long currentMillis = millis();

  switch (state) {
    case STATE_ACCESS_GRANTED:
      if (currentMillis - stateTimer >= AUTO_LOCK_DELAY_MS) {
        lock();
        digitalWrite(PIN_LED_GREEN, LOW);
        resetBuffer();
        showPrompt();
        state = STATE_LOCKED;
      }
      break;

    case STATE_ACCESS_DENIED:
      if (currentMillis - stateTimer >= 2000) {
        digitalWrite(PIN_LED_RED, LOW);
        resetBuffer();
        showPrompt();
        state = STATE_LOCKED;
      }
      break;

    case STATE_SECURITY_LOCKOUT:
      runLockoutAlert();
      break;

    default:
      break;
  }
}