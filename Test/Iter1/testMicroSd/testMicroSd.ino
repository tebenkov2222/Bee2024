#include <SPI.h>
#include <SD.h>
#include <GyverPower.h>

Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;

void setup() {
  power.setSystemPrescaler(PRESCALER_2);

  Serial.begin(115200);
  Serial.print("\nInitializing SD card...");

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    // неверное подключение или карта неисправна
    Serial.println("initialization failed");
    return;
  } else {
    // всё ок!
    Serial.println("Wiring is correct and a card is present.");
  }

  // считываем тип карты и выводим его в COM-порт
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // инициализация файловой системы
  if (!volume.init(card)) {
    // неверная файловая система
    Serial.println("Could not find FAT16/FAT32 partition.");
    return;
  }

  // считываем тип и вычисляем размер первого раздела
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();  // блоков на кластер
  volumesize *= volume.clusterCount();     // кластеров
  volumesize *= 512;                       // 512 байтов в блоке, итого байт..
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  // выводим список файлов
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop(void) {
  delay(1000);
  File dataFile = SD.open("test.txt", FILE_WRITE);
    String dataString = "Hello from RobotClass";

    if (dataFile) {
        // записываем строку в файл
        dataFile.println(dataString);
        dataFile.close();
        Serial.println("Success!");
    } else {
        // выводим ошибку если не удалось открыть файл
        Serial.println("error opening file");
    }

    Serial.println("Read test.txt");

    File myFile = SD.open("test.txt");
    if (myFile) {
        // считываем все байты из файла и выводим их в COM-порт
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // закрываем файл
        myFile.close();
    } else {
        // выводим ошибку если не удалось открыть файл
        Serial.println("error opening test.txt");
    }
}