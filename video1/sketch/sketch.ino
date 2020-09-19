
#include <DHT.h> // подключение библиотек
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define DHTPIN 5 // пин подключния

unsigned long timing; // переменная задержки в милисекндах

DHT dht(DHTPIN, DHT22); // DHT22 или DHT11, зависит от вашего датчика

const char *ssid = ""; // Имя вайфай точки доступа
const char *pass = ""; // Пароль от точки доступа

const char *mqtt_server = ""; // Имя сервера MQTT
const int mqtt_port = 5631; // Порт для подключения к серверу MQTT
const char *mqtt_user = ""; // Логин от сервер
const char *mqtt_pass = ""; // Пароль от сервера
int otkat = 1000;

WiFiClient wclient;
PubSubClient client(wclient, mqtt_server, mqtt_port);

void setup() {
  dht.begin(); // иницализация
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  pinMode(DHTPIN, INPUT);
}

void loop() {
  try_connect(); // функция подключения
  if (client.connected() ){ // проверка подключения
    client.loop(); 
    send_data(); // отправка данных
  }
}


void send_data(){
  if (millis() - timing > otkat){ // проверка задержки
    float t = dht.readTemperature(); // считывает с датчика
    float h = dht.readHumidity();    
    client.publish("test/temp",String(String(t) + " " + String(h))); // отправка с датчика на mqtt cервер топик - test/temp
    Serial.println(t); // вывод показаний датчика
    Serial.println(h);
    timing = millis(); // обнуление задержки
  }
      
}


void try_connect(){ // функция проверки подключения 
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  //return;
  Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server");
      if (client.connect(MQTT::Connect("arduinoClient2")
      .set_auth(mqtt_user, mqtt_pass))) {
        Serial.println("Connected to MQTT server");
      } else {
        Serial.println("Could not connect to MQTT server");
      }
    }
  }
}
