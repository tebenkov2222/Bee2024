#include <Wire.h>
#include <DS3231.h>
                  // Подключаем библиотеку Wire  
#include <GyverPower.h>                    // Подключаем библиотеку Wire  

DS3231 rtc;                 // Инициализация DS3231
RTClib myRTC;

void setup()
{
  power.setSystemPrescaler(PRESCALER_2);



  Serial.begin(115200);                // Установка последовательного соединения
  Wire.begin();                         // Инициализировать rtc
// Установка времени
  rtc.setClockMode(false);

  rtc.setHour(23);
  rtc.setMinute(0);
  rtc.setSecond(0);
  
  rtc.setDate(10);
  rtc.setMonth(3);
  rtc.setYear(1976); //костыль. хз почему то это 2024 год
  
}
void loop()
{
  DateTime now = myRTC.now();
  
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay (1000);                        // Задержка в одну секунду
}