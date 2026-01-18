#include <Arduino.h>
#include<mylib.h>
#include<WiFi.h>
#include<WebServer.h>

const char* ssid = "IPhone cua Hanh";
const char* password = "01052004";
WebServer server(80);
void viewer(){
  
}




void setup() {
  Serial.begin(9600);//config toc do UART de co serial moritor de nhin ra log
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.printf("..");
    delay(100);
  }
  Serial.println("Connect ...good");
  Serial.print(WiFi.localIP());
server.on("/hello", viewer);



}

void loop() {
  
}
