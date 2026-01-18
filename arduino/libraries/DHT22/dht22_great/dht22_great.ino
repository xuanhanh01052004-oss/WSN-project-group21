#include <DHT.h>

#define DHTPIN PA2      // chân DATA nối PA2
#define DHTTYPE DHT22   //  DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Can not read DHT22");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print(" Temparature: ");
  Serial.print(t);
  Serial.println(" C degree");

  delay(2000);
}
