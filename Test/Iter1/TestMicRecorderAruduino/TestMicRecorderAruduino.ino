// ПОД ARDUINO NANO С ПОНИЖЕНИЕМ ЧАСТОТЫ ПРОШИВАТЬ ЧЕРЕЗ Arduino Pro or Pro Mini 
// В ПРОЦЕССОРЕ УКАЗАТЬ ЧТО 8 МГЦ

#include <Wire.h>
#include <DS3231.h>
                  // Подключаем библиотеку Wire  
#include <GyverPower.h>                    // Подключаем библиотеку Wire  

#include <SdFat.h>
#include <SPI.h>
#include <TMRpcm.h>

#include <TimerMs.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define SD_ChipSelectPin 10
TMRpcm audio;
SdFat SD;

char* dash = "-";
char* slash = "/";
char* typeAudio = ".wav";
#define Mic A0
#define Mic2 A2
#define Rate 32000
#define RecDuration 30000 

int ind = 1;
bool isS = false;
byte hourStart;

DS3231 rtc;                 // Инициализация DS3231
RTClib myRTC;

DateTime now;

TimerMs tmr(RecDuration, 1, 1);

char* curFileCA;
String res = "";
void UpdateNowTime(){
  now = myRTC.now();
}

/*char* getDate(){
  String res = "";

  res += now.day();
  res += now.month();
  res += now.year();
  return res.c_str();
}

char* getTime(){
  String res = "";

  res += now.hour();
  res += '-';
  res += now.minute();
  return res.c_str();
}

char* getDateTime(){
  String res = "";

  res += getDate();
  res += "-";
  res += getTime();
  return res.c_str();
}
*/
char* getFileName(){
  UpdateNowTime();
  res = isS ? "S" : "";
  /*res += now.day();
  res += now.month();
  res += now.year();
  res += dash;
  res += now.hour();
  res += dash;
  res += now.minute();*/ 
  // было закомнчено так как отпал провод на модуле времени. Костыль небольшой
  res += ind;
  res += typeAudio;

  ind++;
  return res.c_str();
}

void startRec(){
  isS = !isS;
  // Тестовай штука. Было сделано для записи двух каналов: Mic без кондера и Mic2 с кондером

  curFileCA = getFileName();

  audio.startRecording(getFileName(), Rate, isS ? Mic2: Mic);
}

void stopRec(){
  Serial.print("Stop rec ");
  Serial.println(curFileCA);

  audio.stopRecording(curFileCA);
}

void restartRec(){
  
  stopRec();
  startRec();
}

void setup()
{
  //штука для повышения частоты опроса АЦП. 
  // у гайвера в светомузыке есть
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  //понижения частоты до 8 мгц
  power.setSystemPrescaler(PRESCALER_2);

  Serial.begin(9600);                // Установка последовательного соединения
  Wire.begin();                         // Инициализировать rtc

  pinMode(Mic2, INPUT);
  pinMode(Mic2, INPUT);
  
  tmr.setPeriodMode(); 

  if (!SD.begin(SD_ChipSelectPin)) {  
    return;
  }else{
    Serial.println("SD OK"); 
  } 

  audio.CSPin = SD_ChipSelectPin;

  res.reserve(30);
  startRec();
}

void loop()
{
  if (tmr.tick()){ //период задается в RecDuration
    restartRec();
  }
}