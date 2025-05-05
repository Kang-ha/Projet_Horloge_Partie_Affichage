/*
 * ===========================
 *        test_mqtt.cpp
 * ===========================
 *
 * Fonction : testMQTT()
 * ------------------------
 * Cette fonction vérifie si la carte est capable de :
 * - se connecter au broker MQTT (via PubSubClient)
 * - publier un message de test sur le topic de présence
 *
 * Fonctionnement :
 * 1. Affiche "Test MQTT" sur l'écran avec une zone de log
 * 2. Si le client MQTT n'est pas déjà connecté, il tente de se connecter
 * 3. Affiche le résultat (succès ou échec)
 * 4. Si la connexion est un succès, envoie un message MQTT sur `topicPresence`
 * 5. Attend 5 secondes avant de quitter le test
 */

#include "config.h"  // Connexion MQTT, WiFi, topics, etc.
#include "utils.h"   // Fonction normalizeText()

void testMQTT() {
    // Efface l'écran et affiche un encadré avec le titre
    M5.Lcd.fillScreen(BLACK);                               // Nettoie l'affichage
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);      // Encadré du titre
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println(normalizeText("Test MQTT"));              // Affiche "Test MQTT"

    // Zone de log encadrée
    M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY);     // Fond zone log
    int currentLogY = 70;                                    // Position verticale de log
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);

    // Si le client MQTT n'est pas connecté
    if (!client.connected()) {
        // Affiche le message de tentative
        M5.Lcd.setCursor(20, currentLogY);
        M5.Lcd.println(normalizeText("Connexion au broker..."));
        currentLogY += 20;
        Serial.println("[MQTT] Connexion au broker...");

        // Tentative de connexion avec l'ID client "M5Test"
        if (client.connect("M5Test")) {
            // Connexion réussie
            M5.Lcd.setCursor(20, currentLogY);
            M5.Lcd.println(normalizeText("Connecté!"));
            currentLogY += 20;
            Serial.println("[MQTT] Connecté!");

            // Publication d'un message test sur le topic "presence"
            client.publish(topicPresence, normalizeText("Test MQTT").c_str());
        } else {
            // Échec de connexion
            M5.Lcd.setCursor(20, currentLogY);
            M5.Lcd.println(normalizeText("Echec connexion"));
            currentLogY += 20;
            Serial.println("[MQTT] Echec connexion");
        }
    }

    delay(5000);  // Pause de 5 secondes pour laisser le temps de lire l’affichage
}