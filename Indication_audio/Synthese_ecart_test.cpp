#include <iostream>
#include <mysql/mysql.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <string>
#include <map>

// Fonction pour obtenir l'heure actuelle en minutes depuis minuit
int getCurrentTimeInMinutes() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    return localTime->tm_hour * 60 + localTime->tm_min;
}

// Fonction pour obtenir le jour de la semaine (1=lundi...7=dimanche)
int getCurrentDayOfWeek() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    return localTime->tm_wday == 0 ? 7 : localTime->tm_wday;
}

void speakText(const char* text) {
    if (!text || !*text) return;
    std::string cmd = "espeak -v fr \"" + std::string(text) + "\" 2>/dev/null";
    system(cmd.c_str());
}

int main() {
    MYSQL *conn = mysql_init(nullptr);
    const char* db_params[] = {"localhost", "root", "horloge", "projet_horloge"};
    
    if (!mysql_real_connect(conn, db_params[0], db_params[1], db_params[2], db_params[3], 0, nullptr, 0)) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        return 1;
    }

    std::map<std::string, int> last_triggered; // Stocke last_triggered_minute par ID

    while (true) {
        int current_min = getCurrentTimeInMinutes();
        int current_day = getCurrentDayOfWeek();
        const char* jours[] = {"", "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche"};

        std::string query = 
            "SELECT id, commentaire, jour_semaine, ecart "
            "FROM evenements "
            "WHERE debut BETWEEN " + std::to_string(current_min) + " AND " + std::to_string(current_min + 15) + 
            " AND audio = 1 AND commentaire IS NOT NULL";

        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
            sleep(60);
            continue;
        }

        MYSQL_RES* res = mysql_use_result(conn);
        MYSQL_ROW row;

        while ((row = mysql_fetch_row(res))) {
            if (!row[0] || !row[1] || !row[2] || !row[3] || strlen(row[2]) < 7) continue;

            std::string id = row[0];
            int ecart_min = atoi(row[3]); // Ecart en minutes
            int last_min = last_triggered[id];

            // Vérifie jour et écart
            if (row[2][current_day - 1] == '1' && 
               (ecart_min == 0 || (current_min - last_min) >= ecart_min)) {
                
                std::cout << "[" << jours[current_day] << " " 
                          << current_min/60 << "h" << current_min%60 << "] " 
                          << row[1] << " (Ecart: " << row[3] << " min)\n";
                
                speakText(row[1]);
                last_triggered[id] = current_min;
            }
        }

        mysql_free_result(res);
        sleep(60); // Vérifie toutes les minutes
    }

    mysql_close(conn);
    return 0;
}
