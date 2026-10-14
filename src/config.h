#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// I2C LCD Configuration
#define LCD_I2C_ADDR          0x27
#define LCD_COLS              16
#define LCD_ROWS              2

// Servo Configuration
#define PIN_SERVO             10
#define SERVO_POS_LOCKED      0
#define SERVO_POS_UNLOCKED    90

// Indicator & Audio Pins
#define PIN_BUZZER            13
#define PIN_LED_RED           12
#define PIN_LED_GREEN         11

// Keypad Dimensions
#define KEYPAD_ROWS           4
#define KEYPAD_COLS           4

// Security Parameters
#define MASTER_PIN            "1234"
#define PIN_MAX_LEN           6
#define MAX_FAILED_ATTEMPTS   3
#define AUTO_LOCK_DELAY_MS    5000
#define LOCKOUT_DELAY_MS      30000

#endif // CONFIG_H