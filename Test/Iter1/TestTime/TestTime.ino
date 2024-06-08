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

  if(true){
    autoUpdateDateTime();
    /*rtc.setHour(12);
    rtc.setMinute(8);
    rtc.setSecond(0);
    
    rtc.setDate(26);
    rtc.setMonth(5);
    rtc.setYear(1976); //костыль. хз почему то это 2024 год*/
  }
}

void autoUpdateDateTime(){
      String date = __DATE__; // "MMM DD YYYY"
    String time = __TIME__; // "HH:MM:SS"

    // Разбираем дату
    int month = 0;
    if (date.substring(0, 3) == "Jan") month = 1;
    else if (date.substring(0, 3) == "Feb") month = 2;
    else if (date.substring(0, 3) == "Mar") month = 3;
    else if (date.substring(0, 3) == "Apr") month = 4;
    else if (date.substring(0, 3) == "May") month = 5;
    else if (date.substring(0, 3) == "Jun") month = 6;
    else if (date.substring(0, 3) == "Jul") month = 7;
    else if (date.substring(0, 3) == "Aug") month = 8;
    else if (date.substring(0, 3) == "Sep") month = 9;
    else if (date.substring(0, 3) == "Oct") month = 10;
    else if (date.substring(0, 3) == "Nov") month = 11;
    else if (date.substring(0, 3) == "Dec") month = 12;

    int day = date.substring(4, 6).toInt();
    int year = date.substring(7, 11).toInt() - 2000; // Преобразуем в формат, понятный библиотеке (24 для 2024 года)

    // Разбираем время
    int hour = time.substring(0, 2).toInt();
    int minute = time.substring(3, 5).toInt();
    int second = time.substring(6, 8).toInt();

    // Устанавливаем время и дату
    rtc.setYear(year);
    rtc.setMonth(month);
    rtc.setDate(day);
    rtc.setHour(hour);
    rtc.setMinute(minute);
    rtc.setSecond(second);
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
  Serial.print(';');
  Serial.println();

  delay (1000);                        // Задержка в одну секунду
}