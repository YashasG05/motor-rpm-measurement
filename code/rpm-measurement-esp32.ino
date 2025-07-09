#include <WiFi.h>
#include <WebServer.h>

// Motor control pins (L298N)
#define IN1 13
#define IN2 14

// Encoder and Hall sensor pins
const int opticalEncoderPin = 15;
const int hallSensorPin = 4;
volatile int opticalPulseCount = 0;
volatile int hallPulseCount = 0;
float opticalRPM = 0;
float hallRPM = 0;
unsigned long lastTimeOptical = 0;
unsigned long lastTimeHall = 0;
unsigned long lastOpticalPulseTime = 0;
unsigned long lastHallPulseTime = 0;

const char* ssid = " ";
const char* password = " ";

WebServer server(80);

// Adjusted debounce intervals
const unsigned long debounceIntervalOptical = 2;
const unsigned long debounceIntervalHall = 100;  // Increased for stability

void IRAM_ATTR opticalEncoderISR() {
  unsigned long currentPulseTime = millis();
  if (currentPulseTime - lastOpticalPulseTime >= debounceIntervalOptical) {
    opticalPulseCount++;
    lastOpticalPulseTime = currentPulseTime;
  }
}

void IRAM_ATTR hallSensorISR() {
  unsigned long currentPulseTime = millis();
  if (currentPulseTime - lastHallPulseTime >= debounceIntervalHall) {
    hallPulseCount++;
    lastHallPulseTime = currentPulseTime;
  }
}

void moveMotorForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void setup() {
  Serial.begin(115200);

  // Wi-Fi Connection Setup with Reconnect Logic
  WiFi.begin(ssid, password);
  unsigned long startTime = millis();
  unsigned long timeout = 30000;

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection timed out. Running offline.");
  }

  // Web Server Routes
  server.on("/", HTTP_GET, []() {
    String html = "<html><head><title>Motor RPM</title>";
    html += "<script type='text/javascript'>function fetchRPM() {";
    html += "fetch('/opticalRPM').then(response => response.text()).then(data => { document.getElementById('opticalRPM').innerHTML = data; });";
    html += "fetch('/hallRPM').then(response => response.text()).then(data => { document.getElementById('hallRPM').innerHTML = data; });";
    html += "setTimeout(fetchRPM, 500); } window.onload = fetchRPM;</script></head>";
    html += "<body><h1>Optical Encoder RPM: <span id='opticalRPM'>Loading...</span></h1>";
    html += "<h1>Magnetic Encoder RPM: <span id='hallRPM'>Loading...</span></h1></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/opticalRPM", HTTP_GET, []() {
    server.send(200, "text/plain", String(opticalRPM));
  });

  server.on("/hallRPM", HTTP_GET, []() {
    server.send(200, "text/plain", String(hallRPM));
  });

  server.begin();
  Serial.println("Web Server started.");

  // Motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Sensor pins with pull-up and interrupts
  pinMode(opticalEncoderPin, INPUT_PULLUP);
  attachInterrupt(opticalEncoderPin, opticalEncoderISR, FALLING);

  pinMode(hallSensorPin, INPUT_PULLUP);
  attachInterrupt(hallSensorPin, hallSensorISR, FALLING);
}

void loop() {
  // Reconnect Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi...");
    delay(1000);
  }

  server.handleClient();

  // RPM calculation every 1 second
  unsigned long currentTime = millis();
  if (currentTime - lastTimeOptical >= 1000) {
    opticalRPM = (opticalPulseCount * 60.0) / (360 * 0.5);
    opticalPulseCount = 0;
    lastTimeOptical = currentTime;
  }

  if (currentTime - lastTimeHall >= 1000) {
    hallRPM = (hallPulseCount * 60.0) / 0.5;
    hallPulseCount = 0;
    lastTimeHall = currentTime;
  }

  moveMotorForward();  // Keep motor running forward
}
