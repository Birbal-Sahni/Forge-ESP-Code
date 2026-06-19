#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SDA_PIN 21
#define SCL_PIN 22
#define SERVOMIN 102
#define SERVOMAX 512
#define SERVO1_REVERSE 0
#define SERVO2_REVERSE 0
#define SERVO3_REVERSE 0
#define SERVO4_REVERSE 0
#define SERVO5_REVERSE 0
#define FAILSAFE_TIMEOUT 500
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);
typedef struct {
  uint8_t servo1;
  uint8_t servo2;
  uint8_t servo3;
  uint8_t servo4;
  uint8_t gripper;
} DataPacket;
DataPacket receivedData;
uint8_t currentAngles[5] = {90,90,90,90,0};
unsigned long lastPacketTime = 0;
uint16_t angleToPulse(uint8_t angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}
uint8_t applyReverse(uint8_t angle, uint8_t reverseFlag) {
  return reverseFlag ? (180 - angle) : angle;
}
void writeServo(uint8_t channel, uint8_t angle) {
  pca9685.setPWM(channel, 0, angleToPulse(angle));
}
void updateServos() {
  currentAngles[0] = applyReverse(receivedData.servo1, SERVO1_REVERSE);
  currentAngles[1] = applyReverse(receivedData.servo2, SERVO2_REVERSE);
  currentAngles[2] = applyReverse(receivedData.servo3, SERVO3_REVERSE);
  currentAngles[3] = applyReverse(receivedData.servo4, SERVO4_REVERSE);
  currentAngles[4] = applyReverse(receivedData.gripper, SERVO5_REVERSE);
  writeServo(0, currentAngles[0]);
  writeServo(1, currentAngles[1]);
  writeServo(2, currentAngles[2]);
  writeServo(3, currentAngles[3]);
  writeServo(4, currentAngles[4]);
}
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len != sizeof(DataPacket)) return;
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  lastPacketTime = millis();
  updateServos();
}
void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  pca9685.begin();
  pca9685.setPWMFreq(50);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
  for (int i = 0; i < 5; i++) {
    writeServo(i, currentAngles[i]);
  }
  lastPacketTime = millis();
}
void loop() {
  if (millis() - lastPacketTime > FAILSAFE_TIMEOUT) {
    return;
  }
}