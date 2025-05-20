/*
========================================================================================
Fichier : diffuseur_control.cpp

Objectif :
Ce programme contrôle intelligemment une prise connectée TP-Link Tapo en fonction :
- des messages MQTT (présence ou commande manuelle)
- des événements enregistrés en base de données MySQL

Fonctionnalités :
- Écoute MQTT sur les topics "presence" et "diffuseur"
- Vérifie si une odeur est programmée à l’instant T (heure, jour, utilisateur)
- Active/désactive la prise avec temporisation et robustesse

Technologies utilisées :
- MQTT (lib Paho)
- MySQL Connector/C++
- Commandes système (tapoctl + ping)
- C++ moderne (chrono, threads, smart pointers)

Ce fichier s’exécute en boucle infinie sur la Raspberry Pi. Il agit comme contrôleur maître.
========================================================================================
*/

#include <iostream>                 // Pour affichage console
#include <cstdlib>                  // Pour std::system (exécution de commandes shell)
#include <string>                   // Pour std::string
#include <array>                    // Pour std::array utilisé dans popen
#include <thread>                   // Pour std::this_thread::sleep_for
#include <chrono>                   // Pour la temporisation en secondes
#include <ctime>                    // Pour récupérer l’heure actuelle

// 📦 Librairies MySQL Connector/C++
#include <mysql_driver.h>          // Pour obtenir un driver MySQL
#include <mysql_connection.h>      // Pour la connexion à la base
#include <cppconn/driver.h>        // Interfaces du driver
#include <cppconn/exception.h>     // Gestion des erreurs SQL
#include <cppconn/statement.h>     // Requêtes SQL
#include <cppconn/resultset.h>     // Résultats des requêtes

#include <mqtt/async_client.h>     // Client MQTT asynchrone de la bibliothèque Paho

// 📌 Contrôle intelligent de la prise TP-Link Tapo via MQTT & MySQL

// Configuration MQTT
const std::string ADDRESS{"tcp://192.168.1.117:1883"}; // Adresse du broker MQTT
const std::string CLIENT_ID{"TapoControllerCpp"};      // ID du client MQTT
const std::string TOPIC_DIFFUSEUR{"horloge/olfactive/diffuseur"}; // Topic pour commandes manuelles
const std::string TOPIC_PRESENCE{"horloge/olfactive/presence"};   // Topic pour détection de présence

// Configuration Tapo
const std::string IP = "192.168.1.102";                     // IP de la prise connectée
const std::string USER = "boutin.l@stjolorient.fr";         // Identifiant de connexion Tapo
const std::string PASS = "Voso1295";                        // Mot de passe Tapo

// État actuel du diffuseur (vrai si allumé, faux sinon)
bool etatDiffuseur = false;

bool checkConnection() {
    std::string cmd = "ping -c 1 -W 1 " + IP + " > /dev/null 2>&1"; // Commande ping vers la prise (1 essai, délai 1s), redirigée vers null
    return (std::system(cmd.c_str()) == 0); // Exécute la commande shell et retourne vrai si le ping réussit (code retour = 0)
}

std::string getTapoState() {
    std::string cmd = "tapoctl info " + IP + " " + USER + " " + PASS + " | grep 'Power State'"; // Commande tapoctl filtrée pour l’état
    std::array<char, 128> buffer;           // Buffer temporaire pour stocker les lignes lues
    std::string result;                     // Chaîne dans laquelle on construit le résultat

    FILE* pipe = popen(cmd.c_str(), "r");   // Lance la commande et ouvre un flux pour lire sa sortie standard (comme un fichier)
    if (!pipe) return "ERROR";              // Si échec d’ouverture, retourne "ERROR"

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();            // Ajoute chaque ligne lue au résultat final
    }
    pclose(pipe);                           // Ferme le tube (libère les ressources)

    if (result.find("Power State: On") != std::string::npos) return "on";   // Si le texte contient "On", retourne "on"
    if (result.find("Power State: Off") != std::string::npos) return "off"; // Si le texte contient "Off", retourne "off"
    return "ERROR";                         // Sinon retourne "ERROR"
}

