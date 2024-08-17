//#include <Mouse.h>
#include "Adafruit_TinyUSB.h"
// Tools -> Manage Libraries, install Adafruit TinyUSB
// Tools -> USB Stack -> Adafruit TinyUSB

struct Pin {
  bool usePullup;
  int number;
  int mask;

  Pin(bool usePullup, int number, int mask) : usePullup(usePullup), number(number), mask(mask) {}

  void setMode() const {
    pinMode(number, usePullup ? INPUT_PULLUP : INPUT);
  }

  operator bool() const {
    return not digitalRead(number);
  }
};

#define PIN_COUNT 5

uint8_t const l = 1, r = 2, m = 4, b = 8, f = 16;
//uint8_t const btns[PIN_COUNT] = {b, l, m, r, f};
Pin const pins[PIN_COUNT] = {{true, D0, b}, {true, D5, l}, {true, D6, m}, {true, D1, r}, {true, D2, f}};

uint8_t loop_counter = 0;

//int const modes = { INPUT_PULLUP, INPUT_PULLUP, INPUT, INPUT, INPUT }

//uint8_t const pins[PIN_COUNT] = {3, 2, 4, 11, 12};
uint8_t const ledPin =  LED_BUILTIN;      // the number of the LED pin

uint8_t const report_id = 0; // no ID

uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_MOUSE(),
  TUD_HID_REPORT_DESC_KEYBOARD()
};

Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_MOUSE | HID_ITF_PROTOCOL_KEYBOARD, 2, false);

void setup() {
  TinyUSB_Device_Init(0);
  usb_hid.begin();
  pinMode(ledPin, OUTPUT);
  for (Pin const pin : pins) {
    pin.setMode();
  }
  Serial.begin(115200);
  //Mouse.begin();
  //while(!Serial);
  Serial.println("Started");
  while (!TinyUSBDevice.mounted());

  Serial.println("Mounted");
}

void loop() {
  loop_counter = (loop_counter + 1) % 100;
  /*
    if (loop_counter > 50)
    digitalWrite(ledPin, HIGH);
    else
    digitalWrite(ledPin, LOW);
  */
  delay(10);
  // Remote wakeup

  //if (usb_hid.ready()) {
  uint8_t fields = 0b00000;
  for (Pin const pin : pins) {
    if (pin) {
      fields |= pin.mask;
    }
    /*
      if (digitalRead(pins[i])){
      //Serial.print("0");
      //Mouse.release(buttons[i]);
      //usb_hid.mouseButtonRelease(report_id, Btn::btns[i]);
      } else {
      //Serial.print("1");
      fields |= Btn::btns[i];
      //usb_hid.mouseButtonPress(report_id, Btn::btns[i]);
      //Mouse.press(buttons[i]);
      }
    */
  }
  //if (fields == 0)
  //return;
  if (TinyUSBDevice.suspended()) {
    //Serial.println("wake up");
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }
  if (fields == 0)
    digitalWrite(ledPin, HIGH);
  else
    digitalWrite(ledPin, LOW);
  Serial.print(fields, BIN);
  Serial.print(" ");
  if (usb_hid.ready())
    usb_hid.mouseButtonPress(report_id, fields);
  //usb_hid.mouseMove(report_id, 0, 1);
  //else
  //Serial.print("Not ready");
  //}
  //Serial.print(" ");
  Serial.print(loop_counter);
  Serial.println();
}
