<?php
// Configuration de la base de données
define('DB_HOST', 'localhost');     // L'hôte de la base de données
define('DB_NAME', 'affichage_horloge'); // Le nom de la base de données
define('DB_USER', 'root');   // L'utilisateur de la base de données
define('DB_PASS', 'horloge');    // Le mot de passe de la base de données

// Définir le mode d'erreur PDO
define('DB_OPTIONS', [
    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
    PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
    PDO::ATTR_EMULATE_PREPARES => false,
]);
?>