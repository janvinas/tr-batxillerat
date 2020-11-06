#include "AdafruitIO_WiFi.h"
#include <SoftwareSerial.h>

#define IO_USERNAME   "janvinas"
#define IO_KEY        "194ab85b6c4147d1a8e19b5633df5796"
#define WIFI_SSID   "vodafoneC170"
#define WIFI_PASS   "vinascorella1"

SoftwareSerial HC(13,12);

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

AdafruitIO_Feed *temperatureIO = io.feed("tr.temperature");
AdafruitIO_Feed *humidityIO = io.feed("tr.humidity");
AdafruitIO_Feed *pressureIO = io.feed("tr.pressure");
AdafruitIO_Feed *lightIO = io.feed("tr.light");

void setup() {

  Serial.begin(9600);
  HC.begin(2400);

  pinMode(0, OUTPUT);

  Serial.print("Connecting to Adafruit IO");
  io.connect();

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  io.run();

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
    
    temperatureIO->save(temperature.toFloat());
    pressureIO->save(pressure.toFloat());
    lightIO->save(light.toFloat());
    humidityIO->save(humidity.toFloat());

  }
}
