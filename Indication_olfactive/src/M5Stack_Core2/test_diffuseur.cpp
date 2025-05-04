#include "config.h"

void testDiffuseur() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.println("Test Diffuseur");

    unsigned long startTime = millis();
    bool etatPresence = false;

    while (millis() - startTime < 10000) {
        M5.update();
        bool currentSensorState = digitalRead(32) == HIGH;

        if (currentSensorState != etatPresence) {
            etatPresence = currentSensorState;
            if (etatPresence) {
                Serial.println("[MQTT] Présence détectée -> Diffuseur ON");
                client.publish(topicDiffuseur, "ON");
                M5.Lcd.fillRect(10, 50, 300, 30, BLACK);
                M5.Lcd.setCursor(10, 50);
                M5.Lcd.println("Diffuseur ON");
            } else {
                Serial.println("[MQTT] Aucune présence -> Diffuseur OFF");
                client.publish(topicDiffuseur, "OFF");
                M5.Lcd.fillRect(10, 50, 300, 30, BLACK);
                M5.Lcd.setCursor(10, 50);
                M5.Lcd.println("Diffuseur OFF");
            }
        }
        delay(1000);
    }
}
