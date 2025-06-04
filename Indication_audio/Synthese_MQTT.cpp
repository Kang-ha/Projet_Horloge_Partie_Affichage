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

// Fonction pour obtenir le jour de la semaine (1=lundi, 2=mardi...7=dimanche)
int getCurrentDayOfWeek() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    return localTime->tm_wday == 0 ? 7 : localTime->tm_wday;
}

// Fonction pour obtenir le numéro de la semaine ISO
int getCurrentWeekNumber() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char weekNum[3];
    strftime(weekNum, sizeof(weekNum), "%V", localTime);
    return atoi(weekNum);
}

// Fonction pour exécuter la synthèse vocale avec eSpeak
void speakText(const char* text) {
    if (text == nullptr || strlen(text) == 0) {
        std::cout << "Aucun commentaire à lire." << std::endl;
        return;
    }

    std::string command = "espeak -v fr \"" + std::string(text) + "\" 2>/dev/null";
    system(command.c_str());
}

// Fonction appelée lors de la présence confirmée
void lancerSynthese() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    const char* server = "localhost";
    const char* user = "root";
    const char* password = "horloge";
    const char* database = "projet_horloge";

    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        return;
    }

    int currentWeek = getCurrentWeekNumber();
    int currentMinutes = getCurrentTimeInMinutes();
    int currentDay = getCurrentDayOfWeek();
    int targetMinutes = currentMinutes + 15;

    std::string query = "SELECT commentaire, jour_semaine, ecart, date_debut FROM evenements "
                        "WHERE debut = " + std::to_string(targetMinutes) +
                        " AND commentaire IS NOT NULL "
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

        std::cout << "[DEBUG] commentaire: " << row[0]
                  << " | jours: " << row[1]
                  << " | ecart: " << row[2]
                  << " | date_debut: " << (row[3] ? row[3] : "NULL") << "\n";

        if (row[0] && row[1] && row[2] && row[3] && strlen(row[1]) >= 7) {
            if (row[1][currentDay - 1] != '1') continue;

            int ecart = atoi(row[2]);
            std::string dateDebutStr = row[3] ? row[3] : "";
            int semaineDebut = -1;

            // Conversion de date_debut en semaine ISO
            if (!dateDebutStr.empty()) {
                std::tm tm_debut = {};
                strptime(dateDebutStr.c_str(), "%Y-%m-%d", &tm_debut);
                char semaineISO[3];
                strftime(semaineISO, sizeof(semaineISO), "%V", &tm_debut);
                semaineDebut = atoi(semaineISO);
            }

            if (ecart >= 0 && semaineDebut >= 0) {
                if ((currentWeek - semaineDebut) % (ecart + 1) != 0) continue;
            }

            std::cout << "[Rappel " << joursNoms[currentDay] << " " 
                      << currentMinutes/60 << "h" << currentMinutes%60
                      << "] " << row[0] << " (Ecart: " << row[2] << " semaines)" << std::endl;

            speakText(row[0]);
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
            } else if (payload.find("aucune presence") != std::string::npos) {
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
                std::cout << "[INFO] Présence confirmée après 10s, exécution de la synthèse...\n";
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
    } catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT] Erreur : " << exc.what() << "\n";
        return 1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cb.tick();  // Vérifie chaque seconde si la présence est confirmée
    }

    return 0;
}
