#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
float temperatureValue, temperatureFValue, humidityValue;  //

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  humidityValue = dht.readHumidity();
  temperatureValue = dht.readTemperature();

  if (isnan(humidityValue) || isnan(temperatureValue)) {
    Serial.println("Failed to read sensor data");
  } else {
    Serial.print("Temperature : ");
    Serial.print(temperatureValue, 0);

    Serial.print("Humidity : ");
    Serial.print(humidityValue, 0);
  }
}
