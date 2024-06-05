void setup() {
  // Инициализация последовательного соединения для вывода данных в монитор порта
  Serial.begin(115200);
  // Инициализация Serial2 для связи с модулем SIM800L
  Serial2.begin(115200);

  // Даем модулю время для инициализации
  delay(1000);

  Serial.println("Initializing module and checking signal strength...");

  // Проверка связи с модулем
  sendCommandToSIM800L("AT");
  delay(1000);

  // Проверка регистрационного статуса сети
  sendCommandToSIM800L("AT+CREG?");
  delay(1000);

  // Проверка уровня сигнала
  sendCommandToSIM800L("AT+CSQ");
  delay(1000);

  // Проверка статуса SIM-карты
  sendCommandToSIM800L("AT+CPIN?");
  delay(1000);
}

void loop() {
  // Если данные поступили от SIM800L
  if (Serial2.available()) {
    String response = Serial2.readString();
    Serial.println(response);
    if (response.startsWith("+CSQ:")) {
      parseSignalStrength(response);
    }
  }
}

void sendCommandToSIM800L(String cmd) {
  Serial2.println(cmd);
  delay(100);
  while (Serial2.available()) {
    String response = Serial2.readString();
    Serial.println(response); // Выводим ответ в монитор порта
  }
}

void parseSignalStrength(String response) {
  int rssi = response.substring(5, response.indexOf(',')).toInt();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.print(" -> ");
  if (rssi == 99) {
    Serial.println("Signal unknown or not detectable");
  } else if (rssi >= 0 && rssi <= 9) {
    Serial.println("Very weak signal");
  } else if (rssi >= 10 && rssi <= 14) {
    Serial.println("Weak signal");
  } else if (rssi >= 15 && rssi <= 19) {
    Serial.println("Average signal");
  } else if (rssi >= 20 && rssi <= 30) {
    Serial.println("Strong signal");
  } else {
    Serial.println("Invalid signal value");
  }
}