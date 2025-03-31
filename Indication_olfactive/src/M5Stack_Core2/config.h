#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <M5Core2.h>

// Variables Wi-Fi et MQTT
extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const char* topicDiffuseur;
extern const char* topicPresence;
extern const int sensorPin;


extern WiFiClient espClient;
extern PubSubClient client;

void reconnect();

#endif // CONFIG_H
