#if ARDUINO_USB_CDC_ON_BOOT
  #define S3USBSerial Serial
#else
  #if ARDUINO_USB_MODE
    #define S3USBSerial USBSerial
  #else
    #error "Please, board settings -> USB CDC On Boot=Enabled"
  #endif
#endif

#include <HPDL1414.h>
#include "Wire.h"

//I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

//HPDL1414
#define PIN_D0   1
#define PIN_D1   3
#define PIN_D2   5
#define PIN_D3   7
#define PIN_D4  43
#define PIN_D5   9
#define PIN_D6  46
#define PIN_A0  40
#define PIN_A1  41
#define PIN_WR0 44      //1台目のHPDL1414のWRピンに接続するピン
#define PIN_WR1 42      //2台目のHPDL1414のWRピンに接続するピン

//HPDL-1414制御用
const byte dataPins[7] = {PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6};	  // ASCIIコードデータ送信用ピン指定 : D0~D6
const byte addrPins[2] = {  PIN_A0, PIN_A0 };				                                  // 表示するセグメントのアドレス送信用ピン指定　: A0, A1
const byte wrenPins[]  = {PIN_WR0, PIN_WR1};					                                // 書き込み開始信号送信用ピン指定 :WREN　※HPDL1414を2個使用（8桁表示）するためピンを2本指定。使用するHPDL1414毎にPIN一本追加。
HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));                        //HPDLのインスタンス開始


uint32_t i = 0;

void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  S3USBSerial.println("onRequest");
}


void onReceive(int len) {
  S3USBSerial.println("");
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


  //S3USBSerial.print("Received: ");
  S3USBSerial.println(buffer);  // 受信した文字列を表示
  //S3USBSerial.println("catch");
}

void setup() {
  S3USBSerial.begin(115200);
  while (!S3USBSerial) {
    ; // シリアルポートが利用可能になるまで待機
  }

  //HDPL1414表示消去
  hpdl.clear(); 
  
  setupI2CSlaveDevice();
}

void loop() {
  S3USBSerial.print(".");
  delay(1000);
}

void setupI2CSlaveDevice(){

  S3USBSerial.println("Setup starting...");
  S3USBSerial.print("Start as I2C 0x");
  S3USBSerial.println(I2C_DEV_ADDR, HEX);
  
  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR);

  S3USBSerial.println("I2C initialized");
}



