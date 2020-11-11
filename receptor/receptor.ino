#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SoftwareSerial.h>

#define IO_KEY "194ab85b6c4147d1a8e19b5633df5796"
#define WIFI_SSID "vodafoneC170"
#define WIFI_PASS "vinascorella1"

SoftwareSerial HC(13,12);

bool sendData(String feed, String data);

void setup() {
  
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  Serial.begin(115200);
  HC.begin(2400);

  pinMode(0, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID , WIFI_PASS);
  
  Serial.print("connecting to wifi  network");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  
}

void loop() {

  if(HC.available()){

    String message = HC.readString();
    Serial.print("data received: ");
    Serial.println(message);
    digitalWrite(0, 1);
    delay(100);
    digitalWrite(0, 0);

    String temperature = message.substring(0, message.indexOf(","));
    message.remove(0, temperature.length() + 1);
    String pressure = message.substring(0, message.indexOf(","));
    message.remove(0, pressure.length() + 1);
    String humidity = message.substring(0, message.indexOf(","));
    message.remove(0, humidity.length() + 1);
    String light = message.substring(0, message.indexOf(";"));
    
    sendData("tr.temperature", temperature);
    delay(100);
    sendData("tr.pressure", pressure);
    delay(100);
    sendData("tr.humidity", humidity);
    delay(100);
    sendData("tr.light", light);

  }
}

bool sendData(String feed, String data){
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient http;

  http.begin(*client, "https://io.adafruit.com/api/v2/janvinas/feeds/" + feed + "/data");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("X-AIO-Key", IO_KEY);

  int httpResponseCode = http.POST("value=" + data);
  Serial.println("responseCode: " + httpResponseCode);
  Serial.println("response: " + http.getString());
}
