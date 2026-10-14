# Finite State Machine Specifications

The system runs a finite state machine composed of 5 discrete conditions:

1. **STATE_LOCKED**: Default state. Actuator at 0 degrees, indicators low, awaiting numeric keypad triggers.
2. **STATE_ENTERING**: Active buffer accumulation. Each incoming key is added to the char buffer and an asterisk is displayed on the LCD.
3. **STATE_GRANTED**: Occurs when input buffer matches `MASTER_PASSWORD`. Servo turns to 90 degrees, green LED triggers, auto-relatch counter begins.
4. **STATE_DENIED**: Occurs when password mismatch is detected. Red LED lights up, buzzer emits a short alert tone, failed counter increases.
5. **STATE_LOCKOUT**: Triggered if failed attempts reach 3. System enters a 30-second loop with audible/visual sirens and rejects all inputs until the timer expires.