#include <iostream>
#include <cstdlib>
#include <string>
#include <mqtt/async_client.h>
#include <array>
#include <thread>
#include <chrono>

/*
========================================================================================
📌 **Contrôle d'une prise TP-Link Tapo via MQTT**
========================================================================================

🔹 **Description :**
Ce programme permet de contrôler une prise TP-Link Tapo (P100) via le protocole MQTT.
Il reçoit des commandes depuis un broker MQTT pour allumer ou éteindre la prise en fonction de la présence détectée.
Le programme interagit avec la prise en utilisant la commande `tapoctl`.

🔹 **Fonctionnement :**
1️⃣ **Connexion MQTT** :  
   - Se connecte au broker MQTT et s'abonne aux topics `horloge/olfactive/diffuseur` et `horloge/olfactive/presence`.
   - Écoute les messages entrants pour allumer (`on`) ou éteindre (`off`) la prise.

2️⃣ **Gestion des commandes ON/OFF** :  
   - Vérifie l’état actuel de la prise avant d'envoyer une commande (évite les actions inutiles).
   - Assure une protection contre les erreurs **403** en ajoutant un délai avant la désactivation.
   - Tente **jusqu'à 3 fois** d’exécuter la commande en cas d’échec.

3️⃣ **Vérifications & Robustesse** :  
   - Vérifie la connexion réseau avant d’envoyer une commande.
   - Convertit automatiquement les commandes reçues en **minuscule** pour éviter les erreurs avec `tapoctl`.
   - Gère les erreurs et affiche des logs détaillés pour le débogage.

🔹 **Utilisation :**
- Le programme tourne en arrière-plan sur la **Raspberry Pi Affichage**.
- Il attend les commandes MQTT et les applique à la prise en temps réel.
- Les logs permettent de surveiller le fonctionnement et les erreurs éventuelles.

========================================================================================
*/

// 🔹 **Configuration MQTT**
const std::string ADDRESS{"tcp://192.168.1.117:1883"};
const std::string CLIENT_ID{"TapoControllerCpp"};
const std::string TOPIC_DIFFUSEUR{"horloge/olfactive/diffuseur"};
const std::string TOPIC_PRESENCE{"horloge/olfactive/presence"};

// 🔹 **Configuration de la prise TP-Link Tapo**
const std::string IP = "192.168.1.102";
const std::string USER = "boutin.l@stjolorient.fr";
const std::string PASS = "Voso1295";

// 🔹 **État actuel du diffuseur**
bool etatDiffuseur = false;

// 🔹 **Vérifie si la prise est bien accessible (Ping)**
bool checkConnection() {
    std::string cmd = "ping -c 1 -W 1 " + IP + " > /dev/null 2>&1";
    return (std::system(cmd.c_str()) == 0);
}

// 🔹 **Récupère l’état actuel de la prise**
std::string getTapoState() {
    std::string cmd = "tapoctl info " + IP + " " + USER + " " + PASS + " | grep 'Power State'";
    std::array<char, 128> buffer;
    std::string result;
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);

    if (result.find("Power State: On") != std::string::npos) return "on";
    if (result.find("Power State: Off") != std::string::npos) return "off";
    return "ERROR";
}

// 🔹 **Envoie une commande `on/off` avec gestion automatique des erreurs**
void executeTapoCommand(const std::string& command) {
    const int maxAttempts = 3;  // 🔁 Nombre maximal de tentatives en cas d'échec
    int attempt = 0;

    while (attempt < maxAttempts) {
        // 🔎 Vérifier la connexion réseau
        if (!checkConnection()) {
            std::cerr << "[ERROR] Prise Tapo inaccessible (Tentative " << (attempt + 1) << "/" << maxAttempts << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            attempt++;
            continue;
        }

        std::string currentState = getTapoState();
        if (currentState == "ERROR") {
            std::cerr << "[ERROR] Impossible de récupérer l’état de la prise (Tentative " << (attempt + 1) << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            attempt++;
            continue;
        }

        // ⚖️ Vérification de l'état avant de modifier la prise
        if ((command == "on" && currentState == "off") || (command == "off" && currentState == "on")) {
            std::cout << "[INFO] Changement d’état détecté : " << currentState << " -> " << command << std::endl;

            // ⏳ Protection contre erreur 403 : Vérification avant OFF
            if (command == "off") {
                std::cout << "[DEBUG] Vérification avant d’éteindre la prise..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
                if (getTapoState() == "off") {
                    std::cout << "[INFO] La prise est déjà éteinte, annulation de la commande OFF." << std::endl;
                    return;
                }
            }

            // 📡 Envoi de la commande tapoctl
            std::string cmd = "tapoctl " + command + " " + IP + " " + USER + " " + PASS;
            std::cout << "[DEBUG] Envoi de la commande tapoctl..." << std::endl;
            int result = std::system(cmd.c_str());

            if (result == 0) {
                std::cout << "[SUCCESS] Commande `" << command << "` exécutée avec succès." << std::endl;
                return;
            } else {
                std::cerr << "[ERROR] Échec de la commande `" << command << "` (Tentative " << (attempt + 1) << ")" << std::endl;
            }
        } else {
            std::cout << "[INFO] Aucun changement nécessaire (État actuel : " << currentState << ")." << std::endl;
            return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));  // 🔄 Pause avant nouvelle tentative
        attempt++;
    }

    std::cerr << "[FATAL ERROR] Impossible d’exécuter `" << command << "` après " << maxAttempts << " tentatives." << std::endl;
}

// 🔹 **Callback MQTT**
class TapoCallback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string payload = msg->get_payload_str();
        std::cout << "[MQTT] Message reçu sur " << msg->get_topic() << " : " << payload << std::endl;

        // 🔄 Conversion en minuscule pour éviter les erreurs
        for (char& c : payload) c = tolower(c);

        if (msg->get_topic() == TOPIC_DIFFUSEUR) {
            if (payload == "on") {
                executeTapoCommand("on");
                etatDiffuseur = true;
            } else if (payload == "off") {
                executeTapoCommand("off");
                etatDiffuseur = false;
            } else {
                std::cerr << "[ERROR] Commande inconnue reçue : `" << payload << "`" << std::endl;
            }
        } else if (msg->get_topic() == TOPIC_PRESENCE) {
            std::cout << "[INFO] Mise à jour de l’état de présence : " << payload << std::endl;
        } else {
            std::cerr << "[MQTT] Message non reconnu : " << payload << std::endl;
        }
    }
};

// 🔹 **Programme principal**
int main() {
    // 📡 Configuration du client MQTT
    mqtt::async_client client(ADDRESS, CLIENT_ID);
    TapoCallback cb;
    client.set_callback(cb);
    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        std::cout << "[MQTT] Connexion au broker MQTT..." << std::endl;
        auto tok = client.connect(connOpts);
        tok->wait();
        std::cout << "[MQTT] Connecté au broker !" << std::endl;

        // 📡 Abonnement aux topics MQTT
        client.subscribe(TOPIC_DIFFUSEUR, 1)->wait();
        client.subscribe(TOPIC_PRESENCE, 1)->wait();
        std::cout << "[MQTT] Abonné aux topics : " << TOPIC_DIFFUSEUR << " et " << TOPIC_PRESENCE << std::endl;

        // 🔄 Boucle principale : maintien de la connexion MQTT
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.disconnect()->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT] Erreur : " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
