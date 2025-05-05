#include <iostream>
#include <mysql/mysql.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <string>

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
    // Convertit de 0-6 (dimanche=0) à 1-7 (lundi=1)
    return localTime->tm_wday == 0 ? 7 : localTime->tm_wday;
}

// Fonction pour exécuter la synthèse vocale avec eSpeak
void speakText(const char* text) {
    if (text == nullptr || strlen(text) == 0) {
        std::cout << "Aucun commentaire à lire." << std::endl;
        return;
    }

    std::string command = "espeak -v fr \"";
    command += text;
    command += "\" 2>/dev/null";
    system(command.c_str());
}

int main() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // Paramètres de connexion
    const char* server = "localhost";
    const char* user = "root";
    const char* password = "horloge";
    const char* database = "affichage_horloge";

    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        return 1;
    }

    while (true) {
        int currentMinutes = getCurrentTimeInMinutes();
        int currentDay = getCurrentDayOfWeek(); // 1-7 (lundi=1)
        int targetMinutes = currentMinutes + 15;
        
        std::string query = "SELECT commentaire, jour_semaine FROM evenements "
                          "WHERE debut = " + std::to_string(targetMinutes) + 
                          " AND commentaire IS NOT NULL "
                          "AND audio = 1";
        
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
            sleep(60);
            continue;
        }

        res = mysql_use_result(conn);
        bool commentFound = false;
        const char* joursNoms[] = {"", "lundi", "mardi", "mercredi", "jeudi", 
                                 "vendredi", "samedi", "dimanche"};

        while (row = mysql_fetch_row(res)) {
            if (row[0] && row[1] && strlen(row[1]) >= 7) {
                char jourActive = row[1][currentDay - 1]; // Index 0-6
                if (jourActive == '1') {
                    std::cout << "[Rappel " << joursNoms[currentDay] << " " 
                              << currentMinutes/60 << "h" << currentMinutes%60
                              << "] " << row[0] << std::endl;
                    speakText(row[0]);
                    commentFound = true;
                }
            }
        }

        mysql_free_result(res);
        sleep(60);
    }

    mysql_close(conn);
    return 0;
}
