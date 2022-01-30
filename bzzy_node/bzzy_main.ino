#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "./functions.h"

#define disable 0
#define enable  1
unsigned int channel = 1;
unsigned long lastmillis;
unsigned long PromTimeout = 60000;//900000; // milliseconds 900,000=15 mins

void blink(int amount, int speed){
  for(int x=0;x<amount;x++){
    digitalWrite(BUILTIN_LED, LOW);
    delay(speed);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(speed);
  }
}

void runPromiscuous(){
  lastmillis = millis() + PromTimeout;
  client_count = 0;
  adx = 0;
  wifi_promiscuous_enable(enable);
  channel = 1;
  wifi_set_channel(channel);
  while (millis() < lastmillis) // 10 minutes *actually microseconds lol
  {
    Serial.print(millis());
    Serial.print(" < ");
    Serial.println(lastmillis);
    channel++;
    if (channel == 17){channel=1;}             // Only scan channels 1 to 16
    wifi_set_channel(channel);
    printTop15(client_list);
    Serial.printf("Clients: %d\n",client_count);
    Serial.printf("Average: %d\n",getAverage());
    delay(500);  // critical processing timeslice & time to probe each channel
  }
  wifi_promiscuous_enable(disable);
}

void postData(const char* ssid, const char* password){
  WiFi.begin(ssid, password);
  digitalWrite(BUILTIN_LED, LOW);
  while (WiFi.status() != WL_CONNECTED){delay(500);}
  digitalWrite(BUILTIN_LED, HIGH);

  WiFiClient client;
  HTTPClient http;
  if (!http.begin(client, "http://192.168.5.15:8080/data")){
    blink(3,500);
    delay(5000);
    return;
  }
  //POST code
  blink(1,250);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  char buff[18];
  sprintf(buff,"clients=%3d&id=000",getAverage());
  http.POST(buff);
  http.writeToStream(&Serial);

  http.end();
  WiFi.disconnect();//FIXME:? ESP.restart()
  blink(2,500);
}


void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nSDK version:%s\n\r", system_get_sdk_version());
  pinMode(BUILTIN_LED, OUTPUT);
  blink(1,2000);

  wifi_set_opmode(STATION_MODE);            // Promiscuous works only with station mode
  wifi_set_channel(channel);
  wifi_promiscuous_enable(disable);
  wifi_set_promiscuous_rx_cb(promisc_cb);   // Set up promiscuous callback
}

void loop() {
  runPromiscuous();
  postData("BELL256-V","rapswin2022");
  //  delay(5000);
}
