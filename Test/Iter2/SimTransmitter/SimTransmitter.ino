void setup() {
  // Инициализация последовательного соединения для вывода данных в монитор порта
  Serial.begin(115200);
  // Инициализация Serial2 для связи с модулем SIM800L
  Serial2.begin(115200);

  // Даем модулю время для инициализации
  delay(1000);

  Serial.println("Enter AT commands:");
}

void loop() {
  // Если данные поступили с последовательного монитора
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    sendCommandToSIM800L(command);
  }

  // Если данные поступили от SIM800L
  if (Serial2.available()) {
    String response = Serial2.readString();
    Serial.println(response);
  }
}

void sendCommandToSIM800L(String cmd) {
  Serial2.println(cmd);
  delay(100);
}