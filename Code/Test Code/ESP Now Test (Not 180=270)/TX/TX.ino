#include <WiFi.h>
#include <esp_now.h>
const char* RECEIVER_MAC = "48:e7:29:6d:71:8f";
#define POT1_PIN 32
#define POT2_PIN 33
#define POT3_PIN 34
#define POT4_PIN 35
#define BUTTON_PIN 25
int POT1_MIN_RAW = 0;
int POT1_MAX_RAW = 4095;
int POT2_MIN_RAW = 0;
int POT2_MAX_RAW = 4095;
int POT3_MIN_RAW = 0;
int POT3_MAX_RAW = 4095;
int POT4_MIN_RAW = 0;
int POT4_MAX_RAW = 4095;
#define ANGLE_DEADBAND 1
typedef struct {
  uint8_t servo1;
  uint8_t servo2;
  uint8_t servo3;
  uint8_t servo4;
  uint8_t gripper;
} DataPacket;
DataPacket currentData;
DataPacket lastSentData;
esp_now_peer_info_t peerInfo;
bool parseMAC(const char* macStr, uint8_t* mac) {
  int values[6];
  if (sscanf(macStr, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) {
    return false;
  }
  for (int i = 0; i < 6; i++) {
    mac[i] = (uint8_t)values[i];
  }
  return true;
}
uint8_t mapPotToAngle(int raw, int minRaw, int maxRaw) {
  raw = constrain(raw, minRaw, maxRaw);
  return map(raw, minRaw, maxRaw, 0, 180);
}
bool dataChanged() {
  return
    abs((int)currentData.servo1 - (int)lastSentData.servo1) >= ANGLE_DEADBAND ||
    abs((int)currentData.servo2 - (int)lastSentData.servo2) >= ANGLE_DEADBAND ||
    abs((int)currentData.servo3 - (int)lastSentData.servo3) >= ANGLE_DEADBAND ||
    abs((int)currentData.servo4 - (int)lastSentData.servo4) >= ANGLE_DEADBAND ||
    currentData.gripper != lastSentData.gripper;
}
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  uint8_t receiverMAC[6];
  parseMAC(RECEIVER_MAC, receiverMAC);
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  memset(&lastSentData, 255, sizeof(lastSentData));
}
void loop() {
  currentData.servo1 = mapPotToAngle(analogRead(POT1_PIN), POT1_MIN_RAW, POT1_MAX_RAW);
  currentData.servo2 = mapPotToAngle(analogRead(POT2_PIN), POT2_MIN_RAW, POT2_MAX_RAW);
  currentData.servo3 = mapPotToAngle(analogRead(POT3_PIN), POT3_MIN_RAW, POT3_MAX_RAW);
  currentData.servo4 = mapPotToAngle(analogRead(POT4_PIN), POT4_MIN_RAW, POT4_MAX_RAW);
  currentData.gripper = digitalRead(BUTTON_PIN) == LOW ? 180 : 0;
  if (dataChanged()) {
    esp_now_send(peerInfo.peer_addr, (uint8_t*)&currentData, sizeof(currentData));
    lastSentData = currentData;
  }
}