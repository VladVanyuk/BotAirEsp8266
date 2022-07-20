#include "samsungCtrl.h"
#include "tgCtrl.h"
//#include "tgUpdate.h"

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

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert);
  setupWiFi();
  bot.sendMessage(chat_id, "ESP8266 WiFi Telegram Test is Started!", "");
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
