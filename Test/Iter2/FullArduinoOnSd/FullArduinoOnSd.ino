#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8);

#include <Wire.h>
#include <DS3231.h>
DateTime now;

DS3231 rtc;                 // Инициализация DS3231
RTClib myRTC;

#include <GyverPower.h>

#include "DHT.h"
#define DHTPIN1 5
DHT dht1(DHTPIN1, DHT22);

#define DHTPIN2 4
DHT dht2(DHTPIN2, DHT22);

float h1;
float t1;
float h2, t2;
#include "HX711.h"

HX711 scale;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 15;
const int LOADCELL_SCK_PIN = 14;

long zero = -27500;
float scaler = -28.7916; //-15.769
long reading = 0;
float mass = 0;

#define SOUND_R_FREQ A3    // аналоговый пин вход аудио для режима с частотами (через кондер)
byte freqMicResult[16];
#define LOG_OUT 1 // use the log output function
#define FHT_N 128         // ширина спектра х2
#include <FHT.h>         // преобразование Хартли

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#include <GyverOLED.h>
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

#define BTN 3

String res = "";
String fileName = "";

unsigned long currentCheckDataTime = 0;
unsigned long checkDataTime = 60000;
//int checkDataTime = 2000;
unsigned long currentCheckButtonTime = 0;
float voltage = 0;
float voltageSolar = 0;

const char dot = '.';
const char comma = ',';
const char colon = ':';
const char semicolon = ';';
const char boxOpen = '[';
const char boxClose = ']';

void initOled(){
  oled.init();        // инициализация
  oled.clear();       // очистка
  oled.setScale(3);   // масштаб текста (1..4)
  oled.home();        // курсор в 0,0
  oled.print("Привет!");
}

void initRtc(){
  Wire.begin();                         // Инициализировать rtc
// Установка времени
  rtc.setClockMode(false);
}

void initDht(){
  dht1.begin();
  dht2.begin();
}

void initMic(){
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);
}

void setup() {
  power.setSystemPrescaler(PRESCALER_2);
  pinMode(BTN, INPUT); 

  res.reserve(16);
  fileName.reserve(16);

  Serial.begin(115200);
  Serial.println("Start progam");

  mySerial.begin(9600);

  initOled();
  initRtc();
  initDht();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  delay(1000);
  oled.clear();
  //testOled();

  //currentDeltaMillis = getDeltaMillis();
  readAllData();
  writeAllData();
}

String getDateNow(){

  res = "";
  res += String(now.day()) + 'd';
  res += String(now.month()) + 'm';
  return res;
}

String getTimeNow(){
  res = "";
  res += String(now.hour()) + colon;
  res += String(now.minute()) + colon;
  res += String(now.second());
  return res;
}

void loop() {

  CheckButton();
  ViewDataOnDisplay();
  if(millis() < currentCheckDataTime)
    currentCheckDataTime = 0;
  if(millis() - currentCheckDataTime < checkDataTime)
    return;
  currentCheckDataTime = millis();
  Serial.println("READ DATA");

  readAllData();

  Serial.println("WRITE DATA");

  writeAllData();
}

void writeAllData(){

  //initSd();
  getDateNow();
  getTimeNow();
  mySerial.println("!");
  
  mySerial.println(getDateNow());

  mySerial.print(getTimeNow());
  mySerial.print(semicolon);
  mySerial.print(h1);
  mySerial.print(semicolon);
  mySerial.print(t1);
  mySerial.print(semicolon);
  mySerial.print(mass);
  mySerial.print(semicolon);
  mySerial.print(voltage);
  mySerial.print(semicolon);
  mySerial.print(boxOpen);

  for (byte j = 0; j < 15; j++) {
    mySerial.print(freqMicResult[j]);
    mySerial.print(comma);
  }
  mySerial.print(freqMicResult[15]);
  mySerial.print(boxClose);
  mySerial.println(semicolon);
  Serial.print("SEND DATA:");
  Serial.println(getTimeNow());

  return;
/*
  Serial.print(getDateNow());
  Serial.print(".txt -> ");
  Serial.print(getTimeNow());
  Serial.print(';');
  Serial.print(h1);
  Serial.print(';');
  Serial.print(t1);
  Serial.print(';');
  Serial.print(mass);
  Serial.print(';');
  Serial.print(voltage);
  Serial.print(';');
  Serial.print('[');

  for (byte j = 0; j < 15; j++) {
    Serial.print(freqMicResult[j]);
    Serial.print(',');
  }
  Serial.print(freqMicResult[15]);
  Serial.print(']');
  Serial.println(';');*/
}

