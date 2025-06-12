#include <iostream>
#include <mysql/mysql.h>       // Pour la connexion MySQL
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>               // Pour la gestion du temps
#include <string>
#include <algorithm>           // Pour les transformations de chaînes
#include <chrono>              // Pour la gestion du temps précis
#include <thread>              // Pour les pauses

// MQTT
#include <mqtt/async_client.h>        // Client MQTT asynchrone


const std::string ADDRESS{"tcp://192.168.1.9:1883"};  // Adresse du broker MQTT
const std::string CLIENT_ID{"SyntheseClient"};         // ID du client MQTT
const std::string TOPIC_PRESENCE{"horloge/olfactive/presence"};  // Topic MQTT

std::chrono::steady_clock::time_point lastPresenceTime;  // Dernière détection de présence

bool presenceActuelle = false;        // État actuel de présence
bool syntheseDéjàLancée = false;      // Flag pour éviter les doubles déclenchements

// Fonction pour obtenir l'heure actuelle en minutes depuis minuit
// Fonction pour obtenir l'heure actuelle en minutes depuis minuit
int getCurrentTimeInMinutes() {
    std::time_t now = std::time(nullptr); // Obtient le temps actuel en secondes depuis l'époque
    std::tm* localTime = std::localtime(&now); // Convertit ce temps en heure locale
    return localTime->tm_hour * 60 + localTime->tm_min; // Retourne le nombre total de minutes depuis minuit
}

// Fonction pour obtenir le jour de la semaine (1=lundi, 2=mardi...7=dimanche)
int getCurrentDayOfWeek() {
    std::time_t now = std::time(nullptr); // Obtient le temps actuel
    std::tm* localTime = std::localtime(&now); // Convertit en heure locale
    return localTime->tm_wday == 0 ? 7 : localTime->tm_wday; // Retourne le jour de la semaine (0=dimanche, donc on le transforme en 7)
}

// Fonction pour obtenir le numéro de la semaine ISO
int getCurrentWeekNumber() {
    std::time_t now = std::time(nullptr); // Obtient le temps actuel
    std::tm* localTime = std::localtime(&now); // Convertit en heure locale
    char weekNum[3];
    strftime(weekNum, sizeof(weekNum), "%V", localTime); // Formate le numéro de semaine ISO dans weekNum
    return atoi(weekNum); // Convertit le numéro de semaine en entier et le retourne
}

// Fonction pour exécuter la synthèse vocale avec eSpeak
void speakText(const char* text) {
    if (text == nullptr || strlen(text) == 0) {
        std::cout << "Aucun commentaire à lire." << std::endl; // Affiche un message si le texte est vide
        return;
    }

    std::string command = "espeak -v fr \"" + std::string(text) + "\" 2>/dev/null"; // Construit la commande pour eSpeak
    system(command.c_str()); // Exécute la commande système pour la synthèse vocale
}

// Fonction appelée lors de la présence confirmée
void lancerSynthese() {
    MYSQL *conn; // Pointeur pour la connexion MySQL
    MYSQL_RES *res; // Pointeur pour le résultat de la requête
    MYSQL_ROW row; // Pointeur pour une ligne de résultat

    const char* server = "localhost"; // Adresse du serveur MySQL
    const char* user = "root"; // Nom d'utilisateur MySQL
    const char* password = "horloge"; // Mot de passe MySQL
    const char* database = "projet_horloge"; // Nom de la base de données

    conn = mysql_init(nullptr); // Initialise la connexion MySQL
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl; // Affiche une erreur si la connexion échoue
        return;
    }

    int currentWeek = getCurrentWeekNumber(); // Obtient le numéro de la semaine actuelle
    int currentMinutes = getCurrentTimeInMinutes(); // Obtient les minutes actuelles depuis minuit
    int currentDay = getCurrentDayOfWeek(); // Obtient le jour actuel de la semaine
    int targetMinutes = currentMinutes + 15; // Calcule les minutes cibles (actuelles + 15)

    std::string query = "SELECT commentaire, jour_semaine, ecart FROM evenements "
                        "WHERE debut = " + std::to_string(targetMinutes) +
                        " AND commentaire IS NOT NULL "
                        "AND audio = 1"; // Construit la requête SQL

    std::cout << "[DEBUG] Requête exécutée : " << query << "\n"; // Affiche la requête pour le débogage

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl; // Affiche une erreur si la requête échoue
        mysql_close(conn); // Ferme la connexion
        return;
    }

    res = mysql_use_result(conn); // Utilise le résultat de la requête
    bool resultEmpty = true; // Variable pour vérifier si le résultat est vide

    const char* joursNoms[] = {"", "lundi", "mardi", "mercredi", "jeudi",
                               "vendredi", "samedi", "dimanche"}; // Tableau des noms des jours

    while ((row = mysql_fetch_row(res))) { // Parcourt chaque ligne du résultat
        resultEmpty = false; // Indique que le résultat n'est pas vide

        std::cout << "[DEBUG] commentaire: " << row[0]
                  << " | jours: " << row[1]
                  << " | ecart: " << row[2]; // Affiche les détails de la ligne pour le débogage

        if (row[0] && row[1] && row[2] && row[3] && strlen(row[1]) >= 7) { // Vérifie si les champs requis sont présents
            if (row[1][currentDay - 1] != '1') continue; // Vérifie si l'événement est prévu pour aujourd'hui

            int ecart = atoi(row[2]); // Convertit l'écart en entier
            std::string dateDebutStr = row[3] ? row[3] : ""; // Obtient la date de début
            int semaineDebut = -1; // Variable pour la semaine de début

            std::cout << "[Rappel " << joursNoms[currentDay] << " "
                      << currentMinutes/60 << "h" << currentMinutes%60
                      << "] " << row[0] << " (Ecart: " << row[2] << " semaines)" << std::endl; // Affiche le rappel
            speakText(row[0]); // Exécute la synthèse vocale du commentaire
        }
    }
    if (resultEmpty) {
        std::cout << "[INFO] Aucune ligne trouvée correspondant aux critères.\n"; // Affiche un message si aucun résultat n'est trouvé
    }
    mysql_free_result(res); // Libère le résultat
    mysql_close(conn); // Ferme la connexion
}

