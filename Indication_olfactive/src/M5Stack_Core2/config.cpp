#include "config.h"

// Initialisation des constantes
const char* ssid = "Horloge";
const char* password = "Horloge_Affichage";
const char* mqtt_server = "192.168.1.117";
const char* topicDiffuseur = "horloge/olfactive/diffuseur";
const char* topicPresence = "horloge/olfactive/presence";
const int sensorPin = 32;

// Objets globaux pour la connexion WiFi et MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
    while (!client.connected()) {
        Serial.print("Connexion au broker MQTT...");
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str())) {
            Serial.println("Connecté au broker!");
        } else {
            Serial.print("Échec, erreur = ");
            Serial.print(client.state());
            Serial.println(". Nouvelle tentative dans 5s...");
            delay(5000);
        }
    }
}