#include <M5Core2.h>

const int sensorPin = 32;  // Broche à laquelle le signal OUT du capteur est connecté

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20, 20);
  M5.Lcd.println("Test Capteur Presence");

  // Configuration de la broche du capteur en entrée
  pinMode(sensorPin, INPUT);


  Serial.println("Initialisation terminee.");
}

void loop() {
  M5.update();

  // Lecture de l'état du capteur
  int sensorValue = digitalRead(sensorPin);

  // Effacer la zone d'affichage pour éviter des résidus
  M5.Lcd.fillRect(20, 60, 200, 20, BLACK);
  M5.Lcd.setCursor(20, 60);

  if (sensorValue == HIGH) {
    M5.Lcd.println("Presence detectee");
    Serial.println("Presence detectee");
  } else {
    M5.Lcd.println("Aucune presence");
    Serial.println("Aucune presence");
  }

  delay(500);
}