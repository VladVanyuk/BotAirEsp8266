#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
#include <FastBot.h>


#define WIFI_SSID "AsusLyra"
#define WIFI_PASS "123456qwerty"
#define BOT_TOKEN "5591834898:AAGhq1f9sPCLL78i-ySib_5XqicI8Kd8V1Y"
#define CHAT_ID "5589891711"
//#define CHAT_ID ""

const int buttonPin = D3;
int currentState;
int lastState = HIGH;
int acState = LOW;

const uint16_t kIrLed = D2;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRSamsungAc ac(kIrLed);
FastBot bot(BOT_TOKEN);

void setupWiFi() {
  delay(2000);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected");
}

// обработчик сообщений
void newMsg(FB_msg& msg) {
  Serial.println(msg.toString());
  //bot.sendMessage(msg.text, msg.chatID);
  if (msg.toString() == "/toggle_ac") {
    if (acState == LOW) {
      samsungON();
      bot.sendMessage("Now ON", msg.chatID);
    } else if (acState == HIGH) {
      samsungOFF();
      bot.sendMessage("Now OFF", msg.chatID);
    }
  } else if (msg.toString() == "/on") {
    samsungON();
    bot.sendMessage("Now ON", msg.chatID);
  } else if (msg.toString() == "/off") {
    samsungOFF();
    bot.sendMessage("Now OFF", msg.chatID);
  }
}

void blinkLed() {
  digitalWrite(LED_BUILTIN, LOW); //on
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);//off
}

void manual_control() {
  //Manual Switch Control
  currentState = digitalRead(buttonPin);
  if (lastState == HIGH && currentState == LOW) {
    Serial.println("button was pressed");
    blinkLed();
    if (acState == LOW) {
      samsungON();
    } else if (acState == HIGH) {
      samsungOFF();
    }
  }
  lastState = currentState;
  delay(100);
}

void samsungON() {
  Serial.println("Turn on the A/C ");
  ac.on();
  ac.send();
  acState != acState;
  blinkLed();
}

void samsungOFF() {
  Serial.println("Turn off the A/C ");
  ac.off();
  ac.send();
  acState != acState;
  blinkLed();
}

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  setupWiFi();

  // можно сменить размер буфера на (приём, отправку), по умолч. 512, 512
  //bot.setBufferSizes(1024, 512);

  bot.setChatID(CHAT_ID); 
  //bot.setChatID("123456,7891011,12131415");
  bot.attach(newMsg);
  bot.sendMessage("Hello, World!");
  ac.begin();
  Serial.println("Setting initial state for A/C.");
  ac.off();

  digitalWrite(LED_BUILTIN, HIGH);
}



void loop() {
  manual_control();
  bot.tick();   // тикаем в луп
}
