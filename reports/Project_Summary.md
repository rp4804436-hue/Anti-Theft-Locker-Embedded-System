# Project Summary Report: Anti-Theft Locker System

## Overview
This project presents an embedded electronic safe-locking solution developed with an Arduino UNO controller. It combines matrix keypad entry, I2C visual feedback, servo-driven deadbolt actuation, and security lockout defenses to stop brute-force attacks.

## Performance Analysis
- **Authentication Response Time**: ~12 ms from '#' submission key to servo signal generation.
- **Debounce Efficiency**: Stable key reads via software interval debouncing (no false multiple inputs observed).
- **Current Draw**: ~45 mA during standby state; ~220 mA momentary peak when the servo motor is active.
- **Fail-Safe Mechanism**: The servo returns to 0 degrees on power boot, keeping the locker secured after a power cycle.