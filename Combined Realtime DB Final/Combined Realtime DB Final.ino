/**
 * Created by Adesola Samuel
 * 
 * Email:adesolasamuel2018@gmail.c0m
 * 
 * Github: https://github.com/adesolasamuel/ESP8266-NodeMCU-to-Google-Firebase
 * 
 * Copyright (c) 2022 samuel
 *
*/

#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "DHT.h"

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

const long utcOffsetInSeconds = 18000;

//Month names
String months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int pir_sensor = 4;  //Digital pin connected to PIR Motion Sensor

#define DHTPIN 0       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11

/* 1. Define the WiFi credentials */
#define WIFI_SSID "TP-Link_87D6"
#define WIFI_PASSWORD "03203201461"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyB-P-eNc2z8lyAc3qsDlhoqE0fOm0230oc"

/* 3. Define the RTDB URL */
#define DATABASE_URL "node-mcu-42873-default-rtdb.firebaseio.com"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "sabn730@gmail.com"
#define USER_PASSWORD "03203201461"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();
  pinMode(pir_sensor, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  timeClient.begin();

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
}

void loop() {
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();

  int pir = digitalRead(pir_sensor);
  String motion = "hola";

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float mq = analogRead(A0);
  float m = mq / 1023 * 100;

  //Get a time structure
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  String currentMonthName = months[currentMonth - 1];
  int currentYear = ptm->tm_year + 1900;

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

  if (pir == 0) {
    motion = "red";
  }

  else if (pir == 1) {
    motion = "green";
  }

  Serial.print("\n");

  Serial.print("Gas Level: ");
  Serial.println(m);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius \n");
  Serial.print("Humidity: ");
  Serial.print(h);

  Serial.print("\n");
  Serial.print("Time: ");
  Serial.println(timeClient.getFormattedTime());

  //Print complete date:
  Serial.print("Date: ");
  String currentDate = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear);
  Serial.println(currentDate);
  Serial.print("\n");

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    Serial.printf("Sending Data to Firebase");

    // FirebaseJsonArray arr;

    FirebaseJson obj;

    // arr.add(pir);
    // arr.add(m);
    // arr.add(t);
    // arr.add(h);
    // arr.add(timeClient.getFormattedTime().c_str());

    obj.add("motion", motion);
    obj.add("gas", m);
    obj.add("temperature", t);
    obj.add("humidity", h);
    obj.add("time", timeClient.getFormattedTime().c_str());
    obj.add("date", currentDate);

    // Firebase.pushArray(fbdo, F("/G3enl2VXB8Yn71hPLFf04vzFJVZ2/"), arr) ? "ok" : fbdo.errorReason().c_str();

    Firebase.push(fbdo, F("/G3enl2VXB8Yn71hPLFf04vzFJVZ2/"), obj) ? "ok" : fbdo.errorReason().c_str();

    // Firebase.setDouble(fbdo, F("/G3enl2VXB8Yn71hPLFf04vzFJVZ2/motion"), pir) ? "ok" : fbdo.errorReason().c_str();
    // Firebase.setFloat(fbdo, F("/G3enl2VXB8Yn71hPLFf04vzFJVZ2/gas-level"), m) ? "ok" : fbdo.errorReason().c_str();
    // Firebase.setFloat(fbdo, F("/G3enl2VXB8Yn71hPLFf04vzFJVZ2/temperature"), t) ? "ok" : fbdo.errorReason().c_str();
    // Firebase.setDouble(fbdo, F("/G3enl2VXB8Yn71hPLFf04vzFJVZ2/humidity"), h) ? "ok" : fbdo.errorReason().c_str();
  }
  Serial.print("\n\n");
  Serial.println("Waiting...");

  delay(15000);
}