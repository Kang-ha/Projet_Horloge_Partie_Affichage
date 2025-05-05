/**
 * ===========================
 *     test_presence.h
 * ===========================
 * 
 * Ce fichier déclare la fonction `testPresence()`, 
 * utilisée pour tester le capteur de présence connecté à la carte.
 * 
 * La logique complète de cette fonction est définie dans `test_presence.cpp`.
 * 
 * Ce header permet d’utiliser cette fonction dans d’autres fichiers
 * comme `M5Stack_Core2.ino`, sans avoir besoin de connaître les détails internes.
 */

#ifndef TEST_PRESENCE_H     // Protection contre inclusion multiple
#define TEST_PRESENCE_H

// Déclaration de la fonction de test du capteur de présence
void testPresence();

#endif  // Fin du bloc d'inclusion
