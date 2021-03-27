#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define IO_KEY "***" //clau per l'API d'Adafruit IO
#define WIFI_SSID "***"    //SSID Wi-Fi
#define WIFI_PASS "***"   //contrasenya Wi-Fi

#define RX_PIN 14
#define TX_PIN 12
#define LED_PIN 2
#define IR_PIN 0

SoftwareSerial HC(RX_PIN, TX_PIN);   //crea un port sèrie virtual

SSD1306Wire display(0x3c, 5, 4);

IRrecv receiver(IR_PIN);
decode_results results;

bool sendData(String feed, String data);    //prototip de la funció que envia una dada per http
void updateDisplay(String val1, String val2, String val3, String message, byte progessBar);
void updateTime(int t);
void handleIR(long long code);

long lastReading = 0;
String temperature = "--";
String pressure = "--";
String humidity = "--";
String light = "--";
float temperatureHistory[64] = {};
int historyLocation = 0;

void setup() {

  Serial.begin(115200);   //inicialitza port sèrie de hardware
  HC.begin(2400);         //inicialitza port sèrie virtual
  receiver.enableIRIn();

  pinMode(LED_PIN, OUTPUT);     //Led d'informació connectat al pin GPIO0

  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.clear();
  updateDisplay("--", "--", "--", "esperant informació", 0);
  
}

void loop() {

  //la següent condició s'activara quan s'hagi rebut dades del receptor de ràdio
  if(HC.available()){

    String message = HC.readString();   //emmagatzema la informació rebuda en una variable
    Serial.print("data received: ");
    Serial.println(message);            //imprimeix la informació
    digitalWrite(LED_PIN, 1);    //curt avís lluminós sobre la rebuda de dades.
    delay(100);
    digitalWrite(LED_PIN, 0);

    temperature = message.substring(0, message.indexOf(","));    //separa la informació
    message.remove(0, temperature.length() + 1);
    pressure = message.substring(0, message.indexOf(","));
    message.remove(0, pressure.length() + 1);
    humidity = message.substring(0, message.indexOf(","));
    message.remove(0, humidity.length() + 1);
    light = message.substring(0, message.indexOf(";"));

    updateDisplay(temperature, pressure, humidity, "s'ha rebut informació", 0);
    delay(1000);

    WiFi.mode(WIFI_STA);    //defineix el mode de funcionament del Wi-Fi
    WiFi.begin(WIFI_SSID , WIFI_PASS);  //connecta a la xarxa Wi-Fi amb la informació proporcionada anteriorment.
    Serial.print("connecting to wifi  network");  //espera mentre s'estableix la connexió
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("connected");    //connexió establerta, podem continuar
    
    updateDisplay(temperature, pressure, humidity, "enviant informació", 0);
    sendData("tr.temperature", temperature);    //envia cada dada amb una petició HTTP independent
    updateDisplay(temperature, pressure, humidity, "enviant informació", 1);
    delay(100);
    sendData("tr.pressure", pressure);
    updateDisplay(temperature, pressure, humidity, "enviant informació", 2);
    delay(100);
    sendData("tr.humidity", humidity);
    updateDisplay(temperature, pressure, humidity, "enviant informació", 3);
    delay(100);
    sendData("tr.light", light);
    updateDisplay(temperature, pressure, humidity, "enviant informació", 4);
    delay(500);
    updateDisplay(temperature, pressure, humidity, "esperant informació", 0);

    temperatureHistory[historyLocation] = temperature.toFloat();
    historyLocation++;
    if(historyLocation > 64) historyLocation = 0;

    lastReading = millis();
    WiFi.disconnect(true);

  }

  if (receiver.decode(&results)) {
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    handleIR(results.value);
    receiver.resume();  // Receive the next value
  }

  updateTime( (millis() - lastReading) / 1000 );
}

bool sendData(String feed, String data){
  
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);    //necessari per fer una petició HTTPS
  client->setInsecure();    //la seguretat no és important aquí, així que no xifrarem la connexió
  HTTPClient http;

  http.begin(*client, "https://io.adafruit.com/api/v2/janvinas/feeds/" + feed + "/data");  //URL on s'haurà de fer la petició. Depèn del "feed" que volguem utilitzar
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //informació sobre el contingut del cos de la transmissió.
  http.addHeader("X-AIO-Key", IO_KEY);    //autenticació, necessària per publicar les dades

  int httpResponseCode = http.POST("value=" + data);    //realitza la petició i desa el codi de resposta
  Serial.println("responseCode: " + httpResponseCode);  //imprimeix el codi de resposta
  Serial.println("response: " + http.getString());      //i la resposta
  
  if(httpResponseCode > 0){
    return true;
  }else{
    return false;
  }
}

void handleIR(long long code){
  if(code == 0xFF30CF){
    updateDisplay(temperature, pressure, humidity, "esperant informació", 0);
  }else if(code == 0xFF18E7){
    updateDisplay2();
  }
}
