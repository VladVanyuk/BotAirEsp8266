#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Arduino.h>
// IR library
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>

int pinState = 0;
int currentState;
int lastState = HIGH;

#define VPIN_BUTTON_1    V1 //51
#define VPIN_BUTTON_2    V2//52
#define VPIN_BUTTON_3    V3
#define VPIN_BUTTON_4    V4


#define pinStatePin D6
#define buttonPin D3

const uint16_t kIrLed = D2;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
//IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
IRSamsungAc irsend(kIrLed);     // Set the GPIO used for sending messages.

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Home";
char pass[] = "dima1307";
char server[] = "blynk-cloud.com";
//int port = 8080;
bool connectedFlag = false;
String pinStatus = "";

BLYNK_WRITE(VPIN_BUTTON_1) {  // Power button
  pinState = param.asInt();
  if (pinState == 0) {
    Serial.println("ON");
    irsend.send(0xE4780854);
  } else if (pinState == 1) {
    Serial.println("OFF");
    irsend.send(0xE4780854); //0xE4780854
  }
  digitalWrite(LED_BUILTIN, LOW); //on
  delay(150);
  digitalWrite(LED_BUILTIN, HIGH); //off
}

BLYNK_WRITE(VPIN_BUTTON_2) {  // Power button
  pinState = param.asInt();
  if (pinState == 0) {
    Serial.println("Set ON");
    irsend.on();
    irsend.send();
    printState();
  } else if (pinState == 1) {
    Serial.println("Set OFF");
    irsend.off();
    irsend.send();
    printState();
  }

  digitalWrite(LED_BUILTIN, LOW); //on
  delay(150);
  digitalWrite(LED_BUILTIN, HIGH); //off
}

void updateState(String state) {
  if (state.substring(0, 1) == "ON") {
    Blynk.virtualWrite(VPIN_BUTTON_1, HIGH);
  } else {
    Blynk.virtualWrite(VPIN_BUTTON_1, LOW);
  }
}

void printState() {
  // Display the settings.
  Serial.println("Samsung A/C remote is in the following state: ");
  Serial.print(pinState);
}

void manual_control() {
  //Manual Switch Control
  currentState = digitalRead(buttonPin);
  if (lastState == HIGH && currentState == LOW) {
    Serial.println("button was pressed");
    if (pinState == 1) {
      digitalWrite(pinStatePin, LOW);
      pinState = 0;
      Serial.println("Device1 OFF");
    } else {
      digitalWrite(pinStatePin, HIGH); // turn off relay 1
      pinState  = 1;
      Serial.println("Device1 ON");
    }

    digitalWrite(LED_BUILTIN, LOW); //on
    delay(150);
    digitalWrite(LED_BUILTIN, HIGH); //off
  }
  lastState = currentState;
  delay(100);
}

void setup() {
  /*#if defined(BLYNK_PRINT)
    // Debug console
    Serial.begin(115200);
    #endif  // BLYNK_PRINT
  */
  irsend.begin();
  Serial.begin(115200);
  delay(200);
  pinMode(buttonPin, INPUT);
  pinMode(pinStatePin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Default state of the remote.");
  printState();

  Blynk.begin(auth, ssid, pass);
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop() {
  while (Serial.available())
  {
    pinStatus = Serial.readString();
    updateState(pinStatus);
    connectedFlag = true;
  }

  Blynk.run();
}
