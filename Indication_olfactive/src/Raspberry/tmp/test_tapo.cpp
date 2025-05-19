#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <array>
#include <unistd.h> // Pour sleep() sur Linux

// Informations de connexion
const std::string IP = "192.168.1.102";
const std::string USER = "boutin.l@stjolorient.fr";
const std::string PASS = "Voso1295";

// Fonction pour récupérer l'état actuel de la prise
std::string getTapoState() {
    std::string cmd = "tapoctl info " + IP + " " + USER + " " + PASS + " | grep 'Power State'";
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        return "ERROR";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);

    // Vérifie si la prise est ON ou OFF
    if (result.find("Power State: On") != std::string::npos) {
        return "ON";
    } else if (result.find("Power State: Off") != std::string::npos) {
        return "OFF";
    } else {
        return "ERROR";
    }
}

int main() {
    std::string state = getTapoState();
    if (state == "ERROR") {
        std::cerr << "Erreur lors de la récupération de l'état de la prise !" << std::endl;
        return 1;
    }

    std::cout << "État actuel de la prise : " << state << std::endl;

    if (state == "OFF") {
        std::cout << "Allumage de la prise..." << std::endl;
        std::system(("tapoctl on " + IP + " " + USER + " " + PASS).c_str());
    } else if (state == "ON") {
        std::cout << "Extinction de la prise après 10 secondes..." << std::endl;
        sleep(10); // ? Maintenant, cette ligne fonctionne
        std::system(("tapoctl off " + IP + " " + USER + " " + PASS).c_str());
    }

    return 0;
}
