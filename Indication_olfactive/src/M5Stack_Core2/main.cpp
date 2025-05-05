/*
 * ============================
 *         main.cpp
 * ============================
 * 
 * Fonction : startDiffuseur()
 * --------------------------------
 * Cette fonction constitue le cœur du système olfactif :
 * - Elle initialise l’affichage sur la M5Stack Core2 (titre + 3 encadrés)
 * - Elle lit l’état du capteur de présence toutes les 500 ms
 * - Elle publie cet état sur MQTT (topicPresence) à chaque changement
 * - Elle attend une présence ou une absence confirmée (10 sec) pour publier ON/OFF sur topicDiffuseur
 * - Elle affiche aussi dynamiquement l’état de la prise (reçu via MQTT)
 */

#include "config.h"   // WiFi, MQTT, sensorPin, topics
#include "main.h"     // startDiffuseur()
#include "utils.h"    // normalizeText()

// Coordonnées des zones à l’écran (titre + 3 encadrés)
const int TITLE_X   = 10, TITLE_Y   = 10,  TITLE_WIDTH   = 300, TITLE_HEIGHT   = 40;
const int DIFF_X    = 10, DIFF_Y    = 60,  DIFF_WIDTH    = 300, DIFF_HEIGHT    = 30;
const int PRES_X    = 10, PRES_Y    = 100, PRES_WIDTH    = 300, PRES_HEIGHT    = 30;
const int PRISE_X   = 10, PRISE_Y   = 140, PRISE_WIDTH   = 300, PRISE_HEIGHT   = 30;

// Variables de logique de présence (confirmation)
unsigned long lastDetectionTime = 0;
unsigned long lastNoDetectionTime = 0;
bool presenceConfirmee = false;
bool absenceConfirmee  = false;
bool etatPresenceActuel = false;

// Variable globale pour l’état MQTT reçu de la prise (ON / OFF)
String priseState = "Inconnue";

// Callback appelé lorsqu’un message MQTT est reçu
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i]; // Reconstitue le message reçu
  }
  // Si le message concerne le topic du diffuseur, on met à jour la variable globale
  if (String(topic) == String(topicDiffuseur)) {
    priseState = msg;
  }
}

// Fonction principale appelée après la temporisation ou automatiquement
void startDiffuseur() {
    // --- Titre ---
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.fillRoundRect(TITLE_X, TITLE_Y, TITLE_WIDTH, TITLE_HEIGHT, 8, DARKGREY);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    int titleTextY = TITLE_Y + (TITLE_HEIGHT / 2) - 4;
    M5.Lcd.setCursor(TITLE_X + 10, titleTextY);
    M5.Lcd.println(normalizeText("Système principal"));

    // --- Encadré Diffuseur (fixe sur OFF) ---
    M5.Lcd.fillRoundRect(DIFF_X, DIFF_Y, DIFF_WIDTH, DIFF_HEIGHT, 8, DARKGREY);
    int diffTextY = DIFF_Y + (DIFF_HEIGHT / 2) - 4;
    M5.Lcd.setCursor(DIFF_X + 10, diffTextY);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println(normalizeText("Diffuseur : OFF"));

    // --- Encadré Présence ---
    M5.Lcd.fillRoundRect(PRES_X, PRES_Y, PRES_WIDTH, PRES_HEIGHT, 8, DARKGREY);
    int presTextY = PRES_Y + (PRES_HEIGHT / 2) - 4;
    M5.Lcd.setCursor(PRES_X + 10, presTextY);
    M5.Lcd.println(normalizeText("Présence : ---"));

    // --- Encadré Prise ---
    M5.Lcd.fillRoundRect(PRISE_X, PRISE_Y, PRISE_WIDTH, PRISE_HEIGHT, 8, DARKGREY);
    int priseTextY = PRISE_Y + (PRISE_HEIGHT / 2) - 4;
    M5.Lcd.setCursor(PRISE_X + 10, priseTextY);
    M5.Lcd.println(normalizeText("Prise : Inconnue"));

    // 🌐 Connexion WiFi + configuration MQTT
    WiFi.begin(ssid, password);
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);

    // 🔁 Boucle jusqu’à connexion au broker MQTT
    while (!client.connected()) {
        reconnect();
        client.loop();
        delay(500);
    }

    // Abonnement au topic du diffuseur
    client.subscribe(topicDiffuseur);

    // Configuration du capteur de présence
    pinMode(sensorPin, INPUT);

    // Boucle principale continue
    while (true) {
        M5.update();

        // Vérifie connexion MQTT
        if (!client.connected()) {
            reconnect();
        }
        client.loop(); // Traitement des messages entrants

        // Lecture de la présence
        bool currentSensorState = (digitalRead(sensorPin) == HIGH);
        unsigned long currentTime = millis();

        // 📤 Publication si changement d’état
        if (currentSensorState != etatPresenceActuel) {
            etatPresenceActuel = currentSensorState;
            if (currentSensorState) {
                Serial.println(normalizeText("Présence détectée"));
                client.publish(topicPresence, normalizeText("Présence détectée").c_str());
            } else {
                Serial.println(normalizeText("Aucune présence"));
                client.publish(topicPresence, normalizeText("Aucune présence").c_str());
            }
        }

        // ⏱️ Détection confirmée (présence stable depuis 10s)
        if (currentSensorState) { 
            if (!presenceConfirmee) { 
                if (lastDetectionTime == 0) { 
                    lastDetectionTime = currentTime;
                } else if (currentTime - lastDetectionTime > 10000) {
                    Serial.println(normalizeText("Présence confirmée"));
                    client.publish(topicDiffuseur, normalizeText("ON").c_str());
                    presenceConfirmee = true;
                    absenceConfirmee = false;
                    lastDetectionTime = 0;
                }
            }
            lastNoDetectionTime = 0;
        } else { 
            if (!absenceConfirmee) {
                if (lastNoDetectionTime == 0) { 
                    lastNoDetectionTime = currentTime;
                } else if (currentTime - lastNoDetectionTime > 10000) {
                    Serial.println(normalizeText("Absence confirmée"));
                    client.publish(topicDiffuseur, normalizeText("OFF").c_str());
                    absenceConfirmee = true;
                    presenceConfirmee = false;
                    lastNoDetectionTime = 0;
                }
            }
            lastDetectionTime = 0;
        }

        // --- Mise à jour de l’encadré Présence ---
        M5.Lcd.fillRoundRect(PRES_X, PRES_Y, PRES_WIDTH, PRES_HEIGHT, 8, DARKGREY);
        presTextY = PRES_Y + (PRES_HEIGHT / 2) - 4;
        M5.Lcd.setCursor(PRES_X + 10, presTextY);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(2);
        if (currentSensorState) {
            M5.Lcd.println(normalizeText("Présence : Détectée"));
        } else {
            M5.Lcd.println(normalizeText("Présence : Aucune"));
        }

        // --- Mise à jour de l’encadré Prise ---
        M5.Lcd.fillRoundRect(PRISE_X, PRISE_Y, PRISE_WIDTH, PRISE_HEIGHT, 8, DARKGREY);
        priseTextY = PRISE_Y + (PRISE_HEIGHT / 2) - 4;
        M5.Lcd.setCursor(PRISE_X + 10, priseTextY);
        M5.Lcd.println(normalizeText(("Prise : " + priseState).c_str()));

        delay(500); // Attente entre chaque cycle
    }
}