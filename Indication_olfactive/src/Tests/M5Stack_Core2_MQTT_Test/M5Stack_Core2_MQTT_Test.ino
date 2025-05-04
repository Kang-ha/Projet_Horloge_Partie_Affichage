#include <M5Core2.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ------------------------------
// Configuration WiFi
// ------------------------------
const char* ssid     = "Horloge";
const char* password = "Horloge_Affichage";

// ------------------------------
// Configuration MQTT
// ------------------------------
const char* mqtt_server = "test.mosquitto.org"; 
// Ou l'adresse IP / domaine de votre broker
// Ex. : "192.168.0.10" ou "mqtt.example.com"

WiFiClient espClient;
PubSubClient client(espClient);

// Broche du capteur de présence
const int sensorPin = 32;

void setup_wifi() {
  // Connexion au réseau WiFi
  Serial.print("Connexion au WiFi : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // On attend la connexion pendant 10s
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connecte au WiFi !");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Echec de connexion au WiFi");
  }
}

// ------------------------------
// Fonction de reconnexion au broker MQTT
// ------------------------------
void reconnect() {
  // Boucle jusqu'à ce qu'on soit connectés
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    // clientId unique
    String clientId = "M5Core2-";
    clientId += String(random(0xffff), HEX);

    // Tentative de connexion
    if (client.connect(clientId.c_str())) {
      Serial.println("Connecte au broker!");
      // Publication d'un message de bienvenue
      client.publish("horloge/olfactive/presence", "M5Core2 Connecte!");
    } else {
      Serial.print("Echec, erreur = ");
      Serial.print(client.state());
      Serial.println(". Nouvelle tentative dans 5s...");
      delay(5000);
    }
  }
}

void setup() {
  // Initialisation
  M5.begin();
  Serial.begin(115200);

  // Initialisation de l'écran
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20, 20);
  M5.Lcd.println("MQTT Presence Test");

  // Configuration de la broche du capteur en entree
  pinMode(sensorPin, INPUT);

  // Connexion WiFi
  setup_wifi();

  // Configuration du broker MQTT
  client.setServer(mqtt_server, 1883);
}

void loop() {
  M5.update();

  // Verification connexion au broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Traitement des messages MQTT (subscription, ping, etc.)

  // Lecture du capteur
  int sensorValue = digitalRead(sensorPin);

  // Efface une zone pour ne pas superposer le texte
  M5.Lcd.fillRect(20, 60, 200, 20, BLACK);
  M5.Lcd.setCursor(20, 60);

  if (sensorValue == HIGH) {
    M5.Lcd.println("Presence detectee");
    Serial.println("Presence detectee");

    // Publier un message sur le topic
    client.publish("horloge/olfactive/presence", "Presence detectee");
    
    // Petit delai pour eviter de spammer
    delay(2000);
  } else {
    M5.Lcd.println("Aucune presence");
    Serial.println("Aucune presence");
    delay(500);
  }
}
