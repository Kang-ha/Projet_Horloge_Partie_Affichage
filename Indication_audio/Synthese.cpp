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
    const char* database = "projet_horloge";

    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        return 1;
    }

    int currentWeek = getCurrentWeekNumber(); // Récupère la semaine actuelle

    while (true) {
        int currentMinutes = getCurrentTimeInMinutes();
        int currentDay = getCurrentDayOfWeek(); // 1-7 (lundi=1)
        int targetMinutes = currentMinutes + 15;
        
        std::string query = "SELECT commentaire, jour_semaine, ecart FROM evenements "
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

        while ((row = mysql_fetch_row(res))) {
            if (row[0] && row[1] && row[2] && strlen(row[1]) >= 7) {
                // Vérifie le jour de la semaine
                if (row[1][currentDay - 1] != '1') continue;
                
                // Vérifie l'écart des semaines
                int ecart = atoi(row[2]);
                if (ecart > 0 && (currentWeek % (ecart + 1)) != 0) continue;
                
                std::cout << "[Rappel " << joursNoms[currentDay] << " " 
                          << currentMinutes/60 << "h" << currentMinutes%60
                          << "] " << row[0] << " (Ecart: " << row[2] << " semaines)" << std::endl;
                speakText(row[0]);
                commentFound = true;
            }
        }

        mysql_free_result(res);
        
        // Mise à jour hebdomadaire
        static int lastWeek = currentWeek;
        if (currentWeek != lastWeek) {
            lastWeek = currentWeek;
            currentWeek = getCurrentWeekNumber(); // Actualise le numéro de semaine
        }

        sleep(60);
    }

    mysql_close(conn);
    return 0;
}