void executeTapoCommand(const std::string& command) {
    const int maxAttempts = 3;             // Nombre maximum de tentatives autorisées
    int attempt = 0;                       // Compteur de tentatives

    while (attempt < maxAttempts) {        // Boucle de tentatives
        if (!checkConnection()) {          // Vérifie si la prise est joignable
            std::cerr << "[ERROR] Prise Tapo inaccessible (Tentative " << (attempt + 1) << ")\n"; // Affiche une erreur
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Attend 5 secondes avant de réessayer
            attempt++;                     // Incrémente la tentative
            continue;                      // Passe à la tentative suivante
        }

        std::string currentState = getTapoState();  // Récupère l’état actuel
        if (currentState == "ERROR") {     // Si l’état est inconnu
            std::cerr << "[ERROR] État actuel inconnu. Tentative " << (attempt + 1) << "\n"; // Affiche une erreur
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Attend avant de réessayer
            attempt++;
            continue;
        }

        if ((command == "on" && currentState == "off") || (command == "off" && currentState == "on")) {
            // Si la commande est cohérente avec un changement d’état
            std::cout << "[INFO] Changement : " << currentState << " -> " << command << "\n";

            if (command == "off") {
                std::this_thread::sleep_for(std::chrono::seconds(5));  // Attente de sécurité avant d’éteindre
                if (getTapoState() == "off") {                         // Si la prise est déjà éteinte
                    std::cout << "[INFO] Prise déjà éteinte, annulation.\n";
                    return;                                           // Ne fait rien
                }
            }

            std::string cmd = "tapoctl " + command + " " + IP + " " + USER + " " + PASS; // Construit la commande tapoctl
            int result = std::system(cmd.c_str());  // Exécute la commande

            if (result == 0) {
                std::cout << "[SUCCESS] Commande `" << command << "` exécutée.\n"; // Succès
                return;
            } else {
                std::cerr << "[ERROR] Commande `" << command << "` échouée.\n"; // Échec
            }
        } else {
            // Aucun changement nécessaire (déjà à l’état demandé)
            std::cout << "[INFO] Aucune action nécessaire. Prise déjà à l'état `" << currentState << "`.\n";
            return;
        }

        attempt++;                                       // Incrémente la tentative
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Pause avant de réessayer
    }

    // Échec total après plusieurs tentatives
    std::cerr << "[FATAL ERROR] Impossible d’exécuter `" << command << "` après " << maxAttempts << " tentatives.\n";
}

