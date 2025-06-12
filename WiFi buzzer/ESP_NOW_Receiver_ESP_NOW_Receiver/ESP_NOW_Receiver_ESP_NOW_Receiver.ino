#include <WiFi.h>
#include <esp_now.h>

const int buzzer = 25;
bool lastState = LOW;

// New callback signature for ESP32 Arduino core v3.x and later
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  bool incomingState;
  if (len == sizeof(incomingState)) {
    memcpy(&incomingState, data, sizeof(incomingState));
    Serial.print("Received state: ");
    Serial.println(incomingState ? "HIGH (pressed)" : "LOW (released)");
    if (incomingState) {
      digitalWrite(buzzer, HIGH); // Buzzer ON
    } else {
      digitalWrite(buzzer, LOW);  // Buzzer OFF
    }
  } else {
    Serial.print("Received unexpected data length: ");
    Serial.println(len);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    while (true) { delay(1); }
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing needed here; all work is done in the callback
}