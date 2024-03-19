#include <Wire.h>
#include <DS3231.h>
                  // Подключаем библиотеку Wire  
//#include <GyverPower.h>                    // Подключаем библиотеку Wire  

#include <SdFat.h>
#include <SPI.h>
#include <TMRpcm.h>

#include <TimerMs.h>

//#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
//#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define SD_ChipSelectPin PA4
TMRpcm audio;
SdFat SD;

char* dash = "-";
char* slash = "/";
char* typeAudio = ".wav";
#define Mic PB0
#define Mic2 PB1
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
  res += ind;
  res += typeAudio;

  ind++;
  char* buf;
  res.toCharArray(buf, 30);
  return buf;
}

void startRec(){
  isS = !isS;

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
  //sbi(ADCSRA, ADPS2);
  //cbi(ADCSRA, ADPS1);
  //sbi(ADCSRA, ADPS0);

  //power.setSystemPrescaler(PRESCALER_2);

  Serial.begin(9600);                // Установка последовательного соединения
  Wire.begin();                         // Инициализировать rtc

  rtc.setClockMode(false);              //  Установить время 16:29:00 (формат 24 часа)

  rtc.setHour(0);              //  Установить время 16:29:00 (формат 24 часа)
  rtc.setMinute(0);              //  Установить время 16:29:00 (формат 24 часа)
  rtc.setSecond(0);              //  Установить время 16:29:00 (формат 24 часа)
  
  rtc.setDate(1);            //  Установить дату 31 августа 2018 года
  rtc.setMonth(1);            //  Установить дату 31 августа 2018 года
  rtc.setYear(1977);            //  Установить дату 31 августа 2018 года


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
  if (tmr.tick()){
    restartRec();
  }
}