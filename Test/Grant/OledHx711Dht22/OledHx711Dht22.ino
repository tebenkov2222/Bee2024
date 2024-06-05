#include <GyverOLED.h>
#include <GyverPower.h>

#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 15;
const int LOADCELL_SCK_PIN = 14;

HX711 scale;

long zero = -62000;
float scaler = -18.95; //-15.769
long reading = 0;
float mass = 0;

#include "DHT.h"
#define DHTPIN 8
DHT dht(DHTPIN, DHT22);
float h;
float t;

GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

#define BTN 3

void setup() {
  power.setSystemPrescaler(PRESCALER_2);

  dht.begin();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  pinMode(BTN, INPUT); 
  oled.init();        // инициализация
  oled.clear();       // очистка
  oled.setScale(3);   // масштаб текста (1..4)
  oled.home();        // курсор в 0,0
  oled.print("Привет!");
  oled.setScale(1);
  delay(1000);
  readDhtData();
  readMass();
  oled.clear();
}
void loop() {
  if(!digitalRead(BTN))
    return;
  readDhtData();
  readMass();
  
  oled.clear();
  
  oled.setCursorXY(20, 10);
  oled.print("Temp = ");
  oled.print(t);
  oled.print("C");

  oled.setCursorXY(20, 30);
  oled.print(" Hum = ");
  oled.print(h);
  oled.print("%");

  oled.setCursorXY(20, 50);
  oled.print("Mass = ");
  oled.print(mass);
  oled.print("kg");
  delay(10000);
  oled.clear();
}

void readDhtData(){
  h = dht.readHumidity(); //Измеряем влажность
  t = dht.readTemperature(); //Измеряем температуру
}

void readMass(){
  reading = scale.read();
  float res = (float)(reading - zero) / scaler;
  
  mass = (res)/1000.0;
  return mass;
}