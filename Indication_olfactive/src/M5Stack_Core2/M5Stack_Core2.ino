#include "config.h"
#include "test_wifi.h"
#include "test_mqtt.h"
#include "test_presence.h"
#include "test_diffuseur.h"
#include "main.h"

// États de navigation
enum AppState { ACCUEIL, MENU_PRINCIPAL, MENU_TESTS, PARAMETRES, TEST_EN_COURS };
AppState currentState = ACCUEIL;

// Contenus des menus
const int numOptionsMain = 2;
const int numOptionsTest = 4;
int selectedOption = 0;
const char* menuMain[numOptionsMain] = { "Test", "Parametres" };
const char* menuTests[numOptionsTest + 1] = { "WiFi", "MQTT", "Presence", "Diffuseur", "Retour" };

// ✅ **Fonction de normalisation des caractères spéciaux**
String normalizeText(const char* text) {
    String normalized = String(text);
    normalized.replace("é", "e");
    normalized.replace("è", "e");
    normalized.replace("ê", "e");
    normalized.replace("ë", "e");
    normalized.replace("à", "a");
    normalized.replace("â", "a");
    normalized.replace("ä", "a");
    normalized.replace("ô", "o");
    normalized.replace("ö", "o");
    normalized.replace("î", "i");
    normalized.replace("ï", "i");
    normalized.replace("û", "u");
    normalized.replace("ü", "u");
    normalized.replace("ç", "c");
    return normalized;
}

// ✅ **Affichage amélioré d’un élément unique**
void drawSingleOption(const char* title, const char* option) {
    M5.Lcd.fillScreen(BLACK);
    
    // 🎨 **Titre encadré**
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println(normalizeText(title));

    // 🎨 **Option encadrée au centre**
    M5.Lcd.fillRoundRect(60, 100, 200, 50, 8, YELLOW);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(80, 115);
    M5.Lcd.println(normalizeText(option));
}

// ✅ **Affichage amélioré des paramètres**
void showParameters() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    
    // 🎨 **Titre encadré**
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println("Parametres");

    // 🎨 **Affichage des valeurs**
    M5.Lcd.setCursor(20, 60);
    M5.Lcd.printf("IP : %s", WiFi.localIP().toString().c_str());

    M5.Lcd.setCursor(20, 90);
    M5.Lcd.printf("Broche : %d", sensorPin);

    float batteryVoltage = M5.Axp.GetBatVoltage();
    float batteryPercent = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100.0;
    if (batteryPercent > 100.0) batteryPercent = 100.0;
    if (batteryPercent < 0.0) batteryPercent = 0.0;

    M5.Lcd.setCursor(20, 120);
    M5.Lcd.printf("Batterie : %.1f%%", batteryPercent);

    // 🎨 **Bouton retour bien visible**
    M5.Lcd.fillRoundRect(60, 180, 200, 40, 8, RED);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(90, 190);
    M5.Lcd.println("Retour (B)");
}

// ✅ **Temporisation au démarrage avec normalisation des caractères**
void attenteDemarrage() {
    M5.Lcd.fillScreen(BLACK);
    
    // 🎨 **Titre bien encadré**
    M5.Lcd.fillRoundRect(30, 10, 260, 40, 10, DARKGREY);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(45, 20);
    M5.Lcd.println(normalizeText("Démarrage auto dans:"));

    unsigned long startTime = millis();
    int lastRemaining = 30;

    while (millis() - startTime < 30000) {
        M5.update();

        // Calcul du temps restant
        int remainingTime = 30 - ((millis() - startTime) / 1000);
        
        // Mise à jour du texte SEULEMENT si le temps change
        if (remainingTime != lastRemaining) {
            lastRemaining = remainingTime;

            // 🎨 **Efface proprement l'ancien affichage du temps**
            M5.Lcd.fillRoundRect(120, 60, 80, 40, 10, BLACK);
            M5.Lcd.setTextColor(YELLOW);
            M5.Lcd.setTextSize(3);
            M5.Lcd.setCursor(145, 70);
            M5.Lcd.printf("%ds", remainingTime);
        }

        // 🟩 **Affichage de la barre de progression**
        int progress = map(remainingTime, 0, 30, 0, 220);
        M5.Lcd.fillRoundRect(50, 120, 220, 20, 5, DARKGREY); // Fond
        M5.Lcd.fillRoundRect(50, 120, progress, 20, 5, GREEN); // Barre verte

        // 🔹 **Si un bouton est pressé, on interrompt l'attente et on affiche le menu**
        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
            currentState = MENU_PRINCIPAL;
            String titleText = normalizeText("Choisissez un menu :");
            drawSingleOption(titleText.c_str(), menuMain[selectedOption]);
            return;
        }

        delay(100); // Rafraîchissement toutes les 100ms
    }

    // ✅ **Si aucun bouton n'est pressé après 30s, on démarre le fonctionnement principal**
    startDiffuseur();
}

