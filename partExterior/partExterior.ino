/*
 *  PART EXTERIOR DE L'ESTACIÓ METEOROLÒGICA
 * 
 *   Treball de Recerca de Batxillerat 2020
 * 
 */ 

/*
 *  Connexions a l'Arduino:
 * 
 *  BME280    | SDA  -> A4
 *            | SCL  -> A5
 *  
 *  RF 433MHz | DATA -> D12
 *  
 *  LDR       |      -> A0
 * 
 */

#include <LowPower.h>
#include <RH_ASK.h>
#include <SPI.h>    //No s'utilitza però és necessaria per compilar RadioHead
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define activateSerial false    //només s'imprimirà missatges a serial si això està activat

RH_ASK driver;          //crea un objecte pel transmissor de ràdio
Adafruit_BME280 bme;    //crea un objecte pel sensor ambiental

void setup() {

  if(activateSerial){
    Serial.begin(9600);   //inicialitza el port sèrie
    delay(10);
  }
  
  driver.init();  //inicialitza el transmissor de ràdio
  bme.begin();    //inicialitza el sensor ambiental

  if(activateSerial) Serial.print("Setup acabat");
  
}

void loop() {
  
  float temperature = bme.readTemperature();  //llegeix informació del sensor ambiental
  float pressure = bme.readPressure();
  float humidity = bme.readHumidity();
  int light = analogRead(A0);                 //llegeix informació del sensor de llum
  
  String dataString =                         //converteix els valors a un objecte String
    String(temperature) + "," + 
    String(pressure) + "," + 
    String(humidity) + "," +
    String(light) + ";";

  const char *msg = dataString.c_str();       //converteix String a c_str, necessari per RadioHead
  
  if (activateSerial) Serial.println(stringReading);  

  driver.send((uint8_t *)msg, strlen(msg));   //envia el c_str per ràdio
  driver.waitPacketSent();                    //espera mentre s'envia

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //espera 24 segons amb el microcontrolador
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //apagat, per estalviar energia.
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  
}
