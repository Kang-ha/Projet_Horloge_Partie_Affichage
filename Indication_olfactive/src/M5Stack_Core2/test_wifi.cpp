/*
 * ================================
 *         test_wifi.cpp
 * ================================
 *
 * Fonction : testWifi()
 * ------------------------
 * Cette fonction teste la connexion WiFi de l'appareil M5Stack.
 *
 * Fonctionnement :
 * 1. Affiche un écran "Test Wifi" avec une zone de log visuelle.
 * 2. Tente de se connecter au réseau WiFi avec les identifiants définis.
 * 3. Affiche un point toutes les 500ms pendant 10 secondes.
 * 4. Si connecté : affiche "Connecté" et l'adresse IP.
 * 5. Sinon : affiche "Échec connexion".
 * 6. Attend 5 secondes avant de sortir.
 */

#include "config.h"  // Accès aux identifiants WiFi (ssid, password)
#include "utils.h"   // Fonction normalizeText()

void testWifi() {
    // 🖥️ Efface l'écran et affiche le titre dans un encadré
    M5.Lcd.fillScreen(BLACK);                                     // Nettoie l'écran
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);            // Encadré titre
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println(normalizeText("Test Wifi"));                   // Affiche "Test Wifi"

    // 🟦 Zone de log visuelle sous le titre
    M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY);           // Encadré de log
    int currentLogY = 70;                                         // Position Y initiale du texte
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);

    // 🔁 Affichage du message de démarrage
    M5.Lcd.setCursor(20, currentLogY);
    M5.Lcd.println(normalizeText("Initialisation Wifi..."));
    currentLogY += 20;

    // 📡 Démarre la tentative de connexion WiFi
    WiFi.begin(ssid, password);
    unsigned long startTime = millis(); // Marque le temps de début

    // ⏳ Attente de la connexion pendant 10 secondes max
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        delay(500);                            // Attente 500ms
        Serial.print(".");                     // Point dans la console
        M5.Lcd.setCursor(20, currentLogY);     
        M5.Lcd.println(normalizeText("."));    // Point sur l'écran
        currentLogY += 20;

        // 🔁 Si la zone de log est remplie, on recommence en haut
        if (currentLogY > 200) {
            M5.Lcd.fillRoundRect(10, 60, 300, 150, 8, DARKGREY); // Efface l'intérieur
            currentLogY = 70;
        }
    }

    // Connexion réussie
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WiFi] Connecté!");
        M5.Lcd.setCursor(20, currentLogY);
        M5.Lcd.println(normalizeText("Connecté!"));
        currentLogY += 20;

        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());  // Affiche l'IP dans le terminal série

        // Affiche l'IP à l'écran
        M5.Lcd.setCursor(20, currentLogY);
        M5.Lcd.printf(normalizeText("IP: %s").c_str(), WiFi.localIP().toString().c_str());
    } 
    // Connexion échouée
    else {
        Serial.println("[WiFi] Echec connexion");
        M5.Lcd.setCursor(20, currentLogY);
        M5.Lcd.println(normalizeText("Echec connexion"));
    }

    delay(5000); // ⏱️ Pause 5 secondes avant la fin du test
}