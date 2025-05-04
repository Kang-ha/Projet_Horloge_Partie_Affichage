#include <iostream>
#include <mysql/mysql.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>

// Fonction pour obtenir l'heure actuelle en minutes depuis minuit
int getCurrentTimeInMinutes() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    return localTime->tm_hour * 60 + localTime->tm_min;
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

    // Paramètres de connexion à la base de données
    const char* server = "localhost";
    const char* user = "root";
    const char* password = "horloge";
    const char* database = "affichage_horloge";

    // Initialisation de la connexion
    conn = mysql_init(nullptr);

    // Connexion à la base de données
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        std::cerr << "Erreur de connexion à MySQL: " << mysql_error(conn) << std::endl;
        return 1;
    }

    // Boucle principale pour vérifier l'heure régulièrement
    while (true) {
        int currentMinutes = getCurrentTimeInMinutes();
        int targetMinutes = currentMinutes + 15; // 15 minutes dans le futur
        
        // Requête SQL pour récupérer les commentaires programmés dans 15 minutes avec audio activé
        std::string query = "SELECT commentaire FROM evenements WHERE debut = " + 
                           std::to_string(targetMinutes) + 
                           " AND commentaire IS NOT NULL "
                           "AND audio = 1"; // Seulement si audio est vrai (1)
        
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return 1;
        }

        res = mysql_use_result(conn);

        // Lecture et synthèse vocale des résultats
        bool commentFound = false;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            if (row[0] != nullptr) {
                int targetHour = targetMinutes / 60;
                int targetMin = targetMinutes % 60;
                std::cout << "[Rappel audio à " << currentMinutes / 60 << "h" << currentMinutes % 60 
                          << "] Commentaire pour " << targetHour << "h" << targetMin 
                          << ": " << row[0] << std::endl;
                speakText(row[0]);
                commentFound = true;
            }
        }

        mysql_free_result(res);

        // Attendre 1 minute avant de vérifier à nouveau
        sleep(60);
    }

    // Nettoyage
    mysql_close(conn);
    return 0;
}