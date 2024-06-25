#if ARDUINO_USB_CDC_ON_BOOT
  #define S3USBSerial Serial
#else
  #if ARDUINO_USB_MODE
    #define S3USBSerial USBSerial
  #else
    #error "Please, board settings -> USB CDC On Boot=Enabled"
  #endif
#endif

void setup() {
  S3USBSerial.begin(115200);
}

void loop() {
  static int cnt = 0;
  cnt++;
  S3USBSerial.printf("cnt = %d\n", cnt);
  delay(100);
}
