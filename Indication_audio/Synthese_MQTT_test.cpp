#include <iostream>
#include <mysql/mysql.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>

// MQTT
#include <mqtt/async_client.h>

const std::string ADDRESS{"tcp://192.168.1.117:1883"};
const std::string CLIENT_ID{"SyntheseClient"};
const std::string TOPIC_PRESENCE{"horloge/olfactive/presence"};

std::chrono::steady_clock::time_point lastPresenceTime;
bool presenceActuelle = false;
bool syntheseDéjàLancée = false;

// Fonction pour obtenir l'heure actuelle en minutes depuis minuit
int getCurrentTimeInMinutes() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    return localTime->tm_hour * 60 + localTime->tm_min;
}

// Fonction appelée lors de la présence confirmée
void lancerSynthese() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    const char* server   = "localhost";
    const char* user     = "root";
    const char* password = "horloge";
    const char* database = "affichage_horloge";

    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        return;
    }

    int currentMinutes = getCurrentTimeInMinutes();
    int targetMinutes  = currentMinutes + 15;  // on cherche les events qui démarrent dans 15 min

    std::string query = "SELECT commentaire, jour_semaine, ecart, date_debut "
                        "FROM evenements "
                        "WHERE debut = " + std::to_string(targetMinutes) + " "
                        "AND commentaire IS NOT NULL "
                        "AND audio = 1";

    std::cout << "[DEBUG] Requête exécutée : " << query << "\n";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return;
    }

    res = mysql_use_result(conn);
    bool resultEmpty = true;

    const char* joursNoms[] = {"", "lundi", "mardi", "mercredi", "jeudi",
                               "vendredi", "samedi", "dimanche"};

    while ((row = mysql_fetch_row(res))) {
        resultEmpty = false;

        const char* commentaire = row[0];
        const char* bitmask    = row[1];  // masque jour_semaine, ex "0111110"
        int ecartRaw           = row[2] ? atoi(row[2]) : 0;
        std::string dateDebutStr = row[3] ? row[3] : "";

        std::cout << "[DEBUG] commentaire: " << commentaire
                  << " | jours: "      << bitmask
                  << " | ecart: "      << ecartRaw
                  << " | date_debut: " << dateDebutStr << "\n";

        // Vérification du jour de la semaine
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        int currentDay = (localTime->tm_wday == 0 ? 7 : localTime->tm_wday);
        if (!bitmask || (int)std::strlen(bitmask) < 7 || bitmask[currentDay - 1] != '1')
            continue;

        // Calcul de l'intervalle en minutes
        int ecart = ecartRaw <= 1 ? 1 : ecartRaw;  // si 0 ou 1 => toutes les minutes
        int debutMinutes = 0;

        // On attend que date_debut soit de la forme "YYYY-MM-DD HH:MM:SS"
        if (!dateDebutStr.empty()) {
            std::tm tm_debut = {};
            strptime(dateDebutStr.c_str(), "%Y-%m-%d %H:%M:%S", &tm_debut);
            debutMinutes = tm_debut.tm_hour * 60 + tm_debut.tm_min;
        }

        int elapsed = currentMinutes - debutMinutes;
        if (elapsed < 0 || (elapsed % ecart) != 0)
            continue;

        // Tout est OK, on lit le commentaire
        std::cout << "[Rappel " << joursNoms[currentDay] << " "
                  << (currentMinutes / 60) << "h" << (currentMinutes % 60)
                  << "] " << commentaire
                  << " (Ecart : " << ecartRaw << " minutes)" << std::endl;

        // Synthèse vocale
        if (commentaire && std::strlen(commentaire) > 0) {
            std::string cmd = "espeak -v fr \"" + std::string(commentaire) + "\" 2>/dev/null";
            system(cmd.c_str());
        }
    }

    if (resultEmpty) {
        std::cout << "[INFO] Aucune ligne trouvée correspondant aux critères.\n";
    }

    mysql_free_result(res);
    mysql_close(conn);
}

// Callback MQTT
class SyntheseCallback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string payload = msg->get_payload_str();
        std::transform(payload.begin(), payload.end(), payload.begin(), ::tolower);

        if (msg->get_topic() == TOPIC_PRESENCE) {
            if (payload.find("presence detectee") != std::string::npos) {
                if (!presenceActuelle) {
                    std::cout << "[MQTT] Présence détectée, début de temporisation...\n";
                    lastPresenceTime = std::chrono::steady_clock::now();
                    presenceActuelle = true;
                    syntheseDéjàLancée = false;
                }
            }
            else if (payload.find("aucune presence") != std::string::npos) {
                std::cout << "[MQTT] Présence absente, annulation.\n";
                presenceActuelle = false;
                syntheseDéjàLancée = false;
            }
        }
    }

    void tick() {
        if (presenceActuelle && !syntheseDéjàLancée) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPresenceTime).count();

            if (duration >= 10) {
                std::cout << "[INFO] Présence confirmée après 10 s, exécution de la synthèse...\n";
                lancerSynthese();
                syntheseDéjàLancée = true;
                presenceActuelle = false;
            }
        }
    }
};

int main() {
    mqtt::async_client client(ADDRESS, CLIENT_ID);
    SyntheseCallback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        std::cout << "[MQTT] Connexion au broker...\n";
        client.connect(connOpts)->wait();
        std::cout << "[MQTT] Connecté !\n";
        client.subscribe(TOPIC_PRESENCE, 1)->wait();
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT] Erreur : " << exc.what() << "\n";
        return 1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cb.tick();
    }

    return 0;
}
