String host = "http://izh.elddir.ru:5001/post";
String sendData = "param1=value1&param2=value2";
String apn = "internet.tele2.ru";
void setup() {
  // Начинаем общение с модулем и монитором порта
  Serial.begin(115200);
  Serial2.begin(115200);
  
  // Инициализация модуля SIM800L
  initModule();
  
  // Установление соединения с интернетом
  connectToInternet("internet.tele2.ru");
  
  // Выполнение POST запроса
  executeHttpPost(host, sendData);
}

void loop() {
  // Здесь можно добавить другой код
  delay(5000);
  executeHttpPost(host, sendData);

}

void initModule() {
  sendATCommand("AT", 1000, true);
  sendATCommand("AT+CFUN=1,1", 10000, true);
}

void connectToInternet(const String& apn) {
  sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 2000, true);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"", 2000, true);
  sendATCommand("AT+SAPBR=1,1", 3000, true);
  sendATCommand("AT+SAPBR=2,1", 3000, true);
}

void connectToTcpServer(const String& server, const String& port) {
  sendATCommand("AT+CIPSHUT", 5000, true); // Завершение предыдущих соединений
  sendATCommand("AT+CIPMUX=0", 2000, true); // Одиночное соединение
  sendATCommand("AT+CSTT=\"your_apn\"", 2000, true); // Установка APN
  sendATCommand("AT+CIICR", 5000, true); // Активизация беспроводного соединения
  sendATCommand("AT+CIFSR", 2000, true); // Получение IP-адреса
  sendATCommand("AT+CIPSTART=\"TCP\",\"" + server + "\"," + port, 10000, true); // Подключение к TCP серверу
}

void sendDataOverTcp(const String& data) {
  sendATCommand("AT+CIPSEND", 2000, true);
  Serial2.print(data);
  delay(100); // Задержка для отправки данных
  Serial2.write(26); // Ctrl+Z для завершения передачи данных
}

void receiveTcpResponse() {
  long int time = millis();
  while ((time + 5000) > millis()) { // Ожидание ответа в течение 5 секунд
    while (Serial2.available()) {
      char c = Serial2.read();
      Serial.write(c);
    }
  }
  sendATCommand("AT+CIPCLOSE", 2000, true); // Закрытие TCP соединения
}

void executeHttpPost(const String& url, const String& data) {
  sendATCommand("AT+HTTPINIT", 2000, true);
  sendATCommand("AT+HTTPPARA=\"CID\",1", 2000, true);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", 2000, true);
  sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"", 2000, true);
  sendATCommand("AT+HTTPDATA=" + String(data.length()) + ",10000", 2000, true);
  
  delay(100); // Задержка для обработки команды
  Serial2.print(data);
  delay(10000); // Задержка для отправки данных

  sendATCommand("AT+HTTPACTION=1", 6000, true);
  sendATCommand("AT+HTTPREAD", 10000, true);
  sendATCommand("AT+HTTPTERM", 2000, true);
}

void sendATCommand(const String& command, const int timeout, bool debug) {
  Serial2.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial2.available()) {
      char c = Serial2.read();
      if (debug) Serial.write(c);
    }
  }
}
