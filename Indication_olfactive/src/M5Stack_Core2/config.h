/**
 * =========================
 *         config.h
 * =========================
 * 
 * Ce fichier d'en-tête contient :
 * - les déclarations des constantes Wi-Fi, MQTT et matériel (capteur)
 * - les déclarations des objets globaux pour les connexions
 * - la fonction de reconnexion MQTT (`reconnect`)
 * 
 * Il permet au reste du programme d’accéder à ces éléments externes
 * définis dans `config.cpp`.
 */

#ifndef CONFIG_H
#define CONFIG_H  // Empêche les inclusions multiples

#include <WiFi.h>         // Bibliothèque Wi-Fi pour ESP32
#include <PubSubClient.h> // Client MQTT
#include <M5Core2.h>      // Librairie M5Stack Core2

// =========================
// Variables Wi-Fi et MQTT (déclarées comme externes)
// =========================
extern const char* ssid;             // Nom du réseau WiFi
extern const char* password;         // Mot de passe du réseau
extern const char* mqtt_server;      // Adresse du broker MQTT
extern const char* topicDiffuseur;   // Topic MQTT pour piloter le diffuseur
extern const char* topicPresence;    // Topic MQTT pour envoyer la présence
extern const int sensorPin;          // Broche utilisée pour le capteur de présence

// =========================
// Objets de connexion globaux
// =========================
extern WiFiClient espClient;         // Objet Wi-Fi client
extern PubSubClient client;          // Objet client MQTT

// =========================
// Fonction de reconnexion au broker
// =========================
void reconnect();                    // Fonction pour se reconnecter au broker MQTT

#endif // CONFIG_H
