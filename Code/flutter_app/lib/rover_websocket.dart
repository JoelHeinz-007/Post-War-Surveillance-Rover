import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:web_socket_channel/status.dart' as status;

class RoverWebSocket {
  WebSocketChannel? _channel;
  final String _url = 'ws://192.168.4.1/ws';
  bool isConnected = false;

  // Connect to the ESP32
  Stream<dynamic> connect() {
    try {
      _channel = WebSocketChannel.connect(Uri.parse(_url));
      isConnected = true;
      return _channel!.stream;
    } catch (e) {
      isConnected = false;
      return const Stream.empty();
    }
  }

  // Send a command string
  void send(String cmd) {
    if (_channel != null && isConnected) {
      _channel!.sink.add(cmd);
    }
  }

  void disconnect() {
    _channel?.sink.close(status.goingAway);
    isConnected = false;
  }
}
