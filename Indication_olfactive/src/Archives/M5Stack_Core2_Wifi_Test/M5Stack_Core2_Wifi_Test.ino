#include <M5Core2.h>
#include <WiFi.h>

// Renseignez votre SSID et mot de passe Wi-Fi ici :
const char* ssid     = "Horloge";
const char* password = "Horloge_Affichage";

void setup() {
  // Initialisation de la M5Stack Core2
  M5.begin();
  Serial.begin(115200);

  // Effacer et configurer l'écran
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20, 20);

  // Affiche un message de démarrage
  M5.Lcd.println("WiFi Test - M5Stack Core2");

  // Démarre la connexion Wi-Fi
  Serial.println();
  Serial.print("Connexion au reseau Wi-Fi : ");
  Serial.println(ssid);

  M5.Lcd.setCursor(20, 60);
  M5.Lcd.printf("Connexion a %s...", ssid);

  WiFi.begin(ssid, password);

  // Boucle jusqu'a obtention d'une connexion ou timeout
  unsigned long startAttemptTime = millis();

  // Durée max : 10 secondes (10000 ms)
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Vérification de l'état de connexion
  M5.Lcd.setCursor(20, 100);
  if (WiFi.status() == WL_CONNECTED) {
    // Connexion réussie
    Serial.println("Connecte au WiFi !");
    M5.Lcd.println("Connecte au WiFi !");

    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());

    M5.Lcd.setCursor(20, 140);
    M5.Lcd.print("IP : ");
    M5.Lcd.println(WiFi.localIP());
  } else {
    // Echec de connexion
    Serial.println("Echec de connexion au WiFi");
    M5.Lcd.println("Echec de connexion WiFi");
  }
}

void loop() {
  // M5.update() pour la gestion des boutons, ecran tactile, etc.
  M5.update();
  
  // Vous pouvez ajouter du code ici si necessaire
  delay(1000);
}