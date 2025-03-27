<?php
$servername = "localhost"; // Adresse du serveur MySQL
$username = "root";        // Utilisateur MySQL
$password = "horloge";            // Mot de passe 
$dbname = "affichage_horloge";    // Nom de la base de données

// Connexion à MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// Vérifier la connexion
if ($conn->connect_error) {
    die("Échec de connexion: " . $conn->connect_error);
}

// Requête pour récupérer le texte et l'heure de début
$sql = "SELECT commentaire, debut FROM evenements ORDER BY debut ASC";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // Écrire les données dans un fichier texte
    $file = fopen("synthese.txt", "w");
    
    while($row = $result->fetch_assoc()) {
        fwrite($file, $row["debut"] . "|" . $row["commentaire"] . "\n");
    }
    
    fclose($file);
} else {
    echo "Aucune donnée trouvée.";
}

$conn->close();
?>