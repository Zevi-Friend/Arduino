#include <WiFi.h>
#include <esp_now.h>

const int button = 0;
const int vol = 32;

//Since I am using the boot pin, pressed == HIGH and vice verca.
bool myState = LOW;
bool oldState = myState;

typedef struct struct_message {
  bool myState;
  int myValue;
} struct_message;

struct_message dataToSend = {myState, 0};

// Receiver's MAC address as a byte array
uint8_t receiverAddress[] = {0x8C, 0x4F, 0x00, 0x30, 0x82, 0xFC}; // your receiver MAC

void setup() {
  pinMode(button, INPUT_PULLUP);
  pinMode(vol, INPUT);

  Serial.begin(115200); // Start serial communication for debug output
  delay(1000);
  WiFi.mode(WIFI_STA);  // Set WiFi mode to Station, required for ESP-NOW
  delay(100);
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return; // Stop here if ESP-NOW failed to initialize
  }

  // Prepare and add the receiver as a peer
  esp_now_peer_info_t peerInfo = {}; // Structure to hold peer information
  memcpy(peerInfo.peer_addr, receiverAddress, 6); // Set peer MAC address
  peerInfo.channel = 0;    // Default channel (0 = current WiFi channel)
  peerInfo.encrypt = false;// Encryption off (can be enabled if needed)
  esp_now_add_peer(&peerInfo); // Register the peer with ESP-NOW

  
  esp_now_send(receiverAddress, (uint8_t*)&myState, sizeof(myState)); // Send the boolean state

}

void loop() {
  dataToSend.myState = !digitalRead(button);

  if(dataToSend.myState != oldState) {
    esp_now_send(receiverAddress, (uint8_t*)&dataToSend, sizeof(dataToSend));
    oldState = dataToSend.myState;
    Serial.println(dataToSend.myState);
  }

  delay(20);
}
