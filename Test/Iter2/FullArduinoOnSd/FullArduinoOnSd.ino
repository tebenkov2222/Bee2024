#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 7);

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <DS3231.h>
DateTime now;

DS3231 rtc;                 // Инициализация DS3231
RTClib myRTC;

#include <GyverPower.h>

#include "DHT.h"
#define DHTPIN1 5
DHT dht1(DHTPIN1, DHT22);

#define DHTPIN2 6
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
float scaler = -20.1671; //-15.769 // -28.7916
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
unsigned long checkDataTime = 600000;
//unsigned long checkDataTime = 12000;
//int checkDataTime = 2000;
unsigned long currentCheckButtonTime = 0;

float busvoltage_main;
float current_mA_main;
float power_mW_main;

float busvoltage_solar;
float current_mA_solar;
float power_mW_solar;

int signal;
int response_pass;
int response_transmit;

const char dot = '.';
const char comma = ',';
const char colon = ':';
const char semicolon = ';';
const char boxOpen = '[';
const char boxClose = ']';

Adafruit_INA219 ina219(0x44);
Adafruit_INA219 ina231(0x40);

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

void initIna(){
  if (!ina219.begin()) {
      Serial.println("Не удалось найти INA219 чип");
  }
  if (!ina231.begin()) {
      Serial.println("Не удалось найти INA231 чип");
  }
}
void setup() {
  power.setSystemPrescaler(PRESCALER_2);
  pinMode(BTN, INPUT); 

  res.reserve(16);
  fileName.reserve(16);

  Serial.begin(115200);
  Serial.println("Start progam");

  mySerial.begin(9600);
  //mySerial.println("Hello world from Serial2!");

  initOled();
  initRtc();
  initDht();
  initIna();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  delay(1000);
  oled.clear();
  //testOled();

  oled.home();        // курсор в 0,0
  oled.print("Загрузка...");
  //currentDeltaMillis = getDeltaMillis();
  delay(10000);
  oled.clear();

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

  checkResult();
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


const char enter = 13;
const int chipSelect = 4;
int iter = 0;
bool isStarted;
String inputLine;

void checkResult(){
  if (mySerial.available()) {
    char input = mySerial.read();
    if(input == enter){
      if(isStarted){
        if (iter == 0){
          signal = inputLine.toInt();
        }
        if (iter == 1){
          response_pass = inputLine.toInt();
        }
        if (iter == 2){
          response_transmit = inputLine.toInt();
          isStarted = false;
        }
        iter++;
      }
      if(inputLine == "!"){
        iter = 0;
        isStarted = true;
      }
      inputLine = "";
    }
    else{
      if(input != 10)
        inputLine += input;
    }
  }
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
  mySerial.print(h2);
  mySerial.print(semicolon);
  mySerial.print(t2);
  mySerial.print(semicolon);
  mySerial.print(mass);
  mySerial.print(semicolon);
  mySerial.print(busvoltage_main);
  mySerial.print(semicolon);
  mySerial.print(current_mA_main);
  mySerial.print(semicolon);
  mySerial.print(power_mW_main);
  mySerial.print(semicolon);
  mySerial.print(busvoltage_solar);
  mySerial.print(semicolon);
  mySerial.print(current_mA_solar);
  mySerial.print(semicolon);
  mySerial.print(power_mW_solar);
  mySerial.print(semicolon);
  mySerial.print(signal);
  mySerial.print(semicolon);
  mySerial.print(response_pass);
  mySerial.print(semicolon);
  mySerial.print(response_transmit);
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
  Serial.println("READ DHT");
  
  readDhtData();
  Serial.println("READ MASS");

  readMass(); 
  Serial.println("READ MIC");

  readIna();
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

void readIna(){
  busvoltage_main = 0;
  current_mA_main = 0;
  power_mW_main = 0;
  float busV = ina219.getShuntVoltage_mV();
  busvoltage_main = ina219.getBusVoltage_V();
  current_mA_main = ina219.getCurrent_mA();
  power_mW_main = ina219.getPower_mW();

  Serial.println("Akk");
  Serial.print("Шунт. Напряжение:   "); Serial.print(busV); Serial.println(" мВ");
  Serial.print("Напряжение на шине: "); Serial.print(busvoltage_main); Serial.println(" В");
  Serial.print("Ток:                "); Serial.print(current_mA_main); Serial.println(" мА");
  Serial.print("Мощность:           "); Serial.print(power_mW_main); Serial.println(" мВт");
  Serial.println("");
  busvoltage_solar = 0;
  current_mA_solar = 0;
  power_mW_solar = 0;
  float busVS = ina231.getShuntVoltage_mV();
  busvoltage_solar = ina231.getBusVoltage_V();
  current_mA_solar = ina231.getCurrent_mA();
  power_mW_solar = ina231.getPower_mW();

  
  Serial.println("Solar");
  Serial.print("Шунт. Напряжение:   "); Serial.print(busVS); Serial.println(" мВ");
  Serial.print("Напряжение на шине: "); Serial.print(busvoltage_solar); Serial.println(" В");
  Serial.print("Ток:                "); Serial.print(current_mA_solar); Serial.println(" мА");
  Serial.print("Мощность:           "); Serial.print(power_mW_solar); Serial.println(" мВт");
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
    else if(currentIndex == 3){
      ViewSlide3();
    }
    else if(currentIndex == 4){
      ViewSlide4();
    }
    else if(currentIndex == 5){
      ViewSlide5();
    }
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
  oled.setCursorXY(10, 0);

  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Аккум");

  oled.setCursorXY(7, 17);
  oled.print("Напр");

  oled.setCursorXY(64, 17);
  oled.print("Ток,ма");

  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(0, 45);
  oled.print(busvoltage_main,1);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(64, 45);
  oled.print(current_mA_main,0);
}

void ViewSlide4(){
  oled.clear();
  oled.setCursorXY(10, 0);

  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Солн панель");

  oled.setCursorXY(7, 17);
  oled.print("Напр");

  oled.setCursorXY(64, 17);
  oled.print("Ток,ма");

  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(0, 45);
  oled.print(busvoltage_solar,1);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(64, 45);
  oled.print(current_mA_solar, 0);
}

void ViewSlide5(){
  oled.clear();
  oled.setCursorXY(10, 0);

  oled.setScale(2);   // масштаб текста (1..4)
  oled.print("Sim");

  oled.setCursorXY(55, 17);
  oled.print(signal);

  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(0, 45);
  oled.print(response_pass);
  oled.setScale(2);   // масштаб текста (1..4)
  oled.setCursorXY(64, 45);
  oled.print(response_transmit);
}