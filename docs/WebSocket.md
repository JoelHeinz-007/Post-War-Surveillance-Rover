# Communication Protocol

**Type:** Asynchronous WebSocket
**Server:** ESP32 (AsyncWebServer)
**Client:** Flutter (Dart WebSocketChannel)

| Direction | Command | Action |
|-----------|---------|--------|
| App -> Rover | `F` | Forward |
| App -> Rover | `B` | Backward |
| App -> Rover | `VS` | Start Auto-Scan |
| Rover -> App | `METAL:1` | Mine Found Alert |
| Rover -> App | `MQ4:450` | Gas Sensor Value |
