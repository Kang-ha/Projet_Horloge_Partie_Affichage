/*
 * =============================
 *       test_presence.cpp
 * =============================
 *
 * Fonction : testPresence()
 * -----------------------------
 * Cette fonction teste l'état du capteur de présence connecté à la broche `sensorPin`.
 *
 * Fonctionnement :
 * 1. Affiche un écran "Test Présence" avec une zone de log visuelle.
 * 2. Configure la broche du capteur en entrée.
 * 3. Pendant 10 secondes, lit l'état du capteur toutes les secondes.
 * 4. Affiche et envoie via MQTT "Présence détectée" ou "Aucune présence".
 * 5. Réinitialise la zone de log si elle est pleine.
 * 6. Termine par une pause de 5 secondes.
 */

#include "config.h"  // Accès au sensorPin, client MQTT et topics
#include "utils.h"   // normalizeText()

void testPresence() {
    // Efface l’écran et affiche un titre dans un encadré
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);  // Encadré titre
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println(normalizeText("Test Présence"));      // Affiche "Test Présence"

    // Création d'une zone de log sous le titre
    M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY); // Fond zone log
    int currentLogY = 70;                                // Position verticale initiale
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);

    // Configure la broche du capteur en entrée
    pinMode(sensorPin, INPUT);

    // Lancement du chronomètre pour 10 secondes
    unsigned long startTime = millis();
    while (millis() - startTime < 10000) {
        M5.update();                         // Met à jour l’état des boutons
        int sensorValue = digitalRead(sensorPin);  // Lit la valeur du capteur

        // Réinitialisation de la zone de log si pleine
        if (currentLogY > 200) {
            M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY);
            currentLogY = 70;
        }

        // Affichage et publication MQTT selon l’état du capteur
        M5.Lcd.setCursor(20, currentLogY);
        if (sensorValue == HIGH) {
            M5.Lcd.println(normalizeText("Présence détectée"));  // Écran
            Serial.println("[Présence] ON");                     // Série
            client.publish(topicPresence, normalizeText("Présence détectée").c_str()); // MQTT
        } else {
            M5.Lcd.println(normalizeText("Aucune présence"));
            Serial.println("[Présence] OFF");
            client.publish(topicPresence, normalizeText("Aucune présence").c_str());
        }

        currentLogY += 20;  // Décale l'affichage de log vers le bas
        delay(1000);        // Pause de 1 seconde entre chaque mesure
    }

    delay(5000);  // Pause de 5 secondes à la fin du test
}