#include <iostream>
#include <cstdlib>
#include <string>
#include <array>
#include <thread>
#include <chrono>
#include <ctime>

// MySQL Connector/C++
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include <mqtt/async_client.h>

/*
========================================================================================
📌 Contrôle intelligent de la prise TP-Link Tapo via MQTT & MySQL
========================================================================================

🔹 Intègre :
- Détection de présence (via MQTT)
- Lecture base de données MySQL pour vérifier si l’odeur est requise à l’instant T
- Activation intelligente du diffuseur via `tapoctl`
========================================================================================
*/

// Configuration MQTT
const std::string ADDRESS{"tcp://192.168.1.117:1883"};
const std::string CLIENT_ID{"TapoControllerCpp"};
const std::string TOPIC_DIFFUSEUR{"horloge/olfactive/diffuseur"};
const std::string TOPIC_PRESENCE{"horloge/olfactive/presence"};

// Configuration Tapo
const std::string IP = "192.168.1.102";
const std::string USER = "boutin.l@stjolorient.fr";
const std::string PASS = "Voso1295";

// État actuel
bool etatDiffuseur = false;

// 🔹 Vérifie si la prise est accessible
bool checkConnection() {
    std::string cmd = "ping -c 1 -W 1 " + IP + " > /dev/null 2>&1";
    return (std::system(cmd.c_str()) == 0);
}

// 🔹 Récupère l’état de la prise (on/off)
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

// 🔹 Envoie commande tapoctl on/off avec vérifications
void executeTapoCommand(const std::string& command) {
    const int maxAttempts = 3;
    int attempt = 0;

    while (attempt < maxAttempts) {
        if (!checkConnection()) {
            std::cerr << "[ERROR] Prise Tapo inaccessible (Tentative " << (attempt + 1) << ")\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            attempt++;
            continue;
        }

        std::string currentState = getTapoState();
        if (currentState == "ERROR") {
            std::cerr << "[ERROR] État actuel inconnu. Tentative " << (attempt + 1) << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            attempt++;
            continue;
        }

        if ((command == "on" && currentState == "off") || (command == "off" && currentState == "on")) {
            std::cout << "[INFO] Changement : " << currentState << " -> " << command << "\n";

            if (command == "off") {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                if (getTapoState() == "off") {
                    std::cout << "[INFO] Prise déjà éteinte, annulation.\n";
                    return;
                }
            }

            std::string cmd = "tapoctl " + command + " " + IP + " " + USER + " " + PASS;
            int result = std::system(cmd.c_str());

            if (result == 0) {
                std::cout << "[SUCCESS] Commande `" << command << "` exécutée.\n";
                return;
            } else {
                std::cerr << "[ERROR] Commande `" << command << "` échouée.\n";
            }
        } else {
            std::cout << "[INFO] Aucune action nécessaire. Prise déjà à l'état `" << currentState << "`.\n";
            return;
        }

        attempt++;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    std::cerr << "[FATAL ERROR] Impossible d’exécuter `" << command << "` après " << maxAttempts << " tentatives.\n";
}

// 🔹 Fonction qui interroge la BDD pour voir si une odeur est requise actuellement
int isOdeurActiveNow() {
    try {
        sql::Driver* driver = get_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "horloge"));
        conn->setSchema("affichage_horloge");

        // Heure actuelle (en minutes depuis minuit)
        std::time_t now = std::time(nullptr);
        std::tm* local = std::localtime(&now);
        int minutesNow = local->tm_hour * 60 + local->tm_min;

        std::string query =
            "SELECT odeur FROM evenements "
            "WHERE " + std::to_string(minutesNow) + " >= debut "
            "AND " + std::to_string(minutesNow) + " < (debut + duree) "
            "ORDER BY id DESC LIMIT 1";

        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

        if (res->next()) {
            int odeur = res->getInt("odeur");
            std::cout << "[BDD] Odeur active = " << odeur << "\n";
            return odeur;
        } else {
            std::cout << "[BDD] Aucun événement actif à cette heure.\n";
            return -1;
        }

    } catch (sql::SQLException& e) {
        std::cerr << "[SQL ERROR] " << e.what() << "\n";
        return -1;
    }
}

// 🔹 Gestion des messages MQTT
class TapoCallback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string payload = msg->get_payload_str();
        for (char& c : payload) c = tolower(c);

        if (msg->get_topic() == TOPIC_PRESENCE) {
            std::cout << "[MQTT] Présence détectée : " << payload << "\n";

            if (payload == "oui") {
                int odeur = isOdeurActiveNow();
                if (odeur == 1) {
                    executeTapoCommand("on");
                    etatDiffuseur = true;
                } else if (odeur == 0) {
                    executeTapoCommand("off");
                    etatDiffuseur = false;
                }
            } else if (payload == "non") {
                if (etatDiffuseur) {
                    executeTapoCommand("off");
                    etatDiffuseur = false;
                }
            }

        } else if (msg->get_topic() == TOPIC_DIFFUSEUR) {
            std::cout << "[MQTT] Commande manuelle reçue : " << payload << "\n";
            if (payload == "on") {
                executeTapoCommand("on");
                etatDiffuseur = true;
            } else if (payload == "off") {
                executeTapoCommand("off");
                etatDiffuseur = false;
            } else {
                std::cerr << "[ERROR] Commande inconnue : " << payload << "\n";
            }
        }
    }
};

// 🔹 Programme principal
int main() {
    mqtt::async_client client(ADDRESS, CLIENT_ID);
    TapoCallback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        std::cout << "[MQTT] Connexion au broker...\n";
        client.connect(connOpts)->wait();
        std::cout << "[MQTT] Connecté !\n";

        client.subscribe(TOPIC_DIFFUSEUR, 1)->wait();
        client.subscribe(TOPIC_PRESENCE, 1)->wait();

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.disconnect()->wait();
    } catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT] Erreur : " << exc.what() << "\n";
        return 1;
    }

    return 0;
}
