/*
 * ===================
 *      utils.cpp
 * ===================
 *
 * Ce fichier contient des fonctions utilitaires communes à tout le projet.
 * 
 * Fonction principale :
 * - normalizeText(const char*) : remplace les caractères accentués par des équivalents sans accent,
 *   pour garantir une compatibilité optimale avec l’affichage sur l’écran M5Stack.
 *
 * Exemples remplacés :
 * é → e, è → e, à → a, ç → c, etc.
 */

#include "utils.h"  // Inclusion du header associé pour déclaration de normalizeText()

// Fonction de normalisation du texte pour affichage compatible M5Stack
String normalizeText(const char* text) {
    String normalized = String(text);  // Convertit le texte en objet String

    // Remplacements manuels des caractères accentués par leurs équivalents simples
    normalized.replace("é", "e");
    normalized.replace("è", "e");
    normalized.replace("ê", "e");
    normalized.replace("ë", "e");
    normalized.replace("à", "a");
    normalized.replace("â", "a");
    normalized.replace("ä", "a");
    normalized.replace("ô", "o");
    normalized.replace("ö", "o");
    normalized.replace("î", "i");
    normalized.replace("ï", "i");
    normalized.replace("û", "u");
    normalized.replace("ü", "u");
    normalized.replace("ç", "c");

    return normalized;  // Retourne la chaîne normalisée
}