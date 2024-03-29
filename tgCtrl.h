#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
//#include "samsungCtrl.h"

#define WIFI_SSID "Home"
#define WIFI_PASS "dima1307"
#define BOT_TOKEN "5591834898:AAFjzip9BD6598vzsgtiM3Ia1TsoLPpf7g0"
String chat_id = "369618659";

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

bool Notification = true;
bool Allownotifications = true;
unsigned long bot_lasttime; // last time messages' scan has been done


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {


    String text = bot.messages[i].text;
    if (text == "/status")
    {
      //ac.toString();
      String statusMsg = "Current status: \n";
      statusMsg += "Enter the following commands to configure:\n\n";
      statusMsg += "\nPower : ";
      statusMsg += String(ac.getPower());
      statusMsg += "\nTemperature : ";
      statusMsg += String(ac.getTemp());
      statusMsg += "\nMode : ";
      statusMsg += String(ac.getMode());
      statusMsg += "\nFan : ";
      statusMsg += String(ac.getFan());
      statusMsg += "\nSwing : ";
      statusMsg += String(ac.getSwing());

      bot.sendMessage(chat_id, statusMsg, "Markdown");
      bot.sendMessage(chat_id, ac.toString(), "Markdown");

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

    if (text == "/set_temperature") {
      int tgTemp = text.toInt();
      ac.setTemp(tgTemp);
    }
    if (text == "/help")
    {
      String welcome = "Welcome to ESP8266 WiFi Telegram Test!\n";
      welcome += "Enter the following commands to configure:\n\n";
      welcome += "/toggle : to toggle AC mode ON/OFF";
      welcome += "/on : to switch the AC ON\n";
      welcome += "/off : to switch the AC OFF\n";
      welcome += "/cool : to set AC mode COOL\n";
      welcome += "/fanlow : to set AC mode FANLOW\n";
      welcome += "/fanhigh : to set AC mode FANHIGH\n";
      welcome += "/swing_on : to set AC swing ON\n";
      welcome += "/swing_off : to set AC swing OFF\n";
      welcome += "/status : get current AC status\n";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}
