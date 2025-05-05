/**
 * ===================
 *       main.h
 * ===================
 * 
 * Ce fichier déclare la fonction `startDiffuseur()`, 
 * qui lance le comportement principal du système olfactif après
 * la phase d’accueil ou de temporisation.
 * 
 * La définition de cette fonction se trouve dans `main.cpp`.
 * Ce header permet à d'autres fichiers comme `M5Stack_Core2.ino`
 * d'utiliser `startDiffuseur()` sans avoir à inclure `main.cpp` directement.
 */

#ifndef MAIN_H       // Empêche les inclusions multiples
#define MAIN_H

// Déclaration de la fonction qui démarre la logique principale du diffuseur
void startDiffuseur();

#endif  // Fin de la protection d'inclusion
