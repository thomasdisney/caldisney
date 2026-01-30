#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char *ssid = "disney";
const char *password = "tommyboy";

WebServer server(80);

constexpr int SERVO_LEFT_LEG = 12;
constexpr int SERVO_RIGHT_LEG = 13;
constexpr int SERVO_LEFT_FOOT = 14;
constexpr int SERVO_RIGHT_FOOT = 15;
constexpr int SERVO_HEAD = 2;

Servo leftLeg;
Servo rightLeg;
Servo leftFoot;
Servo rightFoot;
Servo headServo;

const int SERVO_MIN_US = 500;
const int SERVO_MAX_US = 2400;

int headOffset = 0;

void setServoPositions(int leftLegPos, int rightLegPos, int leftFootPos, int rightFootPos) {
  leftLeg.write(leftLegPos);
  rightLeg.write(rightLegPos);
  leftFoot.write(leftFootPos);
  rightFoot.write(rightFootPos);
}

void neutralPose() {
  setServoPositions(90, 90, 90, 90);
}

void stopMotion() {
  neutralPose();
}

void forwardStep() {
  setServoPositions(70, 110, 95, 85);
  delay(250);
  setServoPositions(110, 70, 85, 95);
  delay(250);
  neutralPose();
}

void backwardStep() {
  setServoPositions(110, 70, 95, 85);
  delay(250);
  setServoPositions(70, 110, 85, 95);
  delay(250);
  neutralPose();
}

void pivotLeft() {
  setServoPositions(70, 70, 100, 80);
  delay(300);
  neutralPose();
}

void pivotRight() {
  setServoPositions(110, 110, 80, 100);
  delay(300);
  neutralPose();
}

void danceWave() {
  for (int i = 0; i < 2; i++) {
    setServoPositions(60, 120, 90, 90);
    delay(200);
    setServoPositions(120, 60, 90, 90);
    delay(200);
  }
  neutralPose();
}

void danceBounce() {
  for (int i = 0; i < 3; i++) {
    setServoPositions(90, 90, 70, 110);
    delay(180);
    setServoPositions(90, 90, 110, 70);
    delay(180);
  }
  neutralPose();
}

void danceShimmy() {
  for (int i = 0; i < 3; i++) {
    setServoPositions(80, 100, 80, 100);
    delay(180);
    setServoPositions(100, 80, 100, 80);
    delay(180);
  }
  neutralPose();
}

void danceSpin() {
  pivotLeft();
  pivotRight();
  pivotLeft();
  neutralPose();
}

void handleCommand() {
  String command = server.arg("command");
  String value = server.arg("value");

  if (command == "forward") {
    forwardStep();
  } else if (command == "backward") {
    backwardStep();
  } else if (command == "left") {
    pivotLeft();
  } else if (command == "right") {
    pivotRight();
  } else if (command == "stop") {
    stopMotion();
  } else if (command == "center") {
    neutralPose();
  } else if (command == "dance-wave") {
    danceWave();
  } else if (command == "dance-bounce") {
    danceBounce();
  } else if (command == "dance-shimmy") {
    danceShimmy();
  } else if (command == "dance-spin") {
    danceSpin();
  } else if (command == "head") {
    int offset = value.toInt();
    if (offset < -45) {
      offset = -45;
    }
    if (offset > 45) {
      offset = 45;
    }
    headOffset = offset;
    headServo.write(90 + headOffset);
  }

  server.send(200, "application/json", "{\"ok\":true}");
}

void handleStatus() {
  server.send(200, "application/json", "{\"status\":\"locked\"}");
}

void handleJPGStream() {
  WiFiClient client = server.client();
  String response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      continue;
    }
    server.sendContent("--frame\r\n");
    server.sendContent("Content-Type: image/jpeg\r\n");
    server.sendContent("Content-Length: " + String(fb->len) + "\r\n\r\n");
    client.write(fb->buf, fb->len);
    server.sendContent("\r\n");
    esp_camera_fb_return(fb);
    if (!client.connected()) {
      break;
    }
  }
}

void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sccb_sda = 26;
  config.pin_sccb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return;
  }
}

void setupServos() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  leftLeg.attach(SERVO_LEFT_LEG, SERVO_MIN_US, SERVO_MAX_US);
  rightLeg.attach(SERVO_RIGHT_LEG, SERVO_MIN_US, SERVO_MAX_US);
  leftFoot.attach(SERVO_LEFT_FOOT, SERVO_MIN_US, SERVO_MAX_US);
  rightFoot.attach(SERVO_RIGHT_FOOT, SERVO_MIN_US, SERVO_MAX_US);
  headServo.attach(SERVO_HEAD, SERVO_MIN_US, SERVO_MAX_US);

  neutralPose();
  headServo.write(90);
}

void setup() {
  setupCamera();
  setupServos();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/command", HTTP_POST, handleCommand);
  server.on("/stream", HTTP_GET, handleJPGStream);
  server.begin();
}

void loop() {
  server.handleClient();
}
