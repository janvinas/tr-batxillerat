/*
 *  PART EXTERIOR DE L'ESTACIÓ METEOROLÒGICA
 * 
 *   Treball de Recerca de Batxillerat 2020
 * 
 *       CC BY 4.0 - Jan Viñas Corella
 * 
 */

#include <LowPower.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define activateSerial false    //només s'imprimirà missatges a serial si això està activat

#define AT_PIN 12
#define SENSOR_PIN 2

SoftwareSerial HC (4,5);

bool debug(String s){
  if(activateSerial){
    Serial.println(s);
    return true;
  }
  return false;
}

void setup() {
  pinMode(SENSOR_PIN, OUTPUT);
  pinMode(AT_PIN, OUTPUT);
  digitalWrite(AT_PIN, HIGH);

  if(activateSerial){
    Serial.begin(4800);   //inicialitza el port sèrie
    delay(10);
  }

  HC.begin(2400);
  
}

void loop() {

  digitalWrite(SENSOR_PIN, 1);
  delay(1000);
  float temperature = 0;
  float pressure = 0;
  float humidity = 0;

  Adafruit_BME280 bme;
  if(!bme.begin()){
    debug("error initializing bme");
    temperature = 0;
    pressure = 0;
    humidity = 0;
  }else{
    temperature = bme.readTemperature();  //llegeix informació del sensor ambiental
    pressure = bme.readPressure();
    humidity = bme.readHumidity();
  }
  
  digitalWrite(SENSOR_PIN, 0);
  
  int light = analogRead(A0);                 //llegeix informació del sensor de llum
  
  String dataString =                         //converteix els valors a un objecte String
    String(temperature) + "," + 
    String(pressure) + "," + 
    String(humidity) + "," +
    String(light) + ";";
  
  debug(dataString);  

  HC.print(dataString);

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //espera 24 segons amb el microcontrolador
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //apagat, per estalviar energia.
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  
}
