<?php
// Configuration des bases de données
$sourceDbConfig = [
    'host' => '192.168.1.117',
    'username' => 'admineleve',
    'password' => 'ieufdl',
    'database' => 'projet_horloge'
];

$targetDbConfig = [
    'host' => '192.168.1.143',
    'username' => 'root',
    'password' => 'horloge',
    'database' => 'affichage_horloge'
];

// ID spécifiques pour le filtrage
$userId = 70; // Pour la table evenements
$configId = 9; // Pour la table periodes

try {
    // Connexion aux bases de données
    $sourceDb = new PDO(
        "mysql:host={$sourceDbConfig['host']};dbname={$sourceDbConfig['database']}",
        $sourceDbConfig['username'],
        $sourceDbConfig['password']
    );
    $sourceDb->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $targetDb = new PDO(
        "mysql:host={$targetDbConfig['host']};dbname={$targetDbConfig['database']}",
        $targetDbConfig['username'],
        $targetDbConfig['password']
    );
    $targetDb->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Démarrer une transaction
    $targetDb->beginTransaction();

    // 1. Synchronisation de la table pictogrammes (complète)
    echo "Synchronisation de la table pictogrammes...\n";
    $query = "SELECT * FROM pictogrammes";
    $stmt = $sourceDb->query($query);
    $pictogrammes = $stmt->fetchAll(PDO::FETCH_ASSOC);

    if (count($pictogrammes) > 0) {
        // Vider la table cible avant synchronisation complète
        $targetDb->exec("TRUNCATE TABLE pictogrammes");
        
        $fields = array_keys($pictogrammes[0]);
        $placeholders = implode(', ', array_fill(0, count($fields), '?'));
        $insertQuery = "INSERT INTO pictogrammes (" . implode(', ', $fields) . ") VALUES ($placeholders)";
        $insertStmt = $targetDb->prepare($insertQuery);

        foreach ($pictogrammes as $row) {
            $insertStmt->execute(array_values($row));
        }
        echo count($pictogrammes) . " pictogrammes synchronisés.\n";
    }

    // 2. Synchronisation de la table evenements (seulement id = 70)
    echo "\nSynchronisation de la table evenements pour id = $userId...\n";
    $query = "SELECT * FROM evenements WHERE id = :id";
    $stmt = $sourceDb->prepare($query);
    $stmt->bindParam(':id', $userId, PDO::PARAM_INT);
    $stmt->execute();
    $evenements = $stmt->fetchAll(PDO::FETCH_ASSOC);

    if (count($evenements) > 0) {
        $fields = array_keys($evenements[0]);
        $updateParts = array_map(function($field) { return "$field = VALUES($field)"; }, $fields);
        
        $insertQuery = "INSERT INTO evenements (" . implode(', ', $fields) . ") 
                        VALUES (:" . implode(', :', $fields) . ")
                        ON DUPLICATE KEY UPDATE " . implode(', ', $updateParts);
        
        $insertStmt = $targetDb->prepare($insertQuery);

        foreach ($evenements as $row) {
            foreach ($row as $key => $value) {
                $insertStmt->bindValue(":$key", $value);
            }
            $insertStmt->execute();
        }
        echo count($evenements) . " evenements synchronisés pour l'id $userId.\n";
    }

    // 3. Synchronisation de la table periodes (seulement id_configuration = 9)
    echo "\nSynchronisation de la table periodes pour id_configuration = $configId...\n";
    $query = "SELECT * FROM periodes WHERE id_configuration = :id_config";
    $stmt = $sourceDb->prepare($query);
    $stmt->bindParam(':id_config', $configId, PDO::PARAM_INT);
    $stmt->execute();
    $periodes = $stmt->fetchAll(PDO::FETCH_ASSOC);

    if (count($periodes) > 0) {
        // Supprimer les anciennes périodes pour cette configuration
        $deleteStmt = $targetDb->prepare("DELETE FROM periodes WHERE id_configuration = :id_config");
        $deleteStmt->bindParam(':id_config', $configId, PDO::PARAM_INT);
        $deleteStmt->execute();

        $fields = array_keys($periodes[0]);
        $placeholders = implode(', ', array_fill(0, count($fields), '?'));
        $insertQuery = "INSERT INTO periodes (" . implode(', ', $fields) . ") VALUES ($placeholders)";
        $insertStmt = $targetDb->prepare($insertQuery);

        foreach ($periodes as $row) {
            $insertStmt->execute(array_values($row));
        }
        echo count($periodes) . " periodes synchronisées pour la configuration $configId.\n";
    }

    // Valider la transaction
    $targetDb->commit();
    echo "\nSynchronisation terminée avec succès.\n";

} catch (PDOException $e) {
    // En cas d'erreur, annuler la transaction
    if (isset($targetDb) && $targetDb->inTransaction()) {
        $targetDb->rollBack();
    }
    echo "Erreur lors de la synchronisation: " . $e->getMessage() . "\n";
    exit(1);
}

// Fermer les connexions
$sourceDb = null;
$targetDb = null;
?>