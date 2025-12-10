#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

// ---------- Wi‑Fi AP config ----------
const char* apSsid     = "RoverAP";
const char* apPassword = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ---------- Motor pins (L298N) ----------
const int ENA = 5;
const int IN1 = 18;
const int IN2 = 19;
const int ENB = 4;
const int IN3 = 25;
const int IN4 = 33;

// ---------- Sensors ----------
const int METAL_PIN = 34;   // metal detector digital OUT
const int MQ4_PIN   = 35;   // MQ‑4 AO (analog)

// ---------- Buzzer ----------
const int BUZZER_PIN = 26;

// ---------- Servo ----------
const int SERVO_PIN = 14;
Servo roverServo;

// ---------- Scan state for auto sweep ----------
bool isScanning = false;
unsigned long lastScanTime = 0;
int servoPos = 90;
int scanDirection = 1;      // 1 = right, -1 = left

// ---------- Motor helpers (full speed ON/OFF) ----------
void enableMotors(bool on) {
  digitalWrite(ENA, on ? HIGH : LOW);
  digitalWrite(ENB, on ? HIGH : LOW);
}

void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);   // left fwd
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);   // right fwd
  enableMotors(true);
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  enableMotors(true);
}

void turnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);    // left back
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);   // right fwd
  enableMotors(true);
}

void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);   // left fwd
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);    // right back
  enableMotors(true);
}

void stopMotors() {
  enableMotors(false);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ---------- Buzzer / sensors / servo helpers ----------
void setHorn(bool on) {
  digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
}

bool readMetal() {
  return digitalRead(METAL_PIN) == HIGH; 
}

int readGasRaw() {
  return analogRead(MQ4_PIN);
}

void setServoAngle(int angle) {
  angle = constrain(angle, 0, 180);
  roverServo.write(angle);
  delay(10); 
}

// ---------- Command handler ----------
void handleCommand(const String &raw) {
  String cmd = raw;
  cmd.trim();
  
  if      (cmd == "F")        forward();
  else if (cmd == "B")        backward();
  else if (cmd == "L")        turnLeft();
  else if (cmd == "R")        turnRight();
  else if (cmd == "STOP")     stopMotors();
  else if (cmd == "H1")       setHorn(true);
  else if (cmd == "H0")       setHorn(false);
  else if (cmd == "VS") {                 
    isScanning = true;
    Serial.println("Servo Auto-Scan STARTED");
  } 
  else if (cmd == "VSTOP") {             
    isScanning = false;
    Serial.println("Servo Auto-Scan STOPPED");
  } 
  else if (cmd.startsWith("V")) {        // e.g. V150
    int angle = cmd.substring(1).toInt();
    isScanning = false;                  // stop scan if manual override
    setServoAngle(angle);
  }
}

// ---------- WebSocket events ----------
void onWsEvent(AsyncWebSocket       *server,
               AsyncWebSocketClient *client,
               AwsEventType          type,
               void                 *arg,
               uint8_t              *data,
               size_t                len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WS client %u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WS client %u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      String msg;
      for (size_t i = 0; i < len; i++) msg += (char)data[i];
      msg.trim();
      handleCommand(msg);
    }
  }
}

// ---------- Sensor broadcasting ----------
bool lastMetal = false;
unsigned long lastGasMs = 0;

void broadcastSensors() {
  bool metal = readMetal();
  int gas    = readGasRaw();
  
  // Send Metal alert only on change
  if (metal != lastMetal) {
    lastMetal = metal;
    String m = String("METAL:") + (metal ? "1" : "0");
    ws.textAll(m);
  }
  
  // Send Gas data every 1 second
  unsigned long now = millis();
  if (now - lastGasMs > 1000) {
    lastGasMs = now;
    String g = String("MQ4:") + gas;
    ws.textAll(g);
  }
}

// ---------- setup & loop ----------
void setup() {
  Serial.begin(115200);
  
  // Motors
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotors();

  // Sensors
  pinMode(METAL_PIN, INPUT);
  pinMode(MQ4_PIN, INPUT);
  
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  setHorn(false);

  // Servo
  roverServo.attach(SERVO_PIN);
  setServoAngle(90);
  servoPos = 90;

  // Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  ws.cleanupClients();
  broadcastSensors();

  // Non-blocking auto scan
  if (isScanning) {
    unsigned long now = millis();
    if (now - lastScanTime > 50) {       // Move every 50ms
      lastScanTime = now;
      servoPos += 5 * scanDirection;
      if (servoPos >= 160) scanDirection = -1;
      if (servoPos <= 20)  scanDirection = 1;
      roverServo.write(servoPos);
    }
  }
}
