# Sensor Suite

## 1. Metal Detection (Inductive Sensor)
- **Model:** Standard Inductive Proximity / Coil setup.
- **Purpose:** Detects metallic landmines or buried UXO.
- **Logic:** Triggers interrupt on ESP32 when metal threshold is crossed.

## 2. Gas Detection
- **Model:** MQ-4 / MQ-135 Sensor.
- **Purpose:** Detects Methane, CO, and post-blast toxic fumes.
- **Safety:** Triggers localized buzzer and app alert if PPM levels exceed safety limits.

## 3. Servo Scanning
- **Hardware:** SG90 Micro Servo.
- **Function:** Rotates the sensor head 180 degrees to sweep the area (Radar mode).
