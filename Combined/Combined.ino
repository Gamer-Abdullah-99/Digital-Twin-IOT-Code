#include <ESP8266WiFi.h>

#include <DHT.h>

const char *ssid = "LOL";  // replace with your wifi ssid and wpa2 key
const char *pass = "abdullah123";

int pir_sensor = 4;

int DHTPIN = 0;

DHT dht(DHTPIN, DHT11);

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(pir_sensor, INPUT);

  dht.begin();
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {

  int pir = digitalRead(pir_sensor);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float m = analogRead(A0);

  if (isnan(h)) {
    Serial.println("Failed to read Humidity!");
    return;
  }

  else if (isnan(t)) {
    Serial.println("Failed to read Temperature");
    return;
  }

  else if (isnan(m)) {
    Serial.println("Failed to read from MQ-135 sensor!");
    return;
  }
  Serial.print("\n");
  Serial.print("Motion : ");
  Serial.print(pir);

  Serial.print("\n");

  Serial.print("Gas Level: ");
  Serial.println(m);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius \n");
  Serial.print("Humidity: ");
  Serial.print(h);

  Serial.print("\n\n");
  Serial.println("Waiting...");
  delay(15000);
}