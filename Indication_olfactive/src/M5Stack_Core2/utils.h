/**
 * ===================
 *       utils.h
 * ===================
 * 
 * Ce fichier déclare des fonctions utilitaires pour le projet.
 * 
 * Actuellement, il contient :
 * - `normalizeText(const char*)` : une fonction pour convertir les caractères spéciaux 
 *    (accents, etc.) en une forme compatible avec l’écran M5Stack.
 * 
 * Cette fonction est définie dans `utils.cpp`.
 */

#ifndef UTILS_H        // Protection contre inclusions multiples
#define UTILS_H

#include <M5Core2.h>   // Librairie M5Stack nécessaire pour types et fonctions LCD

// 🔤 Fonction de normalisation de texte pour affichage
String normalizeText(const char* text);

#endif  // Fin de la protection d’inclusion
