# System Working

1. **Initialization:** ESP32 creates a Wi-Fi Access Point (`RoverAP`).
2. **Connection:** The Flutter App connects via WebSocket (`ws://192.168.4.1/ws`).
3. **Control:** User Joystick inputs are sent as strings ("F", "B", "L", "R") to the ESP32.
4. **Execution:** ESP32 parses these strings and drives the L298N motor driver.
5. **Feedback:** Sensors (MQ-4, Metal) continuously poll data. If a threshold is crossed, the ESP32 pushes an alert ("METAL:1") back to the app immediately.
