#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca(0x40);

#define SERVOMIN 102
#define SERVOMAX 512            

typedef struct {
  int angle;
} DataPacket;

DataPacket data;

int currentPulse = 0;

void onReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&data, incomingData, sizeof(data));

  int targetPulse = map(data.angle, 0, 160, SERVOMIN, SERVOMAX);

  // FAST SMOOTHING (no delay)
  currentPulse = targetPulse;

  for (int i = 0; i < 5; i++) {
    pca.setPWM(i, 0, currentPulse);
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin(D2, D1);
  pca.begin();
  pca.setPWMFreq(50);

  WiFi.mode(WIFI_STA);

  esp_now_init();

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // nothing here → ultra fast interrupt-based update
}