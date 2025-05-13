#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// WiFi
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

// Firebase
#define DATABASE_URL "YourFirebase_URL"
#define API_KEY "YOUR_API_KEY_FIREBASE"
//AIzaSyBgwbuDz11vzbZMUszikrNJ5jWwhGgjoaQ
// k086y7Ot5ndZVzJSU1o4S8q4UzJ8n2tNVXehFSTa

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// DHT
#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastMsg = 0;


void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Inisialisasi Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Anonymous sign-in (aktifkan di Firebase Console)
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signUp success");
  } else {
    Serial.printf("Firebase signUp failed: %s\n", config.signer.signupError.message.c_str());
  }
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && (millis() - lastMsg > 5000 || lastMsg == 0)) {
    lastMsg = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.printf("Temperature: %.2f\n", t);
    Serial.printf("Humidity: %.2f\n", h);

    // Kirim ke Firebase
    Firebase.RTDB.setFloat(&fbdo, "/esp32/temp", t);
    Firebase.RTDB.setFloat(&fbdo, "/esp32/humi", h);
  }
}
