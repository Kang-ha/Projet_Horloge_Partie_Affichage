#include <iostream>
#include <mariadb/mysql.h>
#include <cstdlib>

using namespace std;

int main() {
    // Connexion à la base de données
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    const char *server = "localhost";
    const char *user = "admineleve";
    const char *password = "ieufdl";
    const char *database = "projet_horloge";

    conn = mysql_init(NULL);
    if (!conn) {
        cerr << "Erreur d'initialisation de MySQL/MariaDB\n";
        return 1;
    }

    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        cerr << "Erreur de connexion à la base de données" << mysql_error(conn) << endl;
        return 1;
    }

    // Exécution de la requête SQL pour récupérer le texte
    if (mysql_query(conn, "SELECT texte FROM horloge WHERE id = 1")) {
        cerr << "Erreur d'exécution de la requête" << mysql_error(conn) << endl;
        return 1;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cerr << "Erreur de récupération du résultat" << mysql_error(conn) << endl;
        return 1;
    }

    //récupération du texte et le faire lire par eSpeak
    if ((row = mysql_fetch_row(res))) {
        string texte = row[0];
        count << "Texte récupéré: " << texte << endl;

        // Lecture du texte par eSpeak
        string command = "espeak \"" + texte + "\"";
        system(command.c_str());
    } else {
        count << "Aucun texte trouvé" << endl;
    }
    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}