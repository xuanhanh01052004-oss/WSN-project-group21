#include <Arduino.h>
#include<WiFi.h> // thu vien wifi de connect duoc wifi
#include<HTTPClient.h>// thu vien de config la client
const char* ssid = "Iphone của Hạnh";
const char* password = "01052004";
const char* url = "http://10.136.192.207:3000";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print("...");
    delay(100);
  }
Serial.println("Connected...good");
  
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    //connnect thanh cong va tao HTTp
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    // ban than voi moi server, ban than esp32 se sinh ra 1 local IP va duoc coi la url
    // lay url server o dau
    // bat dau di khoi tao server
    //call API toi server va nhan du lieu tu server nha ve
    String jsonData = "{\"tem\":\"20\"}";
    int res = http.POST(jsonData);
    if(res>0 ){
      Serial.println("Eceed");
      Serial.println(http.getString());
    }else{
      Serial.println("Fail");
    }
    http.end();
  }
  delay(5000);
}

