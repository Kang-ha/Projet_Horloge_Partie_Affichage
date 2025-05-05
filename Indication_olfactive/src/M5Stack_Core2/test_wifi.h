/**
 * =====================
 *     test_wifi.h
 * =====================
 * 
 * Ce fichier déclare la fonction `testWifi()`, utilisée pour tester
 * la connexion WiFi du M5Stack. 
 * 
 * La définition complète de la fonction se trouve dans `test_wifi.cpp`.
 * 
 * Ce header permet à d'autres fichiers (comme `M5Stack_Core2.ino`)
 * d’appeler `testWifi()` sans inclure directement le `.cpp`.
 */

#ifndef TEST_WIFI_H   // Empêche les inclusions multiples
#define TEST_WIFI_H

// Déclaration de la fonction de test WiFi (implémentée ailleurs)
void testWifi();

#endif  // FIN de la protection contre les inclusions multiples
