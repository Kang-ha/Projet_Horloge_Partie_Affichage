/**
 * ===========================
 *     test_diffuseur.h
 * ===========================
 * 
 * Ce fichier déclare la fonction `testDiffuseur()`,
 * qui permet de tester l'envoi d'une commande vers le diffuseur
 * via le topic MQTT associé (`topicDiffuseur`).
 * 
 * La logique de cette fonction est définie dans `test_diffuseur.cpp`.
 * Ce header permet de rendre la fonction accessible à d'autres fichiers
 * comme `M5Stack_Core2.ino`.
 */

#ifndef TEST_DIFFUSEUR_H     // Empêche les inclusions multiples
#define TEST_DIFFUSEUR_H

// Déclaration de la fonction de test du diffuseur
void testDiffuseur();

#endif  // Fin de la protection d'inclusion
