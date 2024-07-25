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

#include <FastLED.h>

#define PIN_LED    21   // 本体フルカラーLEDの使用端子（G21）
#define NUM_LEDS   1    // 本体フルカラーLEDの数
CRGB leds[NUM_LEDS];

//I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

//HPDL1414
#define PIN_D0   9
#define PIN_D1   7
#define PIN_D2   5
#define PIN_D3   3
#define PIN_D4  43
#define PIN_D5  46
#define PIN_D6   1
#define PIN_A0  40
#define PIN_A1  41
#define PIN_WR0 44      //1台目のHPDL1414のWRピンに接続するピン
#define PIN_WR1 42      //2台目のHPDL1414のWRピンに接続するピン
//const byte dataPins[7] = {9, 7, 5, 3, 43, 46, 1}; // Segment data pins: D0 - D6
//const byte addrPins[2] = {40, 41};                // Segment address pins: A0, A1
//const byte wrenPins[]  = {44, 42};                // Write Enable pins (left to right)
//HPDL-1414制御用
const byte dataPins[7] = {PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6};	  // ASCIIコードデータ送信用ピン指定 : D0~D6
const byte addrPins[2] = {PIN_A0, PIN_A1 };				                                  // 表示するセグメントのアドレス送信用ピン指定　: A0, A1
const byte wrenPins[]  = {PIN_WR0, PIN_WR1};					                                // 書き込み開始信号送信用ピン指定 :WREN　※HPDL1414を2個使用（8桁表示）するためピンを2本指定。使用するHPDL1414毎にPIN一本追加。

HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));                        //HPDLのインスタンス開始
int delayTime = 300;// スクロールのステップ間の遅延時間（ミリ秒）

String text = "HPDL1414 UNIT";
int displayLength = 8; // 一度に表示できる文字数


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

void scrollDisplay(char buffer[129]){
  S3USBSerial.print("scroll func Command: ");
  S3USBSerial.println(buffer);
  hpdl.clear();
  std::string strBuffer(buffer);
  printScrollingText(hpdl, strBuffer.c_str(), displayLength, delayTime);
}

void fixedDisplay(char buffer[33]){
  S3USBSerial.print("fix func Command: ");
  S3USBSerial.println(buffer);
  // バッファが9文字以上の場合、9文字目以降をカット
  if (strlen(buffer) > 8) {
      buffer[8] = '\0';  // 9文字目の位置にヌル文字を挿入して文字列を終了
  }

  hpdl.clear();
  hpdl.print(buffer);
}

void setScrollDelay(char buffer[33]){
  delayTime = atoi(buffer);
  S3USBSerial.print("set delay func Command: ");
  S3USBSerial.println(buffer);
}

void setup() {
  S3USBSerial.begin(115200);
  //while (!S3USBSerial) {
  //  ; // シリアルポートが利用可能になるまで待機
  //}

  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB(40, 40, 40);   // 白色（赤, 緑, 青）※3色それぞれの明るさを0〜255で指定


  //HDPL1414表示消去
  hpdl.begin();
  hpdl.printOverflow(true);
  hpdl.clear();

  /*
  delay(1000);
  hpdl.print("0123456789");
  delay(1000);
  hpdl.clear();
  delay(1000);
  hpdl.print("abcdefgh");
  delay(1000);
  hpdl.clear();
  delay(1000);
  hpdl.print("ABCDEFGH");
  delay(1000);
  hpdl.clear();
  delay(1000);
  hpdl.print("FFFFFFFF123");
  delay(1000);
  hpdl.clear();
  */
  
  
  printScrollingText(hpdl, text, displayLength, delayTime);
  setupI2CSlaveDevice();
  
}

void loop() {
  S3USBSerial.print(".");
  //delay(1000);

  /*
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  leds[0] = CRGB(40, 40, 40);
  FastLED.show();
  delay(500);
  */
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


void printScrollingText(HPDL1414& hpdl, const String& text, int displayLength, int delayMs) {
    // 表示をスムーズにするために、テキストの先頭と末尾にスペースを追加
    String paddedText = String("        ") + text + String("        "); // 前後に8文字分のスペースを追加

    for (int i = 0; i < paddedText.length() - displayLength + 1; ++i) {
        // 表示する部分文字列を抽出
        String subText = paddedText.substring(i, i + displayLength);
        
        // hpdl.printを使って表示
        hpdl.print(subText);
        
        // 指定された遅延時間だけ待機
        delay(delayTime);
    }
}