// ✅ **Initialisation**
void setup() {
    M5.begin();
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    client.setServer(mqtt_server, 1883);

    currentState = MENU_PRINCIPAL;
    selectedOption = 0;
    drawSingleOption("Choisissez un menu :", menuMain[selectedOption]);

    attenteDemarrage(); // **Ajout de la temporisation au démarrage**
}

// ✅ **Gestion de la navigation**
void loop() {
    M5.update();

    // 🌟 **Menu Principal**
    if (currentState == MENU_PRINCIPAL) {
        if (M5.BtnA.wasPressed()) {
            selectedOption = (selectedOption - 1 + numOptionsMain) % numOptionsMain;
            drawSingleOption("Choisissez un menu :", menuMain[selectedOption]);
        }
        if (M5.BtnC.wasPressed()) {
            selectedOption = (selectedOption + 1) % numOptionsMain;
            drawSingleOption("Choisissez un menu :", menuMain[selectedOption]);
        }
        if (M5.BtnB.wasReleased()) {
            if (selectedOption == 0) {
                currentState = MENU_TESTS;
                selectedOption = 0;
                drawSingleOption("Choisissez un test :", menuTests[selectedOption]);
            } else if (selectedOption == 1) {
                currentState = PARAMETRES;
                showParameters();
            }
        }
    }

    // 🌟 **Menu des tests**
    else if (currentState == MENU_TESTS) {
        if (M5.BtnA.wasPressed()) {
            selectedOption = (selectedOption - 1 + numOptionsTest + 1) % (numOptionsTest + 1);
            drawSingleOption("Choisissez un test :", menuTests[selectedOption]);
        }
        if (M5.BtnC.wasPressed()) {
            selectedOption = (selectedOption + 1) % (numOptionsTest + 1);
            drawSingleOption("Choisissez un test :", menuTests[selectedOption]);
        }
        if (M5.BtnB.wasReleased()) {
            if (selectedOption == numOptionsTest) { // Retour au menu principal
                currentState = MENU_PRINCIPAL;
                selectedOption = 0;
                drawSingleOption("Choisissez un menu :", menuMain[selectedOption]);
            } else { // Lancer un test
                currentState = TEST_EN_COURS;

                M5.Lcd.fillScreen(BLACK);
                M5.Lcd.setCursor(60, 80);
                M5.Lcd.setTextColor(YELLOW);
                M5.Lcd.setTextSize(3);
                M5.Lcd.println("Test en cours...");

                switch (selectedOption) {
                    case 0: testWifi(); break;
                    case 1: testMQTT(); break;
                    case 2: testPresence(); break;
                    case 3: testDiffuseur(); break;
                }

                // 🎨 **Affichage test terminé**
                M5.Lcd.fillScreen(BLACK);
                M5.Lcd.setCursor(60, 80);
                M5.Lcd.setTextColor(WHITE);
                M5.Lcd.setTextSize(3);
                M5.Lcd.println("Test termine !");
                M5.Lcd.setCursor(70, 140);
                M5.Lcd.setTextSize(2);
                M5.Lcd.println("Appuyez sur B");
            }
        }
    }

    // 🌟 **Retour des tests**
    else if (currentState == TEST_EN_COURS) {
        if (M5.BtnB.wasReleased()) {
            currentState = MENU_TESTS;
            selectedOption = 0;
            drawSingleOption("Choisissez un test :", menuTests[selectedOption]);
        }
    }

    // 🌟 **Retour des paramètres**
    else if (currentState == PARAMETRES) {
        if (M5.BtnB.wasReleased()) {
            currentState = MENU_PRINCIPAL;
            selectedOption = 0;
            drawSingleOption("Choisissez un menu :", menuMain[selectedOption]);
        }
    }
}