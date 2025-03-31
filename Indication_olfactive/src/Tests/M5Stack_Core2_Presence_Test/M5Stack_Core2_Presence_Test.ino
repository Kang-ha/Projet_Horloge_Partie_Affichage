#include <M5Core2.h>

const int sensorPin = 32;

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20, 20);
  M5.Lcd.println("Test Capteur Presence");

  pinMode(sensorPin, INPUT);

  Serial.println("[INFO] Initialisation terminee.");
}

void loop() {
  M5.update();

  int sensorValue = digitalRead(sensorPin);
  Serial.print("[DEBUG] sensorValue = ");
  Serial.println(sensorValue);

  M5.Lcd.fillRect(20, 60, 200, 20, BLACK);
  M5.Lcd.setCursor(20, 60);

  if (sensorValue == HIGH) {
    M5.Lcd.println("Presence detectee");
    Serial.println("[LOG] Presence detectee");
  } else {
    M5.Lcd.println("Aucune presence");
    Serial.println("[LOG] Aucune presence");
  }

  delay(500);
}