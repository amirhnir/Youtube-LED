#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

int texteffect_index = 0;
textEffect_t texteffect[] =
{
  PA_PRINT,
  PA_SCAN_HORIZ,
  PA_SCROLL_LEFT,
  PA_WIPE,
  PA_SCROLL_UP_LEFT,
  PA_SCROLL_UP,
  PA_OPENING_CURSOR,
  PA_GROW_UP,
  PA_MESH,
  PA_SCROLL_UP_RIGHT,
  PA_BLINDS,
  PA_CLOSING,
  PA_RANDOM,
  PA_GROW_DOWN,
  PA_SCAN_VERT,
  PA_SCROLL_DOWN_LEFT,
  PA_WIPE_CURSOR,
  PA_DISSOLVE,
  PA_OPENING,
  PA_CLOSING_CURSOR,
  PA_SCROLL_DOWN_RIGHT,
  PA_SCROLL_RIGHT,
  PA_SLICE,
  PA_SCROLL_DOWN
};
MD_Parola myDisplay = MD_Parola(MD_MAX72XX::FC16_HW, D7, D5, D8, 4);
//MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DIN, CLK, CS, MAX_DEVICES);

char* StringToCharArray(String str){
  int str_len = str.length() + 1; 
  char char_array[str_len];
  str.toCharArray(char_array, str_len);
  return char_array;
}

void setup() {
  myDisplay.begin();
  myDisplay.setIntensity(0); // 0-15
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.setSpeed(50);

  WiFi.begin("NAME","PASSWORD");
  WiFi.mode(WIFI_STA);
}

double subs=0.0, views=0.0;
long lastTime = 0;
bool isSub = true;

void getData(){
  HTTPClient http; 
  WiFiClientSecure client;
  client.setInsecure();
  client.connect("www.googleapis.com", 443);
  http.begin(client, "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=CHANNLEID&key=APIKEY");

  int httpcode = http.GET();
  if (httpcode == 200){
    DynamicJsonBuffer jsonBuffer(500);
    JsonObject& root = jsonBuffer.parseObject(http.getString());
    subs = ((int) root["items"][0]["statistics"]["subscriberCount"])/1000.0;
    views = ((int) root["items"][0]["statistics"]["viewCount"])/1000.0;
    lastTime = millis();
    
    myDisplay.displayText(StringToCharArray(String(subs)+"K"), myDisplay.getTextAlignment(), myDisplay.getSpeed(), 25000, texteffect[texteffect_index], texteffect[texteffect_index]);
    if (texteffect_index < sizeof(texteffect)) 
      texteffect_index++;
    else 
      texteffect_index = 0;
  }
  http.end();
}

void loop() {
  if (myDisplay.displayAnimate()) {
    if (millis()-lastTime > 3600000 || subs == 0){
     if (WiFi.status() == WL_CONNECTED) {
       getData(); 
     }
    }else {
      if (isSub){
        myDisplay.displayText(StringToCharArray(String(views)+"K"), myDisplay.getTextAlignment(), myDisplay.getSpeed(), 25000, texteffect[texteffect_index], texteffect[texteffect_index]);
      } else {
        myDisplay.displayText(StringToCharArray(String(subs)+"K"), myDisplay.getTextAlignment(), myDisplay.getSpeed(), 25000, texteffect[texteffect_index], texteffect[texteffect_index]);
      }
      if (texteffect_index < sizeof(texteffect)) 
        texteffect_index++;
      else 
        texteffect_index = 0;
      isSub = !isSub;
      }
    myDisplay.displayReset();
  }
}
