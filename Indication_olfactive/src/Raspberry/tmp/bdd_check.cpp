#include <iostream>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

// Renvoie 0 ou 1 selon la valeur d'odeur dans la dernière ligne
int getOdeurValue() {
    try {
        // Connexion à la base de données
        sql::Driver* driver = get_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "horloge"));
        conn->setSchema("affichage_horloge");

        // Requête SQL
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT odeur FROM evenements ORDER BY id DESC LIMIT 1"));

        if (res->next()) {
            int odeur = res->getInt("odeur");
            return odeur;
        } else {
            std::cerr << "[ERROR] Aucune donnée dans la table `evenements`.\n";
            return -1;
        }

    } catch (sql::SQLException& e) {
        std::cerr << "[SQL ERROR] " << e.what() << std::endl;
        return -1;
    }
}

int main() {
    int valeur = getOdeurValue();
    std::cout << "Valeur de odeur = " << valeur << std::endl;
    return 0;
}
