/**
 * =========================
 *         config.cpp
 * =========================
 * 
 * Ce fichier initialise les constantes de configuration :
 * - Informations WiFi (SSID, mot de passe)
 * - Adresse du serveur MQTT et noms des topics
 * - Broche du capteur de présence
 * 
 * Il crée aussi les objets nécessaires pour :
 * - la connexion WiFi (`WiFiClient`)
 * - la communication MQTT (`PubSubClient`)
 * 
 * Et fournit une fonction `reconnect()` pour se (re)connecter au broker MQTT.
 */

#include "config.h"  // Inclusion de l'en-tête associé (déclarations externes)

// =========================
// Initialisation des constantes globales
// =========================
const char* ssid = "Horloge";                          // Nom du réseau WiFi
const char* password = "Horloge_Affichage56";            // Mot de passe du WiFi
const char* mqtt_server = "192.168.1.9";             // Adresse IP du broker MQTT local
const char* topicDiffuseur = "horloge/olfactive/diffuseur"; // Topic MQTT pour contrôler le diffuseur
const char* topicPresence = "horloge/olfactive/presence";   // Topic MQTT pour envoyer l'état de présence
const int sensorPin = 32;                              // Broche utilisée pour le capteur de présence

// =========================
// Objets globaux pour les communications
// =========================
WiFiClient espClient;               // Client WiFi bas niveau
PubSubClient client(espClient);     // Client MQTT utilisant la connexion WiFi

// =========================
// Fonction de reconnexion MQTT
// =========================
void reconnect() {
    // Boucle tant que la connexion n'est pas établie
    while (!client.connected()) {
        Serial.print("Connexion au broker MQTT...");

        // Génère un identifiant client unique
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX); // Ajoute un nombre aléatoire en hexadécimal

        // Tente la connexion au broker
        if (client.connect(clientId.c_str())) {
            Serial.println("Connecté au broker!");
        } else {
            // En cas d'échec, affiche l'erreur et réessaie après 5 secondes
            Serial.print("Échec, erreur = ");
            Serial.print(client.state());
            Serial.println(". Nouvelle tentative dans 5s...");
            delay(5000);
        }
    }
}
