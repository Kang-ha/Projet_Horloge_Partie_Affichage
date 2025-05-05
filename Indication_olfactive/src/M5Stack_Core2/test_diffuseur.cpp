/*
 * ===============================
 *       test_diffuseur.cpp
 * ===============================
 *
 * Fonction : testDiffuseur()
 * -----------------------------
 * Cette fonction teste le fonctionnement du diffuseur olfactif en se basant
 * sur les détections d’un capteur de présence connecté à la broche 32.
 *
 * Fonctionnement :
 * 1. Affiche un écran "Test Diffuseur" avec une zone de log.
 * 2. Pendant 10 secondes, surveille les changements d'état du capteur.
 * 3. Si une présence est détectée : envoie "ON" via MQTT sur `topicDiffuseur`.
 *    Sinon : envoie "OFF".
 * 4. Affiche à l'écran le statut du diffuseur et journalise en série.
 * 5. Réinitialise la zone de log si elle déborde.
 * 6. Pause de 1 seconde entre chaque lecture, et 5 secondes finales.
 */

#include "config.h"  // Déclarations globales : MQTT, WiFi, pins
#include "utils.h"   // normalizeText()

void testDiffuseur() {
    // Efface l'écran et affiche le titre dans un encadré
    M5.Lcd.fillScreen(BLACK);                              // Vide l'écran
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);     // Encadré du titre
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println(normalizeText("Test Diffuseur"));        // Affiche le titre

    // Zone de log encadrée sous le titre
    M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY);    // Zone de log
    int currentLogY = 70;                                   // Position verticale initiale
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);

    unsigned long startTime = millis();      // Temps de départ
    bool etatPresence = false;               // Dernier état connu du capteur

    // Surveille les changements de présence pendant 10 secondes
    while (millis() - startTime < 10000) {
        M5.update();                         // Met à jour les boutons (précaution)
        bool currentSensorState = (digitalRead(32) == HIGH); // Lecture du capteur

        // Si la zone de log est pleine, on la vide
        if (currentLogY > 200) {
            M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY);
            currentLogY = 70;
        }

        // Détection d'un changement d'état du capteur
        if (currentSensorState != etatPresence) {
            etatPresence = currentSensorState;             // Met à jour l'état
            M5.Lcd.setCursor(20, currentLogY);             // Position du texte

            if (etatPresence) {
                // Présence détectée → Activation du diffuseur
                M5.Lcd.println(normalizeText("Diffuseur ON"));
                Serial.println("[MQTT] Présence détectée -> Diffuseur ON");
                client.publish(topicDiffuseur, normalizeText("ON").c_str());
            } else {
                // Aucune présence → Désactivation du diffuseur
                M5.Lcd.println(normalizeText("Diffuseur OFF"));
                Serial.println("[MQTT] Aucune présence -> Diffuseur OFF");
                client.publish(topicDiffuseur, normalizeText("OFF").c_str());
            }

            currentLogY += 20;  // Décale la ligne suivante
        }

        delay(1000);  // Pause de 1 seconde entre les lectures
    }

    delay(5000);  // Pause finale de 5 secondes à la fin du test
}