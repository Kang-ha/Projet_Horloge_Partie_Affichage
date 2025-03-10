<?php
    ignore_user_abort(true);
    set_time_limit(0);
    $serveurNom = "localhost:3306";
    $utilisateur = "admineleve";
    $motDePasse = "ieufdl";
    $dbNom = "projet_horloge";
    $conn = new mysqli($serveurNom, $utilisateur, $motDePasse, $dbNom);
    if ($conn->connect_error) {
        die("Connexion échouée: " . $conn->connect_error);
        console.log("Connexion échouée: " . $conn->connect_error);
    }
    sleep(10);
    file_put_contents("log.txt", "Exécution terminée\n", FILE_APPEND);
?>