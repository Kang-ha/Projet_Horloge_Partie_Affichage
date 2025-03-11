<?php
    $serveurNom = "localhost:3306";
    $utilisateur = "admineleve";
    $motDePasse = "ieufdl";
    $dbNom = "projet_horloge";
    $conn = new mysqli($serveurNom, $utilisateur, $motDePasse, $dbNom);
    if ($conn->connect_error) {
        die("Connexion échouée: " . $conn->connect_error);
        console.log("Connexion échouée: " . $conn->connect_error);
    }
?>