#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
/*#include <IRrecv.h>
  #include <IRutils.h>
  #define D5 14
  uint16_t RECV_PIN = D5; // ИК-детектор
  IRrecv irrecv(RECV_PIN);
  decode_results results;*/


#define WIFI_SSID "Home"
#define WIFI_PASS "dima1307"
#define BOT_TOKEN "5591834898:AAGhq1f9sPCLL78i-ySib_5XqicI8Kd8V1Y"
//#define CHAT_ID "5589891711"
//#define CHAT_ID ""
String chat_id = "5589891711";
String chat_id2 = "369618659";
String chat_id3 = "5589891711, 369618659";

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

bool Notification = false;
bool Allownotifications = true;
unsigned long bot_lasttime; // last time messages' scan has been done

const int buttonPin = D3;
int currentState;
int lastState = HIGH;
int acState = LOW;
const uint16_t kIrLed = D2;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRSamsungAc ac(kIrLed);

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setupWiFi() {
  delay(200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}


void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    //String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (text == "/on")
    {
      samsungON();
      bot.sendMessage(chat_id, "AC is ON", "");
    }

    if (text == "/off")
    {
      samsungOFF();
      bot.sendMessage(chat_id, "AC is OFF", "");
    }

    if (text == "/toggle") {
      if (acState == LOW) {
        samsungON();
        bot.sendMessage(chat_id, "AC is ON", "");
      } else if (acState == HIGH) {
        samsungOFF();
        bot.sendMessage(chat_id, "AC is OFF", "");
      }
    }

    if (text == "/help")
    {
      String welcome = "Welcome to ESP8266 WiFi Telegram Test!\n";
      welcome += "Enter the following commands to configure:\n\n";
      welcome += "/on : to switch the Led 1 ON\n";
      welcome += "/off : to switch the Led 1 OFF\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
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
  ac.setMode(kSamsungAcCool);
  ac.send();
  blinkLed();
}

void samsungFanHigh() {
  Serial.println("Set the fan to high and the swing on ...");
  ac.setFan(kSamsungAcFanHigh);
  ac.setSwing(true);
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

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert);
  setupWiFi();

  //bot.sendMessage(chat_id, "ESP8266 WiFi Telegram Test is Started!", "");
  bot.sendMessage(chat_id3, "ESP8266 WiFi Telegram Test is Started!", "");

  ac.begin();
  Serial.println("Setting initial state for A/C.");
  ac.off();

  digitalWrite(LED_BUILTIN, HIGH);
}



void loop() {
  manual_control();
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}
