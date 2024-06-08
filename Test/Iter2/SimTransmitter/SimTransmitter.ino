static String serverUrl = F("http://rest-api.bee.elddir.ru");
static String apn = F("internet.beeline.ru");
static String user = F("beeline");
static String pass = F("beeline");
bool useSsl;

// Прототипы функций
void initModule();
void connectToInternet();
String getImei();
String getPassword(const String& url);
void transmitData(const String& url, const String& login, const String& password, const String& data);
void enterPowerSavingMode();
String sendATCommand(const String& command, const int timeout, bool debug);
String savedImai;

const char enter = 13;
const int chipSelect = 4;

int signal;
int response_pass;
int response_transmit;
String inputLine;

bool isStarted = false;
bool isDateNow = true;
String dateNow;
String data;

void setup() {
  pinMode(PB3, OUTPUT);
  digitalWrite(PB3, LOW);
  delay(500);
  digitalWrite(PB3, HIGH);

  // Инициализация последовательного соединения для вывода данных в монитор порта
  Serial3.begin(9600);
  Serial2.begin(9600);
  // Инициализация Serial2 для связи с модулем SIM800L
  Serial1.begin(9600);
  
  // Даем модулю время для инициализации
  delay(1000);

  Serial3.println("Enter AT commands:");

  savedImai.reserve(16);
  initModule();
  delay(1000);
  Serial3.println("checkPassAndImai");

  checkImai();
}

void loop() {
  // Если данные поступили с последовательного монитора
  if (Serial3.available()) {
    String command = Serial3.readStringUntil('\n');
    sendCommandToSIM800L(command);
  }

  if (Serial2.available()) {
    char input = Serial2.read();
    if(input == enter){
      if(isStarted){
        if(isDateNow){
          dateNow = String(inputLine);
        }
        else{
          data = String(inputLine);
          isStarted = false;
          sendData(data);
          sendResultToBase();
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
  // Если данные поступили от SIM800L
  if (Serial1.available()) {
    String response = Serial1.readString();
    Serial3.println(response);
  }
}

void sendResultToBase(){
  Serial3.println("Send result to base");
  Serial3.println(signal);
  Serial3.println(response_pass);
  Serial3.println(response_transmit);
  Serial3.println("/Send result to base");

  Serial2.println("!");
  Serial2.println(signal);
  Serial2.println(response_pass);
  Serial2.println(response_transmit);
}

int getSignalStrength() {
  String response = sendATCommand("AT+CSQ", 1000, true);
  
  // Пример ответа: "+CSQ: 15,0"
  int index = response.indexOf("+CSQ:");
  if (index != -1) {
    String values = response.substring(index + 6);
    int commaIndex = values.indexOf(",");
    if (commaIndex != -1) {
      String rssi = values.substring(0, commaIndex);
      rssi.trim(); // Убираем пробелы
      return rssi.toInt();
    }
  }
  return -1; // Возвращаем -1 если не удалось получить уровень сигнала
}

int getHttpStatusCode(const String& response) {
  int index = response.indexOf("+HTTPACTION:");
  if (index != -1) {
    String values = response.substring(index + 12);
    values.trim(); // Убираем лишние пробелы

    // Разбиваем строку на части
    int firstCommaIndex = values.indexOf(',');
    int secondCommaIndex = values.indexOf(',', firstCommaIndex + 1);

    if (firstCommaIndex != -1 && secondCommaIndex != -1) {
      String statusCode = values.substring(firstCommaIndex + 1, secondCommaIndex);
      statusCode.trim(); // Убираем пробелы
      return statusCode.toInt();
    }
  }
  return -1;
}

void sendCommandToSIM800L(String cmd) {
  Serial1.println(cmd);
  //Serial3.println(cmd);
  //delay(100);
}

void checkImai(){
  savedImai = getImei();
  Serial3.print("Imai = '");
  Serial3.print(savedImai);
  Serial3.println("'");
}

void checkPass(){
  connectToInternet();
  String password = getPassword(serverUrl + "/getpass");
  Serial3.println();
  Serial3.print("Pass = ");
  Serial3.println(password);
  enterPowerSavingMode();
}


void sendData(String data){
  connectToInternet();
  String password = getPassword(serverUrl + "/getpass");
  Serial3.println(password);
  transmitData(serverUrl + "/transmitdata", savedImai, password, data);
  enterPowerSavingMode();
}

void initModule() {
  String response;
  sendATCommand("AT", 1000, true);
  sendATCommand("AT+CFUN=1,1", 10000, true);
  
  Serial3.println("Wait  Sms Ready");

  // Ожидание строки "SMS Ready"
  while (true) {
    Serial3.println("Send AT");
    signal = getSignalStrength();

    if (signal > 3) {
      Serial3.println("Signal norm");
      break;
    }
    delay(2000);
  }
}

void connectToInternet() {
  signal = getSignalStrength();
  sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 2000, true);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"", 2000, true);
  sendATCommand("AT+SAPBR=3,1,\"USER\",\"" + user + "\"", 2000, true);
  sendATCommand("AT+SAPBR=3,1,\"PWD\",\"" + pass + "\"", 2000, true);
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
  String response = sendATCommand("AT+HTTPACTION=0", 6000, true); // GET запрос
  response_pass = getHttpStatusCode(response);
  response = sendATCommand("AT+HTTPREAD", 10000, true);

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
  result = result.substring(0,16);
  result.trim();
  return result;
}

void transmitData(const String& url, const String& login, const String& password, const String& data) {
  sendATCommand("AT+HTTPINIT", 2000, true);
  sendATCommand("AT+HTTPPARA=\"CID\",1", 2000, true);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", 2000, true);
  sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"", 2000, true);

  String postData = "login=" + login + "&password=" + password + "&data=" + data;
  Serial3.print("postData = ");
  Serial3.println(postData);
  sendATCommand("AT+HTTPDATA=" + String(postData.length()) + ",10000", 2000, true);
  
  delay(100); // Задержка для обработки команды
  Serial1.print(postData);
  delay(1000); // Задержка для обработки команды

  String response = sendATCommand("AT+HTTPACTION=1", 10000, true); // POST запрос
  response_transmit = getHttpStatusCode(response);

  sendATCommand("AT+HTTPREAD", 10000, true);
  sendATCommand("AT+HTTPTERM", 2000, true);
}

void enterPowerSavingMode() {
  sendATCommand("AT+SAPBR=0,1", 1000, true);

  sendATCommand("AT+CSCLK=2", 1000, true); // Включение энергосберегающего режима
}

String sendATCommand(const String& command, const int timeout, bool debug) {
  Serial1.println(command);
  String response = "";
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      char c = Serial1.read();
      response += c;
      if (debug) Serial3.write(c);
    }
  }
  return response;
}