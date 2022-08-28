#include <iostream>
#include <string>

#include <Arduino_JSON.h>

#include <WiFi.h>
#include <WebServer.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
//#include <DHT_U.h>

#define DHTPIN 19     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

const char* ssid     = "ssid"; // 自分のSSIDに書き換える
const char* password = "password"; // 自分のパスワードに書き換える
const int serial_freq = 115200;

WebServer server(80);
String coef_temperature = "0.0";
String coef_humidity = "0.0";

DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


void setup() {

  Serial.begin(serial_freq);
  // Initialize device.
  dht.begin();
  sensor_t sensor;
  delayMS = sensor.min_delay / 1000;


  // シリアルコンソールのセットアップ
  delay(10);
  Serial.println();
  
  // WiFiに接続
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(dht.readTemperature());

  // アクセスされた際に行う関数を登録する
  server.on("/temperature", HTTP_ANY, [](){

    if (server.method() == HTTP_POST) { // POSTメソッドでアクセスされた場合
      coef_temperature = server.arg("plain"); // server.arg("plain")でリクエストボディが取れる。
    }
    JSONVar myArray;  
    myArray["temperature_before"] = dht.readTemperature();
    myArray["temperature_coef"] = coef_temperature.toFloat();
    myArray["temperature_after"] = dht.readTemperature() + coef_temperature.toFloat();
    
    server.send(200, "text/plain", JSON.stringify(myArray)); // 値をクライアントに返す
  });

  server.on("/humidity", HTTP_ANY, [](){
    if (server.method() == HTTP_POST) { // POSTメソッドでアクセスされた場合
      coef_humidity = server.arg("plain"); // server.arg("plain")でリクエストボディが取れる。
    }
    JSONVar myArray;  
    myArray["humidity_before"] = dht.readHumidity();
    myArray["humidity_coef"] = coef_humidity.toFloat();
    myArray["humidity_after"] = dht.readHumidity() + coef_humidity.toFloat();
    server.send(200, "text/plain", JSON.stringify(myArray)); // 値をクライアントに返す
  });
  // 登録されてないパスにアクセスがあった場合
  server.onNotFound([](){
    server.send(404, "text/plain", "Not Found."); // 404を返す
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
