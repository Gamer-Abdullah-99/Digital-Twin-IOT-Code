#include <ESP8266WiFi.h>

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
}
void loop() {
  float h = analogRead(A0);
  if (isnan(h)) {
    Serial.println("Failed to read from MQ-135 sensor!");
    return;
  }

  Serial.print("Gas Level: ");
  Serial.println(h / 1023 * 100);

  delay(500);
  client.stop();
  Serial.println("Waiting...");

  delay(1500);
}