// Callback MQTT
class SyntheseCallback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string payload = msg->get_payload_str(); // Obtient le payload du message MQTT
        std::transform(payload.begin(), payload.end(), payload.begin(), ::tolower); // Convertit le payload en minuscules

        if (msg->get_topic() == TOPIC_PRESENCE) { // Vérifie si le sujet du message est celui de la présence
            if (payload.find("presence detectee") != std::string::npos) { // Vérifie si le message indique une présence détectée
                if (!presenceActuelle) {
                    std::cout << "[MQTT] Présence détectée, début de temporisation...\n"; // Affiche un message de présence détectée
                    lastPresenceTime = std::chrono::steady_clock::now(); // Enregistre le temps actuel
                    presenceActuelle = true; // Indique que la présence est actuelle
                    syntheseDéjàLancée = false; // Indique que la synthèse n'a pas encore été lancée
                }
            } else if (payload.find("aucune presence") != std::string::npos) { // Vérifie si le message indique aucune présence
                std::cout << "[MQTT] Présence absente, annulation.\n"; // Affiche un message d'absence de présence
                presenceActuelle = false; // Indique qu'il n'y a pas de présence actuelle
                syntheseDéjàLancée = false; // Indique que la synthèse n'a pas été lancée
            }
        }
    }

    void tick() {
        if (presenceActuelle && !syntheseDéjàLancée) { // Vérifie si la présence est actuelle et la synthèse n'a pas été lancée
            auto now = std::chrono::steady_clock::now(); // Obtient le temps actuel
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPresenceTime).count(); // Calcule la durée depuis la dernière présence

            if (duration >= 10) { // Vérifie si 10 secondes se sont écoulées
                std::cout << "[INFO] Présence confirmée après 10s, exécution de la synthèse...\n"; // Affiche un message de confirmation de présence
                lancerSynthese(); // Lance la synthèse vocale
                syntheseDéjàLancée = true; // Indique que la synthèse a été lancée
                presenceActuelle = false; // Indique qu'il n'y a plus de présence actuelle
            }
        }
    }
};

int main() {
    mqtt::async_client client(ADDRESS, CLIENT_ID); // Crée un client MQTT asynchrone
    SyntheseCallback cb; // Crée une instance du callback
    client.set_callback(cb); // Définit le callback pour le client
    mqtt::connect_options connOpts; // Crée les options de connexion

    connOpts.set_clean_session(true); // Définit une session propre

    try {
        std::cout << "[MQTT] Connexion au broker...\n"; // Affiche un message de tentative de connexion
        client.connect(connOpts)->wait(); // Connecte le client au broker
        std::cout << "[MQTT] Connecté !\n"; // Affiche un message de connexion réussie
        client.subscribe(TOPIC_PRESENCE, 1)->wait(); // Souscrit au sujet de la présence
    } catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT] Erreur : " << exc.what() << "\n"; // Affiche une erreur si la connexion échoue
        return 1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Fait une pause d'une seconde
        cb.tick(); // Vérifie chaque seconde si la présence est confirmée
    }
    return 0;
}
+
