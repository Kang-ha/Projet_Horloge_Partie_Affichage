#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

void attendre_heure(const std::string& heure) {
    struct tm now_tm;
    time_t now = time(0);
    localtime_r(&now, &now_tm);
    
    int heure_cible, minute_cible;
    sscanf(heure.c_str(), "%d:%d", &heure_cible, &minute_cible);

    // Convertir l'heure actuelle en secondes
    int maintenant_sec = now_tm.tm_hour * 3600 + now_tm.tm_min * 60 + now_tm.tm_sec;
    int cible_sec = heure_cible * 3600 + minute_cible * 60;

    // Calcul du délai
    int attente_sec = cible_sec - maintenant_sec;
    if (attente_sec > 0) {
        std::cout << "Attente de " << attente_sec << " secondes jusqu'à " << heure << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(attente_sec));
    }
}

int main() {
    std::ifstream file("synthese.txt");
    if (!file) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier synthese.txt" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string heure, texte;

        if (std::getline(iss, heure, '|') && std::getline(iss, texte)) {
            attendre_heure(heure);
            
            // Lancer la synthèse vocale
            std::string command = "espeak-ng -v fr \"" + texte + "\"";
            std::cout << "Lecture : " << texte << std::endl;
            system(command.c_str());
        }
    }

    return 0;
}