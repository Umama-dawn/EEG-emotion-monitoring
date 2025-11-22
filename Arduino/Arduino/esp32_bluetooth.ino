#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("EEG_Headset");
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    SerialBT.println(data);
  }
  // Optionally handle incoming BT commands
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n');
    // handle commands if needed
  }
}
