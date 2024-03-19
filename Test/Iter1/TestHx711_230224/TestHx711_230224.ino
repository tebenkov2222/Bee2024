#include <GyverHub.h>
#include <EEPROM.h>
#include <DHT.h>
#include "HX711.h"                                            // подключаем библиотеку для работы с тензодатчиком


#define DHTTYPE DHT22
uint8_t DHTPin = 2;
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 0;

//DHTStable DHT;
DHT dht(DHTPin, DHTTYPE);
HX711 scale; 

GyverHub hub("MyDevices", "BeeTest1", "");  // префикс, имя, иконка
bool isAp = false;
bool updateGet = false;
bool updateRead = false;

String massS = "Mass";
float mass;

String massOutVelS = "MassVelocity";
float massOutVel;

String dmassOutS = "DeltaMass";
float dmassOut;


String tempOutS = "Temperature";
float tempOut;

String tempOutVelS = "TemperatureVelocity";
float tempOutVel;

String dtempOutS = "DeltaTemperature";
float dTempOut;


String humOutS = "Humidity";
float humOut;

String humOutVelS = "HumidityVelocity";
float humOutVel;

String dhumOutS = "DeltaHumidity";
float dHumOut;

float aver_pressure;
float mass_array[60];
float temp_array[60];
float hum_array[60];


unsigned long timeToUpdateGet = 60000;
unsigned long currentUpdateGetMillis = 0;

unsigned long timeToOnline = 60000;
unsigned long currentOnlineMillis = 0;


unsigned long timeToCheckDeltaPress = 60000;
unsigned long currentCheckDeltaPressMillis = 0;

void updateGetSensors() {
  String comma = ",";
  String getStr =
    massS + comma + massOutVelS + comma + dmassOutS + comma + tempOutS + comma + tempOutVelS + comma + dtempOutS + comma + humOutS + comma + humOutVelS + comma + dhumOutS + comma;
  hub.sendGet(getStr);
}
void build() {
  hub.Gauge_(massS, mass);
  hub.Gauge_(massOutVelS, massOutVel);
  hub.Gauge_(dmassOutS, dmassOut);

  hub.Gauge_(tempOutS, tempOut);
  hub.Gauge_(tempOutVelS, tempOutVel);
  hub.Gauge_(dtempOutS, dTempOut);

  hub.Gauge_(humOutS, humOut);
  hub.Gauge_(humOutVelS, humOutVel);
  hub.Gauge_(dhumOutS, dHumOut);

  hub.sendGetAuto(true);
}
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  //DHT.read22(DHTPin);
  dht.begin();
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("Keenetic", "Xiaomi_new");
  Serial.println("Setup");
  bool lightWifi;
  //Serial.println("Setup");
  int countWifi = 0;
  while (WiFi.status() != WL_CONNECTED) {
    countWifi++;
    if (countWifi > 6000) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("Keenetic_MeteoAlarm", "Xiaomi_new");
      isAp = true;
      Serial.println("Start is Ap");
      break;
    }
    delay(500);
    lightWifi = !lightWifi;
    digitalWrite(LED_BUILTIN, lightWifi);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Start Wifi");
  }
  digitalWrite(LED_BUILTIN, isAp);

  delay(3000);
  mass = getMassPoints();
  tempOut = getTemp();             // Gets the values of the temperature
  humOut = getHum();      // Gets the values of the humidity
  for (byte i = 0; i < 60; i++) {  // счётчик от 0 до 5
    mass_array[i] = mass;     // забить весь массив текущим давлением
    temp_array[i] = tempOut;       // забить весь массив текущим давлением
    hum_array[i] = humOut;         // забить весь массив текущим давлением
  }

  hub.setupMQTT("192.168.1.64", 1883, "hass", "Hass9128519956@");

  hub.onBuild(build);  // подключаем билдер
  hub.begin();         // запускаем систему

  hub.onEvent([](GHevent_t event, GHconn_t from) {
    if (event == GH_CONNECTED && from == GH_MQTT) {
      updateGetSensors();
    }
  });
  updateRead = true;
}

void UpdateOnline() {
  if (millis() - currentOnlineMillis > timeToOnline) {
    currentOnlineMillis = millis();
    hub.turnOn();
  }
  if (millis() < currentOnlineMillis) {
    currentOnlineMillis = millis();
  }
}
void UpdateGet() {
  if (millis() - currentUpdateGetMillis > timeToUpdateGet || updateGet) {
    currentUpdateGetMillis = millis();
    updateGetSensors();
    updateGet = false;
  }
  if (millis() < currentUpdateGetMillis) {
    currentUpdateGetMillis = millis();
  }
}

void CalculateHumOut() {
  humOut = getHum();         // Gets the values of the temperature
  for (byte i = 0; i < 59; i++) {     // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
    hum_array[i] = hum_array[i + 1];  // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
  }
  hum_array[59] = humOut;  // последний элемент массива теперь - новое давление
  dHumOut = humOut - hum_array[0];
  humOutVel = humOut - hum_array[58];
}
void CalculateTempOut() {
  tempOut = getTemp();                  // Gets the values of the temperature
  for (byte i = 0; i < 59; i++) {       // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
    temp_array[i] = temp_array[i + 1];  // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
  }
  temp_array[59] = tempOut;  // последний элемент массива теперь - новое давление
  dTempOut = tempOut - temp_array[0];
  tempOutVel = tempOut - temp_array[58];
}
void CalculateMass() {
  mass = getMassPoints();                  // Gets the values of the temperature
  for (byte i = 0; i < 59; i++) {       // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
    mass_array[i] = mass_array[i + 1];  // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
  }
  mass_array[59] = mass;  // последний элемент массива теперь - новое давление
  dmassOut = mass - mass_array[0];
  massOutVel = mass - mass_array[58];
}

float getTemp() {
  float t = dht.readTemperature();  // Gets the values of the temperature
  if (t < 0) t = (t + 3276.7) * -1;
  return t;
}
float getHum() {
  return dht.readHumidity();
}
float getMassPoints() {
  return scale.read_medavg(15);
}
void checkDeltaMeteo() {
  if (millis() - currentCheckDeltaPressMillis > timeToCheckDeltaPress || updateRead) {
    //DHT.read22(DHTPin);
    CalculateTempOut();
    CalculateMass();
    CalculateHumOut();
    currentCheckDeltaPressMillis = millis();
    updateRead = false;
  }
  if (millis() < currentCheckDeltaPressMillis) {
    currentCheckDeltaPressMillis = millis();
  }
}

void loop() {
  UpdateGet();
  UpdateOnline();
  checkDeltaMeteo();
  hub.tick();
}
