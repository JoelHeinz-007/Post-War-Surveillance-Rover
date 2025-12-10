# Rover Controller App 📱

## Setup
1. Ensure Flutter SDK is installed.
2. Run `flutter pub get`.
3. Connect mobile Wi-Fi to `RoverAP`.
4. Run `flutter run`.

## Architecture
- **State Management:** StatefulWidget
- **Networking:** WebSocketChannel (Single instance pattern)
- **Protocol:** Custom text-based commands (F, B, L, R, STOP).
