#include "config.h"

void testMQTT() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.println("Test MQTT");

    client.setServer(mqtt_server, 1883);

    if (!client.connected()) {
        Serial.println("[MQTT] Connexion au broker...");
        if (client.connect("M5Test")) {
            Serial.println("[MQTT] Connecté!");
            client.publish(topicPresence, "Test MQTT");
        } else {
            Serial.println("[MQTT] Échec connexion");
        }
    }

    delay(5000);
}
