#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
#include <IRrecv.h>
#include <IRutils.h>

#define D5 14

uint16_t RECV_PIN = D5; // ИК-детектор
IRrecv irrecv(RECV_PIN);
decode_results results;

bool receiverEnabled = true;
int acState = LOW;

const uint16_t kIrLed = D2;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRSamsungAc ac(kIrLed);

//bool flag = false;
unsigned long timerDelay = 10000;
unsigned long timerButton = 0;
unsigned long timerReceived = 0;
const int buttonPin = D3;
int currentState;
int lastState = HIGH;

void blinkLed() {
  digitalWrite(LED_BUILTIN, LOW); //on
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);//off}
}


void receiverIR() {
  if (receiverEnabled == true) {
    if ( irrecv.decode( &results )) { // если данные пришли
      Serial.println(results.value);
      switch (results.value) {
        case 3064457680://off
          ac.off();
          break;
        case 295605724:
          ac.on();
          break;
      }
      irrecv.resume();
    }
  }
}

void samsungON() {
  Serial.println("Turn on the A/C ");
  ac.on();
  ac.setTemp(16);
  ac.setFan(kSamsungAcFanHigh);
  ac.send();
  acState = HIGH;
  blinkLed();
}

void samsungOFF() {
  Serial.println("Turn off the A/C ");
  ac.off();
  ac.send();
  acState = LOW;
  blinkLed();
}
void samsungCool() {
  Serial.println("Set the A/C mode to cooling ...");
  ac.setSwing(false);
  ac.setMode(kSamsungAcCool);
  ac.send();
  blinkLed();
}

void samsungFanHigh() {
  Serial.println("Set the fan to high ");
  ac.setSwing(false);
  ac.setFan(kSamsungAcFanHigh);
  ac.send();
  blinkLed();
}

void samsungFanLow() {
  // Change to Fan mode, lower the speed, and stop the swing.
  Serial.println("Set the A/C to fan only with a low speed, & no swing ...");
  ac.setSwing(false);
  ac.setMode(kSamsungAcFan);
  ac.setFan(kSamsungAcFanLow);
  ac.send();
  blinkLed();
}

void samsungSetSwingON() {
  Serial.println("Set the A/C swing ON");
  ac.setSwing(true);
  ac.send();
  blinkLed();
}

void samsungSetSwingOFF() {
  Serial.println("Set the A/C swing OFF");
  ac.setSwing(false);
  ac.send();
  blinkLed();
}

//void samsungSetTemp()
