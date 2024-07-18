#include <HPDL1414.h>

/*

  Segment numeration example

*/

const byte dataPins[7] = {9, 7, 5, 3, 43, 46, 1}; // Segment data pins: D0 - D6
const byte addrPins[2] = {40, 41};                // Segment address pins: A0, A1
const byte wrenPins[]  = {44, 42};                // Write Enable pins (left to right)

HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));

void printScrollingText(HPDL1414& hpdl, const String& text, int displayLength, int delayMs) {
    // 表示をスムーズにするために、テキストの先頭と末尾にスペースを追加
    String paddedText = String("        ") + text + String("        "); // 前後に8文字分のスペースを追加

    for (int i = 0; i < paddedText.length() - displayLength + 1; ++i) {
        // 表示する部分文字列を抽出
        String subText = paddedText.substring(i, i + displayLength);
        
        // hpdl.printを使って表示
        hpdl.print(subText);
        
        // 指定された遅延時間だけ待機
        delay(delayMs);
    }
}

void setup() {
  hpdl.begin();
  hpdl.printOverflow(true);
  hpdl.clear();

  String text = "hello world";
  int displayLength = 8; // 一度に表示できる文字数
  int delayMs = 300; // スクロールのステップ間の遅延時間（ミリ秒）
  
  printScrollingText(hpdl, text, displayLength, delayMs);
}

void loop() {
  // 何もしない
}
