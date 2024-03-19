#include <SPI.h>
#include <SD.h>

Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = PA4;

void setup() {
  //power.setSystemPrescaler(PRESCALER_2);
  
  Serial1.begin(115200);
  Serial1.print("\nInitializing SD card...");

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    // неверное подключение или карта неисправна
    Serial1.println("initialization failed");
    return;
  } else {
    // всё ок!
    Serial1.println("Wiring is correct and a card is present.");
  }

  // считываем тип карты и выводим его в COM-порт
  Serial1.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial1.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial1.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial1.println("SDHC");
      break;
    default:
      Serial1.println("Unknown");
  }

  // инициализация файловой системы
  if (!volume.init(card)) {
    // неверная файловая система
    Serial1.println("Could not find FAT16/FAT32 partition.");
    return;
  }

  // считываем тип и вычисляем размер первого раздела
  uint32_t volumesize;
  Serial1.print("\nVolume type is FAT");
  Serial1.println(volume.fatType(), DEC);
  Serial1.println();

  volumesize = volume.blocksPerCluster();  // блоков на кластер
  volumesize *= volume.clusterCount();     // кластеров
  volumesize *= 512;                       // 512 байтов в блоке, итого байт..
  Serial1.print("Volume size (bytes): ");
  Serial1.println(volumesize);
  Serial1.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial1.println(volumesize);
  Serial1.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial1.println(volumesize);

  Serial1.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  // выводим список файлов
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop(void) {
}