#ifndef LOCKER_FSM_H
#define LOCKER_FSM_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "config.h"

enum SystemState {
  STATE_LOCKED,
  STATE_ENTERING_PASSWORD,
  STATE_ACCESS_GRANTED,
  STATE_ACCESS_DENIED,
  STATE_SECURITY_LOCKOUT
};

class LockerSystem {
public:
  LockerSystem(LiquidCrystal_I2C &lcdRef, Servo &servoRef);
  void init();
  void processKey(char key);
  void update();

private:
  LiquidCrystal_I2C &lcd;
  Servo &servo;
  SystemState state;

  char inputBuffer[PIN_MAX_LEN + 1];
  uint8_t bufferIndex;
  uint8_t failedAttempts;
  unsigned long stateTimer;

  void resetBuffer();
  void showPrompt();
  void verifyCredentials();
  void unlock();
  void lock();
  void handleAccessDenied();
  void triggerLockout();
  void runLockoutAlert();
};

#endif // LOCKER_FSM_H