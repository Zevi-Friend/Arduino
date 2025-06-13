#include <WiFi.h>
#include <esp_now.h>

const int buzzer = 25;

const int freq = 2000;     // 2 kHz tone (adjust for your buzzer)
const int pwmChannel = 0;  // ESP32 has 16 channels: 0-15
const int resolution = 8;  // 8-bit (0-255)

typedef struct struct_message {
  bool myState;
  int myValue;
} struct_message;

// New callback signature for ESP32 Arduino core v3.x and later
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  struct_message incomingData;
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, data, sizeof(incomingData));
    Serial.print("Received state: ");
    Serial.println(incomingData.myState ? "HIGH (pressed)" : "LOW (released)");

    int mapped = map(incomingData.myValue, 0, 4095, 0, 255);

    if (incomingData.myState) {
      ledcWrite(pwmChannel, mapped);
    } else {
      ledcWrite(pwmChannel, 0);  // Buzzer OFF
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

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(buzzer, pwmChannel);

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing needed here; all work is done in the callback
}
