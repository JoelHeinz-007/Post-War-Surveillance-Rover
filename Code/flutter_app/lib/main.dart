import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'controller_ui.dart';

void main() {
  runApp(const RoverApp());
}

class RoverApp extends StatelessWidget {
  const RoverApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Rover Command',
      theme: ThemeData(
        brightness: Brightness.dark,
        primaryColor: Colors.tealAccent,
        textTheme: GoogleFonts.robotoMonoTextTheme(Theme.of(context).textTheme).apply(
          bodyColor: Colors.white,
          displayColor: Colors.white,
        ),
      ),
      home: const ControllerUI(),
    );
  }
}
