#if ARDUINO_USB_CDC_ON_BOOT
  #define S3USBSerial Serial
#else
  #if ARDUINO_USB_MODE
    #define S3USBSerial USBSerial
  #else
    #error "Please, board settings -> USB CDC On Boot=Enabled"
  #endif
#endif

#include <stdlib.h>
#include <HPDL1414.h>
#include "Wire.h"

//I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

//HPDL1414
#define PIN_D0   7
#define PIN_D1   5
#define PIN_D2   3
#define PIN_D3   1
#define PIN_D4  43
#define PIN_D5  46
#define PIN_D6   9
#define PIN_A0  40
#define PIN_A1  41
#define PIN_WR0 44      //1台目のHPDL1414のWRピンに接続するピン
#define PIN_WR1 42      //2台目のHPDL1414のWRピンに接続するピン
//const byte dataPins[7] = {7, 5, 3, 1, 43, 46, 9}; // Segment data pins: D0 - D6
//const byte addrPins[2] = {40, 41};                // Segment address pins: A0, A1
const byte wrenPins[]  = {44, 42};                // Write Enable pins (left to right)
//HPDL-1414制御用
//const byte dataPins[7] = {PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6};	  // ASCIIコードデータ送信用ピン指定 : D0~D6
//const byte addrPins[2] = {  PIN_A0, PIN_A0 };				                                  // 表示するセグメントのアドレス送信用ピン指定　: A0, A1
//const byte wrenPins[]  = {PIN_WR0, PIN_WR1};					                                // 書き込み開始信号送信用ピン指定 :WREN　※HPDL1414を2個使用（8桁表示）するためピンを2本指定。使用するHPDL1414毎にPIN一本追加。

HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));                        //HPDLのインスタンス開始
int delayTime = 500;

uint32_t i = 0;

void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  S3USBSerial.println("onRequest");
}


void onReceive(int len) {
  S3USBSerial.println("");  // 改行
  S3USBSerial.println("onReceive called");  // デバッグメッセージ
  S3USBSerial.printf("onReceive[%d]: ", len);

  char com[3] = {0};  // 2文字を格納するバッファ（+1は終端用）
  char buffer[33] = {0};  // 残りのデータを格納するバッファ（+1は終端用）
  int comIndex = 0;
  int bufferIndex = 0;
  int totalBytesRead = 0;

  while (Wire.available()) {
    char c = Wire.read();
    totalBytesRead++;
    
    if (totalBytesRead == 1) {
      // 最初の1文字を捨てる
      continue;
    } else if (totalBytesRead >= 2 && totalBytesRead <= 3) {
      // 2文字目から3文字目をcomに格納
      if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
        com[comIndex++] = c;
      }
    } else if (bufferIndex < sizeof(buffer) - 1) {
      // 残りのデータをbufferに格納
      if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
        buffer[bufferIndex++] = c;
      }
    } else {
      // バッファサイズを超えたデータを無視（必要に応じてデバッグ出力可能）
      S3USBSerial.print("*");  // 超過データを視覚的に確認するためのデバッグ出力
    }
  }

  //com[comIndex] = '\0';  // comバッファの終端を設定
  //buffer[bufferIndex] = '\0';  // bufferバッファの終端を設定

  // 受信したデータを表示
  S3USBSerial.print("Command: ");
  S3USBSerial.println(com);
  S3USBSerial.print("Data: ");
  S3USBSerial.println(buffer);
  
  // comの内容に応じて関数を呼び出す
  if (strcmp(com, "S:") == 0) {
    scrollDisplay(buffer);
  } else if (strcmp(com, "F:") == 0) {
    fixedDisplay(buffer);
  } else if (strcmp(com, "D:") == 0) {
    setScrollDelay(buffer);
  }
}


  void scrollDisplay(char buffer[33]){
    S3USBSerial.print("scroll func Command: ");
    S3USBSerial.println(buffer);
    hpdl.print(buffer);
  }

  void fixedDisplay(char buffer[33]){
    S3USBSerial.print("fix func Command: ");
    S3USBSerial.println(buffer);
    hpdl.print(buffer);
  }

  void setScrollDelay(char buffer[33]){
    delayTime = atoi(buffer);
    S3USBSerial.print("set delay func Command: ");
    S3USBSerial.println(buffer);
    hpdl.print(buffer);
  }

void setup() {
  S3USBSerial.begin(115200);
  while (!S3USBSerial) {
    ; // シリアルポートが利用可能になるまで待機
  }

  //HDPL1414表示消去
  hpdl.begin();
  hpdl.printOverflow(true);
  hpdl.clear();
  delay(1000);
  hpdl.print("01234567");
  delay(1000);
  hpdl.clear();
  delay(1000);
  hpdl.print("01234567");
  delay(1000);
  hpdl.clear();
  delay(1000);
  hpdl.print("01234567");

  
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