int isOdeurActiveNow() {
    try {
        sql::Driver* driver = get_driver_instance(); // Récupère le driver MySQL installé sur le système
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "horloge")); // Établit une connexion à la base locale avec login root
        conn->setSchema("projet_horloge"); // Sélectionne la base de données "projet_horloge"

        std::time_t now = std::time(nullptr); // Récupère l’heure système actuelle en timestamp
        std::tm* local = std::localtime(&now); // Convertit le timestamp en heure locale (struct tm)
        int minutesNow = local->tm_hour * 60 + local->tm_min; // Convertit l’heure actuelle en minutes depuis minuit
        int jourIndex = local->tm_wday == 0 ? 6 : local->tm_wday - 1; // Calcule l’indice du jour (0 = Lundi, 6 = Dimanche)

        // Construction de la requête SQL pour vérifier si une odeur est active
        std::string query =
            "SELECT odeur FROM evenements "
            "WHERE " + std::to_string(minutesNow) + " >= debut " // Heure actuelle >= début
            "AND " + std::to_string(minutesNow) + " < (debut + duree) " // Heure actuelle < fin
            "AND SUBSTRING(jour_semaine, " + std::to_string(jourIndex + 1) + ", 1) = '1' " // Vérifie que le jour est actif dans le champ binaire
            "AND id_utilisateur = 70 " // Ne prend que les événements de l’utilisateur 70
            "ORDER BY id DESC LIMIT 1"; // Ne récupère que la dernière ligne correspondante

        std::cout << "[SQL] Requête exécutée :\n" << query << "\n"; // Affiche la requête SQL dans le terminal pour debug

        std::unique_ptr<sql::Statement> stmt(conn->createStatement()); // Crée un objet statement
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query)); // Exécute la requête et stocke le résultat

        if (res->next()) { // Si une ligne a été trouvée
            int odeur = res->getInt("odeur"); // Récupère la valeur du champ "odeur"
            std::cout << "[BDD] Odeur active = " << odeur << "\n"; // Affiche le résultat dans la console
            return odeur; // Retourne la valeur (0 ou 1)
        } else {
            std::cout << "[BDD] Aucun événement actif à cette heure pour ce jour.\n"; // Aucun événement trouvé dans la plage
            return -1; // Retourne -1 pour signaler absence de correspondance
        }

    } catch (sql::SQLException& e) { // En cas d'erreur SQL
        std::cerr << "[SQL ERROR] " << e.what() << "\n"; // Affiche l’erreur SQL
        return -1; // Retourne -1 en cas d’échec
    }
}

class TapoCallback : public virtual mqtt::callback { // Déclaration d’une classe qui hérite de l’interface mqtt::callback
    std::chrono::steady_clock::time_point lastPresenceTime; // Stocke l’instant où la présence a été détectée
    bool presenceActuelle = false; // Indique si une présence est actuellement détectée (en cours de temporisation)

public:
    void message_arrived(mqtt::const_message_ptr msg) override { // Surcharge de la méthode appelée à la réception d’un message MQTT
        std::string payload = msg->get_payload_str(); // Récupère le contenu du message MQTT
        for (char& c : payload) c = tolower(c); // Convertit tout en minuscules pour standardiser la comparaison

        if (msg->get_topic() == TOPIC_PRESENCE) { // Si le message vient du topic de présence
            std::cout << "[MQTT] Présence détectée : " << payload << "\n"; // Affiche le message reçu dans la console

            if (payload == "presence detectee") { // Si le message indique qu’une présence est détectée
                if (!presenceActuelle) { // Si ce n’est pas encore une présence en cours
                    lastPresenceTime = std::chrono::steady_clock::now(); // Démarre le chrono de 10s
                    presenceActuelle = true; // Marque que la présence est active (début de la temporisation)
                    std::cout << "[INFO] Début de la temporisation de présence...\n"; // Message de debug
                } else {
                    auto now = std::chrono::steady_clock::now(); // Récupère l’heure actuelle
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPresenceTime).count(); // Calcule la durée écoulée depuis le début
                    if (duration >= 10) { // Si 10 secondes se sont écoulées avec détection continue
                        std::cout << "[INFO] Présence confirmée après 10s.\n"; // Affiche que la présence est confirmée
                        int odeur = isOdeurActiveNow(); // Vérifie si une odeur est nécessaire via la base de données
                        std::cout << "[DEBUG] Valeur retournée par isOdeurActiveNow() = " << odeur << "\n"; // Affiche la valeur retournée pour debug

                        if (odeur == 1) { // Si l’odeur est requise
                            std::cout << "[ACTION] Odeur requise -> activation du diffuseur.\n"; // Affiche l’action entreprise
                            executeTapoCommand("on"); // Active la prise via tapoctl
                            etatDiffuseur = true; // Met à jour l’état local
                        } else {
                            std::cout << "[INFO] Odeur non requise ou événement non valide -> extinction.\n"; // Sinon, on n’active pas
                            if (etatDiffuseur) { // Si la prise était allumée
                                executeTapoCommand("off"); // On l’éteint
                                etatDiffuseur = false; // Mise à jour de l’état
                            }
                        }
                        presenceActuelle = false; // Réinitialise l’état de présence
                    }
                }
            } else if (payload == "aucune presence") { // Si le message indique l'absence de présence
                presenceActuelle = false; // Réinitialise la présence
                if (etatDiffuseur) { // Si la prise était active
                    std::cout << "[INFO] Présence absente -> extinction du diffuseur.\n"; // Message console
                    executeTapoCommand("off"); // Éteint la prise
                    etatDiffuseur = false; // Mise à jour de l’état
                }
            }

        } else if (msg->get_topic() == TOPIC_DIFFUSEUR) { // Si le message est une commande manuelle
            std::cout << "[MQTT] Commande manuelle reçue : " << payload << "\n"; // Affiche la commande
            if (payload == "on") { // Si la commande est "on"
                executeTapoCommand("on"); // Allume la prise
                etatDiffuseur = true; // Met à jour l’état
            } else if (payload == "off") { // Si la commande est "off"
                executeTapoCommand("off"); // Éteint la prise
                etatDiffuseur = false; // Mise à jour
            } else {
                std::cerr << "[ERROR] Commande inconnue : " << payload << "\n"; // Si la commande n’est pas reconnue
            }
        }
    }
    void tick() {
        if (presenceActuelle) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPresenceTime).count();
            if (duration >= 10) {
                std::cout << "[INFO] Présence confirmée après 10s (vérification timer).\n";
                int odeur = isOdeurActiveNow();
                std::cout << "[DEBUG] Valeur retournée par isOdeurActiveNow() = " << odeur << "\n";
    
                if (odeur == 1) {
                    std::cout << "[ACTION] Odeur requise -> activation du diffuseur.\n";
                    executeTapoCommand("on");
                    etatDiffuseur = true;
                } else {
                    std::cout << "[INFO] Odeur non requise ou événement non valide -> extinction.\n";
                    if (etatDiffuseur) {
                        executeTapoCommand("off");
                        etatDiffuseur = false;
                    }
                }
                presenceActuelle = false; // Réinitialise
            }
        }
    }    
};

