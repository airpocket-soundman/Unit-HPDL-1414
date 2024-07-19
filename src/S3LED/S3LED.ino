#include <FastLED.h>

#define PIN_LED    21   // 本体フルカラーLEDの使用端子（G21）
#define NUM_LEDS   1    // 本体フルカラーLEDの数

CRGB leds[NUM_LEDS];


void setup() {
  USBSerial.begin(9600);
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB(40, 40, 40);   // 白色（赤, 緑, 青）※3色それぞれの明るさを0〜255で指定
}

//赤 -> 消える -> 白を繰り返す
void loop() { 
  // Turn the LED on, then pause
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);

  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);

  // Turn the LED on, then pause
  leds[0] = CRGB(40, 40, 40);
  FastLED.show();
  delay(500);
}