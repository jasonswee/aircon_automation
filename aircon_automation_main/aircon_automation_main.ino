/*
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/

#include <esp_now.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>


// Replace with your network credentials
const char* ssid = "WifiName";
const char* password = "WifiPassword";

// Initialize Telegram BOT
#define BOTtoken "long-token-from-botfather-on-telegram"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "XXXXXXX"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

//////////////////espnow variables
// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0x7C, 0x9E, 0xBD, 0x60, 0xE3, 0x90};


///////////////////////////////////////////espnow variables end

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

long last_warning_soil_timestamp = 0;
long last_warning_humidity_timestamp = 0;
long time_between_warnings = 60000 * 60 * 47; //in millis




float closet_temperature;
float closet_humidity;


void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {
    //Serial.println("handle message");

    String chat_id = String(bot.messages[i].chat_id);
    // If the type is a "callback_query", a inline keyboard button was pressed
    if (bot.messages[i].type ==  F("callback_query")) {
      String text = bot.messages[i].text;
      Serial.print("Call back button pressed with text: ");
      Serial.println(text);

      if (text == F("humidity")) {
        closet_humidity = get_humidity();
        String state_str = "The Humidity in your room is: " + String(int(closet_humidity)) + "%.\n";
        bot.sendMessage(chat_id, state_str, "");
      }
      else if (text == F("temperature")) {
        closet_temperature = get_temperature();
        String state_str = "The Temperature in your room is: " + String(int(closet_temperature)) + "C°.\n";
        bot.sendMessage(chat_id, state_str, "");
      }
      else if (text == F("run_x_minutes")) {
        bot.sendMessage(chat_id, "In how many minutes?", "");
        bool confirmation_received = false;
        int new_message = 0;
        while (confirmation_received == false) {
          new_message = bot.getUpdates(bot.last_message_received + 1);
          if (new_message) {
            int request_minutes = bot.messages[i].text.toInt();
            Serial.println(request_minutes);
            if (request_minutes == 0) {
              bot.sendMessage(chat_id, "Learn how to type an int. Dumbass...", "");
            }
            else {
              confirmation_received = true;
              set_aircon_timer(request_minutes);
            }
          }
        }
      }
      else if (text == F("regulate_x_C")) {
        if (get_regulation_is_active()) {
          bot.sendMessage(chat_id, "Turning off AC regulation", "");
          stop_aircon_regulation();
        }
        else {
          bot.sendMessage(chat_id, "What Temperature, my Lord?", "");
          bool confirmation_received = false;
          int new_message = 0;
          while (confirmation_received == false) {
            new_message = bot.getUpdates(bot.last_message_received + 1);
            if (new_message) {
              float request_degrees = bot.messages[i].text.toFloat();
              Serial.println(request_degrees);
              if (request_degrees == 0.0) {
                bot.sendMessage(chat_id, "Learn how to type a float. *Eyes rolling*...", "");
              }
              else {
                confirmation_received = true;
                set_regulation_temperature(request_degrees);
                start_aircon_regulation();
              }
            }
          }
        }
      }
      else if (text == F("off")) {
        turn_off();
        bot.sendMessage(chat_id, "AC turned off. 環境がありがたい", "");
      }
      else if (text == F("on1")) {
        turn_on_1();
        bot.sendMessage(chat_id, "AC turned on level 1.", "");
      }
      else if (text == F("on2")) {
        turn_on_2();
        bot.sendMessage(chat_id, "AC turned on level 2.", "");
      }
      else if (text == F("on3")) {
        turn_on_3();
        bot.sendMessage(chat_id, "AC turned on level 3.", "");
      }


      // When a user first uses a bot they will send a "/start" command
      // So this is a good place to let the users know what commands are available
      else if (text == F("start")) {
        Serial.println("start was pushed");
        bot.sendMessage(chat_id, "drück hierdrauf -> /Atsui\n", "Markdown");
      }

    } else {

      String text = bot.messages[i].text;

      if (text == F("/Atsui")) {

        // Keyboard Json is an array of arrays.
        // The size of the main array is how many row options the keyboard has
        // The size of the sub arrays is how many coloums that row has

        // "The Text" property is what shows up in the keyboard
        // The "callback_data" property is the text that gets sent when pressed

        String keyboardJson = F("[[{ \"text\" : \"off\", \"callback_data\" : \"off\" },{ \"text\" : \"1\", \"callback_data\" : \"on1\" },{ \"text\" : \"2\", \"callback_data\" : \"on2\" }, { \"text\" : \"3\", \"callback_data\" : \"on3\" }],");
        keyboardJson += F("[{ \"text\" : \"turn off in x min\", \"callback_data\" : \"run_x_minutes\" }, { \"text\" : \"regulate x °C\", \"callback_data\" : \"regulate_x_C\" }],");
        keyboardJson += F("[{ \"text\" : \"Temperature\", \"callback_data\" : \"temperature\" }, { \"text\" : \"Humidity\", \"callback_data\" : \"humidity\"}]]");
        bot.sendMessageWithInlineKeyboard(chat_id, "Is it getting to hot again? *wink*", "", keyboardJson);
      }
    }
  }
}

void stop_everything() {
  stop_aircon_timer();
  stop_aircon_regulation();
}
  
void setup() {
  Serial.begin(115200);
  Serial.println("HELLO!!!");
  delay(100),

  init_servo();


  
#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());


  //pinMode(humidity_closet_pin, INPUT);

  init_temperature_sensor();


  //////////////////////espnow init
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet


  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  //////////////////////////////////////espnow init end
}

void loop() {
  // send periodic request to bot
  if (millis() > lastTimeBotRan + botRequestDelay)  {

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // regulate temperature
  check_and_execute_aircon_timer();

  check_and_execute_aircon_regulation();

}
