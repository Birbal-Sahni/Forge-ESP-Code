          #include <WiFi.h>
#include <esp_now.h>

uint8_t receiverMAC[] = {0x48, 0xE7, 0x29, 0x6D, 0x71, 0x8F};

#define POT_PIN 32

typedef struct {
  int angle;
} DataPacket;

DataPacket data;

int lastAngle = -1;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_now_init();

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);
}

void loop() {
  int potValue = analogRead(POT_PIN);
  int angle = map(potValue, 0, 4095, 0, 160);

  // send ONLY if value changes (removes spam + lag)
  if (abs(angle - lastAngle) >= 1) {
    data.angle = angle;
    esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));
    lastAngle = angle;
  }
}