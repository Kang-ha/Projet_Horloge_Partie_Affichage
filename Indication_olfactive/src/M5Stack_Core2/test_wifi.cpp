#include "config.h"

void testWifi() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.println("Test Wifi");

    WiFi.begin(ssid, password);
    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WiFi] Connecté!");
        M5.Lcd.setCursor(10, 60);
        M5.Lcd.println("Connecté!");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("[WiFi] Échec connexion");
        M5.Lcd.setCursor(10, 60);
        M5.Lcd.println("Échec connexion");
    }
    delay(5000);
}
