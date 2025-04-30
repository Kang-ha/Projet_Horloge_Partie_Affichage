#include <iostream>
#include <mysql/mysql.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

// Fonction pour exécuter la synthèse vocale avec eSpeak
void speakText(const char* text) {
    if (text == NULL || strlen(text) == 0) {
        std::cout << "Aucun commentaire à lire." << std::endl;
        return;
    }

    // Construction de la commande eSpeak
    std::string command = "espeak -v fr \"";
    command += text;
    command += "\" 2>/dev/null"; // Redirection des erreurs pour plus de propreté

    // Exécution de la commande
    system(command.c_str());
}

int main() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // Paramètres de connexion à la base de données
    const char* server = "localhost";
    const char* user = "admineleve";
    const char* password = "ieufdl";
    const char* database = "projet_horloge";

    // Initialisation de la connexion
    conn = mysql_init(NULL);

    // Connexion à la base de données
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        std::cerr << "Erreur de connexion à MySQL: " << mysql_error(conn) << std::endl;
        return 1;
    }

    // Reqête SQL pour récupérer les commentaires
    if (mysql_query(conn, "SELECT commentaire FROM evenements WHERE commentaire IS NOT NULL")) {
        std::cerr << "Erreur MySQL: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return 1;
    }

    res = mysql_use_result(conn);

    // Lecture et synthèse vocale des résultats
    std::cout << "Lecture des commentaires..." << std::endl;
    while ((row = mysql_fetch_row(res)) != NULL) {
        if (row[0] != NULL) {
            std::cout << "Commentaire: " << row[0] << std::endl;
            speakText(row[0]);
            // Pause entre les commentaires
            sleep(1);
        }
    }

    // Nettoyage
    mysql_free_result(res);
    mysql_close(conn);

    std::cout << "Lecture terminée." << std::endl;
    return 0;
}
