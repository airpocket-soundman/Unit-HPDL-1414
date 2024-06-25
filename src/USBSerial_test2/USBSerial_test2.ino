#if ARDUINO_USB_CDC_ON_BOOT
  #define S3USBSerial Serial
#else
  #if ARDUINO_USB_MODE
    #define S3USBSerial USBSerial
  #else
    #error "Please, board settings -> USB CDC On Boot=Enabled"
  #endif
#endif

#include "Wire.h"
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

uint32_t i = 0;

void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  S3USBSerial.println("onRequest");
}
/*
void onReceive(int len) {
  S3USBSerial.println("onReceive called");  // デバッグメッセージ
  S3USBSerial.printf("onReceive[%d]: ", len);
  while (Wire.available()) {
    char c = Wire.read();
    S3USBSerial.write(c);  // デバッグ用にシリアルに出力
    S3USBSerial.print(c);  // デバッグ用に文字を表示
  }
  S3USBSerial.println("catch");
}
*/

void onReceive(int len) {
  S3USBSerial.println("onReceive called");  // デバッグメッセージ
  S3USBSerial.printf("onReceive[%d]: ", len);

  char buffer[32];  // 受信データを格納するバッファ
  int index = 0;

  while (Wire.available() && index < sizeof(buffer) - 1) {
    char c = Wire.read();
    if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
      buffer[index++] = c;
    }
    //S3USBSerial.write(c);  // デバッグ用にシリアルに出力
  }
  buffer[index] = '\0';  // 文字列の終端を設定

  S3USBSerial.print("Received: ");
  S3USBSerial.println(buffer);  // 受信した文字列を表示
  S3USBSerial.println("catch");
}

void setup() {
  S3USBSerial.begin(115200);
  while (!S3USBSerial) {
    ; // シリアルポートが利用可能になるまで待機
  }
  S3USBSerial.println("Setup starting...");
  S3USBSerial.print("Start as I2C 0x");
  S3USBSerial.println(I2C_DEV_ADDR, HEX);

  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR);

  S3USBSerial.println("I2C initialized");
}

void loop() {
  S3USBSerial.println("loop");
  delay(1000);
}