void readAllData(){
  now = myRTC.now();
  voltage = analogRead(A7) / 1023.0 * 5.0;
  Serial.println("READ DHT");
  
  readDhtData();
  Serial.println("READ MASS");

  readMass(); 
  Serial.println("READ MIC");

  analyzeSampleAudio();
}

void readDhtData(){
  h1 = dht1.readHumidity(); //Измеряем влажность
  t1 = dht1.readTemperature(); //Измеряем температуру
  
  h2 = dht2.readHumidity(); //Измеряем влажность
  t2 = dht2.readTemperature(); //Измеряем температуру
}

void readMass(){
  reading = scale.read();
  float res = (float)(reading - zero) / scaler;
  
  mass = (res)/1000.0;
}

void analyzeSampleAudio() {
  for (int i = 0 ; i < FHT_N ; i++) {
    int sample = analogRead(SOUND_R_FREQ);
    fht_input[i] = sample; // put real data into bins
  }
  fht_window();  // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run();     // process the data in the fht
  fht_mag_log(); // take the output of the fht
  for (byte j = 2; j < 18; j++) {         // первые 2 канала - хлам
    freqMicResult[j-2] = fht_log_out[j];
  }
}

bool isActiveDisplay;
int currentIndex = 0;
unsigned long millisActiveDisplay;
void CheckButton(){
  if(!digitalRead(BTN))
    return;
  isActiveDisplay = true;
  millisActiveDisplay = millis();
  readAllData();
  getDateNow();
  getTimeNow();
  ViewSlideStart();
  currentIndex = 0;
}

void ViewDataOnDisplay(){
  if(!isActiveDisplay)
    return;
  if(millis() < millisActiveDisplay)
    millisActiveDisplay = 0;
  if(millis() - millisActiveDisplay > 2000){
    if(currentIndex == 0){
      ViewSlide0();
    }
    else if(currentIndex == 1){
      ViewSlide1();
    }
    else if(currentIndex == 2){
      ViewSlide2();
    }
    /*else if(currentIndex == 3){
      ViewSlide3();
    }*/
    /*else if (currentIndex == 4){
      ViewSlide4();
    }*/
    else{
      currentIndex = 0;
      oled.clear();

      isActiveDisplay = false;
    }
    millisActiveDisplay = millis();
    currentIndex++;
  }
}

void ViewSlideStart(){
  oled.clear();
  oled.setCursorXY(0, 0);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Умный Улей");

  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(5, 20);
  oled.print(now.day());
  oled.print(".");
  oled.print(now.month());
  oled.print(".");
  oled.print(now.year());
  oled.setScale(3);   // масштаб текста (1..4)
  oled.setCursorXY(15, 40);

  oled.print(now.hour());
  oled.print(":");
  oled.print(now.minute());
}

void ViewSlide0(){
  oled.clear();
  oled.setCursorXY(45, 0);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Вес");

  oled.setScale(3);   // масштаб текста (1..4)
  oled.setCursorXY(0, 30);
  oled.print(mass);
}

void ViewSlide1(){
  oled.clear();
  oled.setCursorXY(0, 0);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Внутри");

  oled.setCursorXY(7, 17);
  oled.print("Темп");

  oled.setCursorXY(72, 17);
  oled.print("Влаж");

  oled.setCursorXY(0, 45);
  oled.print(t1);

  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(64, 45);
  oled.print(h1);
}

void ViewSlide2(){
  oled.clear();
  oled.setCursorXY(10, 0);

  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Снаружи");

  oled.setCursorXY(7, 17);
  oled.print("Темп");

  oled.setCursorXY(72, 17);
  oled.print("Влаж");

  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(0, 45);
  oled.print(t2);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(64, 45);
  oled.print(h2);
}

void ViewSlide3(){
  oled.clear();
  oled.setCursorXY(0, 0);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Напряжение");

  oled.setScale(3);   // масштаб текста (1..4)
  oled.setCursorXY(0, 30);
  oled.print(voltage);
}

void ViewSlide4(){
  oled.clear();
  oled.setCursorXY(0, 0);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Напряжение СБ");

  oled.setScale(3);   // масштаб текста (1..4)
  oled.setCursorXY(0, 30);
  oled.print(voltageSolar);
}