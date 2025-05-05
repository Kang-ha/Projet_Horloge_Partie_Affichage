/**
 * ============================
 *      M5Stack_Core2.ino
 * ============================
 * 
 * Ce programme gère une interface utilisateur sur M5Stack Core2
 * permettant :
 * - de lancer des tests (WiFi, MQTT, présence, diffuseur)
 * - d'afficher les paramètres système (IP, broche capteur, batterie)
 * - une temporisation de 10 secondes au démarrage avant lancement automatique
 * 
 * Navigation via les boutons A (←), B (OK), C (→)
 * État global géré par une énumération de type AppState
 */

#include "config.h"           // Configuration WiFi et MQTT
#include "test_wifi.h"        // Fonction de test WiFi
#include "test_mqtt.h"        // Fonction de test MQTT
#include "test_presence.h"    // Fonction de test de capteur de présence
#include "test_diffuseur.h"   // Fonction de test du diffuseur
#include "main.h"             // Déclarations principales
#include "utils.h"            // Fonctions utilitaires (ex : normalizeText)

// =========================
// États de navigation (machine à états)
// =========================
enum AppState { ACCUEIL, MENU_PRINCIPAL, MENU_TESTS, PARAMETRES, TEST_EN_COURS };
AppState currentState = ACCUEIL; // État initial

// =========================
// Menus et options
// =========================
const int numOptionsMain = 2; // Nombre d'options du menu principal
const int numOptionsTest = 4; // Nombre d'options du menu de test
int selectedOption = 0;       // Index de sélection
const char* menuMain[numOptionsMain] = { "Test", "Parametres" }; // Menu principal
const char* menuTests[numOptionsTest + 1] = { "WiFi", "MQTT", "Presence", "Diffuseur", "Retour" }; // Menu tests

// =========================
// Affichage d'une seule option
// =========================
void drawSingleOption(const char* title, const char* option) {
    M5.Lcd.fillScreen(BLACK); // Efface l'écran

    // Affiche le titre dans un encadré
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println(normalizeText(title));

    // Affiche l'option sélectionnée
    M5.Lcd.fillRoundRect(60, 100, 200, 50, 8, YELLOW);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(80, 115);
    M5.Lcd.println(normalizeText(option));
}

// =========================
// Affichage des paramètres système
// =========================
void showParameters() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);

    // Titre "Parametres"
    M5.Lcd.fillRoundRect(10, 10, 300, 40, 8, DARKGREY);
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.println("Parametres");

    // Affiche l'adresse IP WiFi
    M5.Lcd.setCursor(20, 60);
    M5.Lcd.printf("IP : %s", WiFi.localIP().toString().c_str());

    // Affiche la broche du capteur
    M5.Lcd.setCursor(20, 90);
    M5.Lcd.printf("Broche : %d", sensorPin);

    // Calcule et affiche le pourcentage de batterie
    float batteryVoltage = M5.Axp.GetBatVoltage();
    float batteryPercent = (batteryVoltage - 3.0) / (4.2 - 3.0) * 100.0;
    if (batteryPercent > 100.0) batteryPercent = 100.0;
    if (batteryPercent < 0.0) batteryPercent = 0.0;

    M5.Lcd.setCursor(20, 120);
    M5.Lcd.printf("Batterie : %.1f%%", batteryPercent);

    // Bouton retour
    M5.Lcd.fillRoundRect(60, 180, 200, 40, 8, RED);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(90, 190);
    M5.Lcd.println("Retour (B)");
}

