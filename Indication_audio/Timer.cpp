#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <ctime>

void log_message(const std::string& message) {
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", std::localtime(&now));
    std::cout << timestamp << " " << message << std::endl;
}

void run_php_script() {
    const char* php_script = "/home/admineleve/Documents/Projet_Horloge_Partie_Affichage/Indication_audio/Synchronisation.php";
    int result = std::system(("php " + std::string(php_script)).c_str());
    
    if (result != 0) {
        log_message("Erreur d'exécution du script PHP (code: " + std::to_string(result) + ")");
    }
}

int main() {
    const int interval_minutes = 60; // passage à une heure
    const std::chrono::minutes interval(interval_minutes);
    
    log_message("Démarrage du scheduler (intervalle: " + std::to_string(interval_minutes) + " minutes)");

    while (true) {
        auto start = std::chrono::system_clock::now();
        
        log_message("Exécution du script PHP...");
        run_php_script();
        
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        auto sleep_time = interval - std::chrono::duration_cast<std::chrono::minutes>(elapsed);

        if (sleep_time.count() > 0) {
            log_message("Prochaine exécution dans " + 
                       std::to_string(std::chrono::duration_cast<std::chrono::seconds>(sleep_time).count()) + 
                       " secondes");
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
        } else {
            log_message("Attention: Le script a pris plus de temps que l'intervalle!");
        }
    }

    return 0;
}
