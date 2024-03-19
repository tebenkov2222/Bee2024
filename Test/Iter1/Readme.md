# Iter1

### TestMicRecorderAruduino
Проект для записи с микрофона в сд карту.

Ссылки:
- https://radioprog.ru/post/270
- https://github.com/TMRh20/TMRpcm
- https://arduinoplus.ru/arduino-delaem-shpionskij-zhuchok-dlya-proslushki/

Пины:

| Micro SD     | CS | SCK | Mosi | Miso |
|--------------|----|-----|------|------|
| Arduino nano | 10 | 13  | 11   | 12   |

| Mic          | Mic | Mic2 | 
|--------------|-----|------|
| Arduino nano | A0  | A2   |

| DS3231       | Sda | Scl | 
|--------------|-----|-----|
| Arduino nano | A4  | A5  |

### TestTime

Прога для записи времени и проверки модуля на работоспособность

Ссылки:
- https://3d-diy.ru/wiki/arduino-moduli/chasy-realnogo-vremeni-ds3231/


### TestMicroSd 

Прога для определния SD карты и модуля. Если все ок то там выйдет основная инфа об этом модуле

Ссылки:
- https://arduinomaster.ru/datchiki-arduino/podklyuchenie-sd-karty-k-arduino/