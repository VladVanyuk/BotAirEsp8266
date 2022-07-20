#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <IRrecv.h>
#include <IRutils.h>
//#include <FS.h>
//#include <SPIFFS.h>
//#include <HTTPUpdate.h>


#define D5 14
uint16_t RECV_PIN = D5; // ИК-детектор
IRrecv irrecv(RECV_PIN);
decode_results results;
bool receiverEnabled = false;

#define WIFI_SSID "Home"
#define WIFI_PASS "dima1307"
#define WIFI_SSID2 "AsusLyra"
#define WIFI_PASS2 "123456qwerty"
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

bool flag = false;
unsigned long timerDelay = 10000;
unsigned long timerButton = 0;
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

    if (text == "/status")
    {
      String statusMsg = "Current status: \n";
      statusMsg += "Enter the following commands to configure:\n\n";
      statusMsg += "power : ";
      statusMsg += String(ac.getPower());
      statusMsg += "Temperature : ";
      statusMsg += String(ac.getTemp());
      statusMsg += "Mode : ";
      statusMsg += String(ac.getMode());
      statusMsg += "Fan : ";
      statusMsg += String(ac.getFan());
      statusMsg += "Swing : ";
      statusMsg += String(ac.getSwing());

      bot.sendMessage(chat_id, statusMsg, "Markdown");
    }

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

    if (text == "/cool")
    {
      samsungCool();
      bot.sendMessage(chat_id, "AC mode Cool", "");
    }

    if (text == "/fanhigh")
    {
      samsungFanHigh();
      bot.sendMessage(chat_id, "AC mode FanHigh", "");
    }

    if (text == "/fanlow")
    {
      samsungFanLow();
      bot.sendMessage(chat_id, "AC mode FanLow", "");
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

    if (text == "/swing_on")
    {
      samsungSetSwingON();
      bot.sendMessage(chat_id, "AC swing ON", "");
    }

    if (text == "/swing_off")
    {
      samsungSetSwingOFF();
      bot.sendMessage(chat_id, "AC swing OFF", "");
    }

    if (text == "/help")
    {
      String welcome = "Welcome to ESP8266 WiFi Telegram Test!\n";
      welcome += "Enter the following commands to configure:\n\n";
      welcome += "/toggle : to toggle AC mode ON/OFF";
      welcome += "/on : to switch the AC ON\n";
      welcome += "/off : to switch the AC OFF\n";
      welcome += "/cool : to set AC mode COOL";
      welcome += "/fanlow : to set AC mode FANLOW";
      welcome += "/fanhigh : to set AC mode FANHIGH";
      welcome += "/swing_on : to set AC swing ON";
      welcome += "/swing_off : to set AC swing OFF";
      welcome += "/status : get current AC status";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }

  }
}

void blinkLed() {
  digitalWrite(LED_BUILTIN, LOW); //on
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);//off
}


void activateReceiverIR() {
  bool btnState = !digitalRead(buttonPin);
  timerButton = millis();
  if (btnState && !flag && millis() - timerButton > 100) {
    flag = true;
    timerButton = millis();
    Serial.println("press");
  }
  if (btnState && flag && millis() - timerButton > timerDelay) {
    timerButton = millis();
    Serial.println("press holdув 10 Seconds");
    irrecv.enableIRIn();
    receiverEnabled = true;
    blinkLed();
  }
  if (!btnState && flag && millis() - timerButton > 500) {
    flag = false;
    timerButton = millis();
    Serial.println("release");
  }

  if (receiverEnabled == true) {
    if ( irrecv.decode( &results )) { // если данные пришли
      Serial.println(results.value);
      irrecv.resume(); // принимаем следующую команду
    }
  }
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
  ac.setTemp(16);
  samsungFanHigh();
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
  ac.setSwing(false);
  ac.setTemp(16);
  digitalWrite(LED_BUILTIN, HIGH);
}



void loop() {
  manual_control();
  activateReceiverIR();
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
