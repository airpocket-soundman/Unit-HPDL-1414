#include "Wire.h"
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

uint32_t i = 0;

void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  Serial.println("onRequest");
  //USBSerial.println("onRequest");
}

void onReceive(int len) {
  Serial.printf("onReceive[%d]: ", len);
  //USBSerial.printf("onReceive[%d]: ", len);
  while (Wire.available()) {
    Serial.write(Wire.read());
    //USBSerial.write(Wire.read());
  }
  Serial.println();
  //USBSerial.println();
}

void setup() {

  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Serial.print("Start as I2C 0x");
  Serial.println(I2C_DEV_ADDR, HEX);
  //USBSerial.begin(115200);
  //USBSerial.setDebugOutput(true);
  //USBSerial.print("Start as I2C 0x");
  //USBSerial.println(I2C_DEV_ADDR, HEX);

  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
}

void loop() {

  Serial.println("main loop");
}