// =========================
// Temporisation au démarrage
// =========================
void attenteDemarrage() {
    M5.Lcd.fillScreen(BLACK);

    // Affiche le titre
    M5.Lcd.fillRoundRect(30, 10, 260, 40, 10, DARKGREY);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(45, 20);
    M5.Lcd.println(normalizeText("Démarrage auto dans"));

    unsigned long startTime = millis(); // Temps de départ
    int lastRemaining = 10;             // Dernier affichage du compte à rebours

    while (millis() - startTime < 10000) {
        M5.update(); // Met à jour les boutons

        int remainingTime = 10 - ((millis() - startTime) / 1000); // Temps restant

        if (remainingTime != lastRemaining) {
            lastRemaining = remainingTime;

            // Met à jour l'affichage du compte à rebours
            M5.Lcd.fillRoundRect(120, 60, 80, 40, 10, BLACK);
            M5.Lcd.setTextColor(YELLOW);
            M5.Lcd.setTextSize(3);
            M5.Lcd.setCursor(145, 70);
            M5.Lcd.printf("%ds", remainingTime);
        }

        // Affichage de la barre de progression
        int progress = map(remainingTime, 0, 10, 0, 220);
        M5.Lcd.fillRoundRect(50, 120, 220, 20, 5, DARKGREY); // Fond
        M5.Lcd.fillRoundRect(50, 120, progress, 20, 5, GREEN); // Remplissage

        // Si un bouton est pressé, on passe au menu
        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
            currentState = MENU_PRINCIPAL;
            String titleText = normalizeText("Choisissez un menu");
            drawSingleOption(titleText.c_str(), menuMain[selectedOption]);
            return;
        }

        delay(100); // Rafraîchissement toutes les 100ms
    }

    // Lancement automatique du diffuseur après 10 secondes
    startDiffuseur();
}

// =========================
// Initialisation
// =========================
void setup() {
    M5.begin();                  // Démarre l'écran et les boutons
    Serial.begin(115200);       // Démarre le port série
    WiFi.begin(ssid, password); // Connexion au WiFi
    client.setServer(mqtt_server, 1883); // Configuration MQTT

    currentState = MENU_PRINCIPAL;
    selectedOption = 0;
    drawSingleOption("Choisissez un menu", menuMain[selectedOption]);

    attenteDemarrage(); // Temporisation
}

// =========================
// Boucle principale
// =========================
void loop() {
    M5.update(); // Met à jour les boutons

    // Menu principal
    if (currentState == MENU_PRINCIPAL) {
        if (M5.BtnA.wasPressed()) {
            selectedOption = (selectedOption - 1 + numOptionsMain) % numOptionsMain;
            drawSingleOption("Choisissez un menu", menuMain[selectedOption]);
        }
        if (M5.BtnC.wasPressed()) {
            selectedOption = (selectedOption + 1) % numOptionsMain;
            drawSingleOption("Choisissez un menu", menuMain[selectedOption]);
        }
        if (M5.BtnB.wasReleased()) {
            if (selectedOption == 0) {
                currentState = MENU_TESTS;
                selectedOption = 0;
                drawSingleOption("Choisissez un test", menuTests[selectedOption]);
            } else if (selectedOption == 1) {
                currentState = PARAMETRES;
                showParameters();
            }
        }
    }

    // Menu des tests
    else if (currentState == MENU_TESTS) {
        if (M5.BtnA.wasPressed()) {
            selectedOption = (selectedOption - 1 + numOptionsTest + 1) % (numOptionsTest + 1);
            drawSingleOption("Choisissez un test", menuTests[selectedOption]);
        }
        if (M5.BtnC.wasPressed()) {
            selectedOption = (selectedOption + 1) % (numOptionsTest + 1);
            drawSingleOption("Choisissez un test", menuTests[selectedOption]);
        }
        if (M5.BtnB.wasReleased()) {
            if (selectedOption == numOptionsTest) {
                // Retour
                currentState = MENU_PRINCIPAL;
                selectedOption = 0;
                drawSingleOption("Choisissez un menu", menuMain[selectedOption]);
            } else {
                // Lancer le test sélectionné
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

                // Affichage de fin
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

    // Retour au menu après un test
    else if (currentState == TEST_EN_COURS) {
        if (M5.BtnB.wasReleased()) {
            currentState = MENU_TESTS;
            selectedOption = 0;
            drawSingleOption("Choisissez un test", menuTests[selectedOption]);
        }
    }

    // Retour au menu principal depuis les paramètres
    else if (currentState == PARAMETRES) {
        if (M5.BtnB.wasReleased()) {
            currentState = MENU_PRINCIPAL;
            selectedOption = 0;
            drawSingleOption("Choisissez un menu", menuMain[selectedOption]);
        }
    }
}