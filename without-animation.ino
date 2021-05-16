#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

MD_Parola myDisplay = MD_Parola(MD_MAX72XX::FC16_HW, D7, D5, D8, 4);
//MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DIN, CLK, CS, MAX_DEVICES);

void setup() {
  myDisplay.begin();
  myDisplay.setIntensity(0); // 0-15
  myDisplay.setTextAlignment(PA_CENTER);

  WiFi.begin("NAME","PASSWORD");
  WiFi.mode(WIFI_STA);
}

double subs=0.0, views=0.0;
long lastTime = 0, lastTime2 = 0;
bool isSub = true;

void getData(){
	HTTPClient http; 
	WiFiClientSecure client;
	client.setInsecure();
	client.connect("www.googleapis.com", 443);
	http.begin(client, "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=CHANNLEID&key=APIKEY");

	if (http.GET() == 200){
		DynamicJsonBuffer jsonBuffer(500);
		JsonObject& root = jsonBuffer.parseObject(http.getString());
		subs = ((int) root["items"][0]["statistics"]["subscriberCount"])/1000.0;
		views = ((int) root["items"][0]["statistics"]["viewCount"])/1000.0;
		lastTime = millis();

		myDisplay.print(String(subs)+"K");
	}
	http.end();
}

void loop() {
  if (millis()-lastTime > 3600000 || subs == 0){
    if (WiFi.status() == WL_CONNECTED) {
      getData(); 
    }
  } else {
    if (millis()-lastTime2 > 25000){
      if (isSub){
        myDisplay.print(String(views)+"K");
      } else {
        myDisplay.print(String(subs)+"K");
      }
      isSub = !isSub;
      lastTime2 = millis();
    }
  }
}
