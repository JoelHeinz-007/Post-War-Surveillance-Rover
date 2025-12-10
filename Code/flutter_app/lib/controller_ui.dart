import 'package:flutter/material.dart';
import 'package:flutter_joystick/flutter_joystick.dart';
import 'rover_websocket.dart';

class ControllerUI extends StatefulWidget {
  const ControllerUI({super.key});
  @override
  State<ControllerUI> createState() => _ControllerUIState();
}

class _ControllerUIState extends State<ControllerUI> {
  final RoverWebSocket _rover = RoverWebSocket();
  String _status = "Disconnected";
  String _sensorGas = "0";
  bool _metalDetected = false;

  @override
  void initState() {
    super.initState();
    _connectRover();
  }

  void _connectRover() {
    _rover.connect().listen((message) {
      setState(() {
        _status = "Connected";
        if (message.toString().startsWith("METAL:1")) _metalDetected = true;
        if (message.toString().startsWith("METAL:0")) _metalDetected = false;
        if (message.toString().startsWith("MQ4:")) {
          _sensorGas = message.toString().split(":")[1];
        }
      });
    }, onError: (error) {
      setState(() => _status = "Error: Retry");
    });
  }

  void _handleJoystick(StickDragDetails details) {
    if (details.y < -0.5) _rover.send("F");
    else if (details.y > 0.5) _rover.send("B");
    else if (details.x < -0.5) _rover.send("L");
    else if (details.x > 0.5) _rover.send("R");
    else _rover.send("STOP");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: _metalDetected ? Colors.red.shade900 : Colors.black,
      appBar: AppBar(title: const Text("⚔️ DEFENSE ROVER"), backgroundColor: Colors.grey[900]),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        children: [
          // Status Panel
          Container(
            padding: const EdgeInsets.all(15),
            margin: const EdgeInsets.symmetric(horizontal: 20),
            decoration: BoxDecoration(
              border: Border.all(color: Colors.tealAccent),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text("STATUS: $_status"),
                Text("GAS: $_sensorGas PPM"),
              ],
            ),
          ),
          
          if (_metalDetected)
            const Text("⚠️ MINE DETECTED! ⚠️", style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold, color: Colors.yellow)),

          // Controls
          Joystick(listener: _handleJoystick, mode: JoystickMode.all),
          
          // Aux Buttons
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              ElevatedButton(onPressed: () => _rover.send("VS"), child: const Text("AUTO SCAN")),
              const SizedBox(width: 10),
              ElevatedButton(onPressed: () => _rover.send("H1"), child: const Text("HORN ON")),
              const SizedBox(width: 10),
              ElevatedButton(onPressed: () => _rover.send("H0"), child: const Text("OFF")),
            ],
          )
        ],
      ),
    );
  }
}