int main() {
    mqtt::async_client client(ADDRESS, CLIENT_ID); // Crée un client MQTT asynchrone avec l'adresse du broker et un ID unique
    TapoCallback cb;                               // Instancie l'objet callback qui gérera les messages entrants
    client.set_callback(cb);                       // Associe le callback au client MQTT

    mqtt::connect_options connOpts;                // Crée un objet d’options de connexion
    connOpts.set_clean_session(true);              // Spécifie qu’on veut une session “propre” (pas de persistance des abonnements précédents)

    try {
        std::cout << "[MQTT] Connexion au broker...\n";     // Affiche que la connexion est en cours
        client.connect(connOpts)->wait();                   // Tente la connexion et attend qu’elle réussisse
        std::cout << "[MQTT] Connecté !\n";                 // Affiche le succès

        client.subscribe(TOPIC_DIFFUSEUR, 1)->wait();       // S’abonne au topic "diffuseur" (qualité QoS 1)
        client.subscribe(TOPIC_PRESENCE, 1)->wait();        // S’abonne au topic "presence" (qualité QoS 1)

        while (true) {                                      // Boucle infinie principale
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Attente passive de 1 seconde
            cb.tick();  // Appelle la vérification de présence chaque seconde
            // Aucun traitement ici : tout est piloté via le callback asynchrone
        }

        client.disconnect()->wait();                        // Déconnexion propre (non atteinte car boucle infinie)

    } catch (const mqtt::exception& exc) {                  // Gestion des erreurs de connexion MQTT
        std::cerr << "[MQTT] Erreur : " << exc.what() << "\n"; // Affiche l’erreur
        return 1;                                           // Quitte le programme avec une erreur
    }

    return 0; // Fin normale du programme (jamais atteinte ici)
}
