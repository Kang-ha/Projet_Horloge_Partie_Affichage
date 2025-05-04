#include "config.h"
#include "main.h"

unsigned long lastDetectionTime = 0;
unsigned long lastNoDetectionTime = 0;
bool presenceConfirmee = false;
bool absenceConfirmee = false;
bool etatPresenceActuel = false; 

void startDiffuseur() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println("Démarrage du diffuseur...");

    WiFi.begin(ssid, password);
    client.setServer(mqtt_server, 1883);

    pinMode(sensorPin, INPUT);

    while (true) {
        M5.update();

        if (!client.connected()) {
            reconnect();
        }
        client.loop();

        bool currentSensorState = digitalRead(sensorPin) == HIGH;
        unsigned long currentTime = millis();

        if (currentSensorState != etatPresenceActuel) {
            etatPresenceActuel = currentSensorState;
            if (currentSensorState) {
                Serial.println("Présence détectée");
                client.publish(topicPresence, "Présence détectée");
            } else {
                Serial.println("Aucune présence");
                client.publish(topicPresence, "Aucune présence");
            }
        }

        if (currentSensorState) { 
            if (!presenceConfirmee) { 
                if (lastDetectionTime == 0) { 
                    lastDetectionTime = currentTime;
                } 
                else if (currentTime - lastDetectionTime > 10000) {
                    Serial.println("Présence confirmée -> ON");
                    client.publish(topicDiffuseur, "ON");
                    M5.Lcd.fillRect(20, 60, 200, 20, BLACK);
                    M5.Lcd.setCursor(20, 60);
                    M5.Lcd.println("Diffuseur ON");
                    presenceConfirmee = true;
                    absenceConfirmee = false;
                    lastDetectionTime = 0;
                }
            }
            lastNoDetectionTime = 0;
        } 
        else { 
            if (!absenceConfirmee) {
                if (lastNoDetectionTime == 0) { 
                    lastNoDetectionTime = currentTime;
                } 
                else if (currentTime - lastNoDetectionTime > 10000) {
                    Serial.println("Absence confirmée -> OFF");
                    client.publish(topicDiffuseur, "OFF");
                    M5.Lcd.fillRect(20, 60, 200, 20, BLACK);
                    M5.Lcd.setCursor(20, 60);
                    M5.Lcd.println("Diffuseur OFF");
                    absenceConfirmee = true;
                    presenceConfirmee = false;
                    lastNoDetectionTime = 0;
                }
            }
            lastDetectionTime = 0;
        }

        delay(500);
    }
}
