#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SoftwareSerial.h>

#define IO_KEY "194ab85b6c4147d1a8e19b5633df5796" //clau per l'API d'Adafruit IO
#define WIFI_SSID "vodafoneC170"    //SSID Wi-Fi
#define WIFI_PASS "vinascorella1"   //contrasenya Wi-Fi

SoftwareSerial HC(13,12);   //crea un port sèrie virtual als pins RX=GPIO13; TX=GPIO12

bool sendData(String feed, String data);    //prototip de la funció que envia una dada per http

void setup() {

  Serial.begin(115200);   //inicialitza port sèrie de hardware
  HC.begin(2400);         //inicialitza port sèrie virtual

  pinMode(0, OUTPUT);     //Led d'informació connectat al pin GPIO0

  WiFi.setSleepMode(WIFI_NONE_SLEEP);   //incrementa el consum d'energia, però ajuda amb la fiabilitat de la conexió
  WiFi.mode(WIFI_STA);    //defineix el mode de funcionament del Wi-Fi
  WiFi.begin(WIFI_SSID , WIFI_PASS);  //connecta a la xarxa Wi-Fi amb la informació proporcionada anteriorment.
  
  Serial.print("connecting to wifi  network");  //espera mentre s'estableix la connexió
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");    //connexió establerta, podem continuar
  
}

void loop() {

  //la següent condició s'activara quan s'hagi rebut dades del receptor de ràdio
  if(HC.available()){

    String message = HC.readString();   //emmagatzema la informació rebuda en una variable
    Serial.print("data received: ");
    Serial.println(message);            //imprimeix la informació
    digitalWrite(0, 1);    //curt avís lluminós sobre la rebuda de dades.
    delay(100);
    digitalWrite(0, 0);

    String temperature = message.substring(0, message.indexOf(","));    //separa la informació
    message.remove(0, temperature.length() + 1);
    String pressure = message.substring(0, message.indexOf(","));
    message.remove(0, pressure.length() + 1);
    String humidity = message.substring(0, message.indexOf(","));
    message.remove(0, humidity.length() + 1);
    String light = message.substring(0, message.indexOf(";"));
    
    sendData("tr.temperature", temperature);    //envia cada dada amb una petició HTTP independent
    delay(100);
    sendData("tr.pressure", pressure);
    delay(100);
    sendData("tr.humidity", humidity);
    delay(100);
    sendData("tr.light", light);

  }
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
