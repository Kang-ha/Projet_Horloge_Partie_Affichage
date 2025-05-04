#include "config.h"

void testPresence() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.println("Test Présence");

       pinMode(sensorPin, INPUT);

    unsigned long startTime = millis();
    while (millis() - startTime < 10000) {
        M5.update();
        int sensorValue = digitalRead(sensorPin); 

        M5.Lcd.fillRect(10, 50, 300, 30, BLACK);
        M5.Lcd.setCursor(10, 50);
        if (sensorValue == HIGH) {
            M5.Lcd.println("Présence détectée");
            Serial.println("[Présence] ON");
            client.publish(topicPresence, "Présence détectée");
        } else {
            M5.Lcd.println("Aucune présence");
            Serial.println("[Présence] OFF");
            client.publish(topicPresence, "Aucune présence");
        }
        delay(1000);
    }
}



