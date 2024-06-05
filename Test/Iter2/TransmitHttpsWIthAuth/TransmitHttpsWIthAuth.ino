#include <Arduino.h>
// Глобальные переменные
static String serverUrl = F("https://rest-api.bee.elddir.ru");
static String apn = F("internet.tele2.ru");
bool useSsl;

// Прототипы функций
void initModule();
void connectToInternet(const String& apn);
String getImei();
String getPassword(const String& url);
void transmitData(const String& url, const String& login, const String& password, const String& data);
void enterPowerSavingMode();
String sendATCommand(const String& command, const int timeout, bool debug);
String savedImai;
void setup() {
  savedImai.reserve(16);
  // Начинаем общение с модулем и монитором порта
  Serial.begin(9600);
  Serial2.begin(115200);
  
  Serial.println("initModule");

  // Инициализация модуля SIM800L
  initModule();
  delay(1000);
  Serial.println("checkPassAndImai");

  checkImai();
  sendData("Hello world");
  //checkPass();
  //checkPassAndImai();
}

void loop() {
}

void checkImai(){
  savedImai = getImei();
  Serial.print("Imai = '");
  Serial.print(savedImai);
  Serial.println("'");
}

void checkPass(){
  connectToInternet(apn);
  String password = getPassword(serverUrl + "/getpass");
  Serial.println();
  Serial.print("Pass = ");
  Serial.println(password);
  enterPowerSavingMode();
}


void sendData(String data){
  connectToInternet(apn);
  String password = getPassword(serverUrl + "/getpass");
  transmitData(serverUrl + "/transmitdata", savedImai, password, data);
  enterPowerSavingMode();
}

void initModule() {
  String response;
  sendATCommand("AT", 1000, true);
  sendATCommand("AT+CFUN=1,1", 10000, true);
  
  // Ожидание строки "SMS Ready"
  while (true) {
    response = sendATCommand("AT", 1000, false);
    if (response.indexOf("SMS Ready") != -1) {
      Serial.println("SMS Ready detected");
      break;
    }
    delay(1000);
  }
}

void connectToInternet(const String& apn) {
  sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 2000, true);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"", 2000, true);
  sendATCommand("AT+SAPBR=1,1", 3000, true);
  sendATCommand("AT+SAPBR=2,1", 3000, true);
}

String getImei() {
  String response = sendATCommand("AT+GSN", 2000, false);
  response.trim(); // Убираем лишние пробелы и символы новой строки
  int index = response.indexOf("AT+GSN");
  if (index != -1) {
    response = response.substring(index + 6); // Убираем команду AT+GSN
  }
  response.replace("\r", ""); // Убираем символы возврата каретки
  response.replace("\n", ""); // Убираем символы новой строки
  response.replace("OK", ""); // Убираем "OK" из ответа
  return response;
}

String getPassword(const String& url) {
  sendATCommand("AT+HTTPINIT", 2000, true);
  sendATCommand("AT+HTTPPARA=\"CID\",1", 2000, true);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", 2000, true);
  //sendATCommand("AT+HTTPSSL=1", 2000, true); // Включаем SSL
  sendATCommand("AT+HTTPACTION=0", 6000, true); // GET запрос
  
  String response = sendATCommand("AT+HTTPREAD", 10000, true);

  response.trim();
  sendATCommand("AT+HTTPTERM", 2000, true);

  String result = "";
  bool isFindFirst = false;
  bool isStarted = false;
  for(byte i = 0; i < response.length(); i++){
    byte id = byte(response[i]);
    if(isFindFirst){
      if(id == 10){
        isStarted = true;
      }
      if(isStarted){
        if(id == 13){
          isStarted = false;
        }
        else{
          result += response[i];
        }
      }
    }
    else{
      if(id == 10){
        isFindFirst = true;
      }
    }
  }
  result.trim();
  return result;
}

void transmitData(const String& url, const String& login, const String& password, const String& data) {
  sendATCommand("AT+HTTPINIT", 2000, true);
  sendATCommand("AT+HTTPPARA=\"CID\",1", 2000, true);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", 2000, true);
  //sendATCommand("AT+HTTPSSL=1", 2000, true); // Включаем SSL
  sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"", 2000, true);

  String postData = "login=" + login + "&password=" + password + "&data=" + data;
  sendATCommand("AT+HTTPDATA=" + String(postData.length()) + ",10000", 2000, true);
  
  delay(100); // Задержка для обработки команды
  Serial2.print(postData);
  delay(10000); // Задержка для отправки данных

  sendATCommand("AT+HTTPACTION=1", 6000, true); // POST запрос
  sendATCommand("AT+HTTPREAD", 10000, true);
  sendATCommand("AT+HTTPTERM", 2000, true);
}

void enterPowerSavingMode() {

  sendATCommand("AT+SAPBR=0,1", 1000, true);

  sendATCommand("AT+CSCLK=2", 1000, true); // Включение энергосберегающего режима
}

String sendATCommand(const String& command, const int timeout, bool debug) {
  Serial2.println(command);
  String response = "";
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial2.available()) {
      char c = Serial2.read();
      response += c;
      if (debug) Serial.write(c);
    }
  }
  return response;
}