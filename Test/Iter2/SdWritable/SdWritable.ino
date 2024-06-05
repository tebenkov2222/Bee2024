#include <GyverPower.h>

#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8);


#include <SPI.h>
#include <SD.h>

const char enter = 13;
const int chipSelect = 4;

String inputLine;

bool isStarted = false;
bool isDateNow = true;
String dateNow;
String data;
void setup()  
{
  inputLine.reserve(128);
  power.setSystemPrescaler(PRESCALER_2);

  // Инициализируем последовательный интерфейс и ждём открытия порта:
  Serial.begin(115200);
 
  // устанавливаем скорость передачи данных для последовательного порта
  mySerial.begin(9600);

  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card Failure");
    return;
	}
  else{
    Serial.println("Card Connected");

  }
}

void loop() {
  if (mySerial.available()){
    char input = mySerial.read();
    if(input == enter){
      if(isStarted){
        if(isDateNow){
          dateNow = String(inputLine);
        }
        else{
          data = String(inputLine);
          isStarted = false;
          WriteOnSd();
        }
        isDateNow = !isDateNow;
      }
      if(inputLine == "!"){
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

String filename;
File myFile ;
void WriteOnSd(){
  filename = dateNow + ".csv";//25d12.csv";// + ".csv";
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    myFile.println(data);
    // close the file:
    myFile.close();
    Serial.println("---");
    Serial.println(dateNow);
    Serial.println(data);
    Serial.println("***");
  } else {
    Serial.println("error opening " + filename);
  }
}
