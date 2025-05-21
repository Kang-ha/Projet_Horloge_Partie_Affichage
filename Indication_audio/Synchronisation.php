<?php
/**
 * Script de synchronisation entre projet_horloge (maître) et projet_horloge (esclave)
 * Avec gestion robuste des erreurs de connexion
 */

// Configuration détaillée
$config = [
    'source' => [
        'host' => '192.168.1.143', // Adresse du serveur maître
        'port' => 3306,        // Port MySQL
        'username' => 'admineleve',
        'password' => 'ieufdl',
        'database' => 'projet_horloge',
        'charset' => 'utf8mb4'
    ],
    'target' => [
        'host' => '192.168.1.117', // Adresse du serveur esclave
        'port' => 3306,
        'username' => 'root',
        'password' => 'horloge',
        'database' => 'projet_horloge',
        'charset' => 'utf8mb4'
    ],
    'sync_rules' => [
        'pictogrammes' => [
            'strategy' => 'full',
            'key_field' => 'id'
        ],
        'evenements' => [
            'strategy' => 'partial',
            'filter_field' => 'id_utilisateur',
            'filter_value' => 70,
            'key_field' => 'id'
        ],
        'periodes' => [
            'strategy' => 'partial',
            'filter_field' => 'id_configuration',
            'filter_value' => 9,
            'key_field' => 'id'
        ]
    ],
    'options' => [
        'create_database' => true, // Crée la BDD si elle n'existe pas
        'create_tables' => true,   // Crée les tables si elles n'existent pas
        'max_retries' => 3,        // Nombre de tentatives de connexion
        'retry_delay' => 2         // Délai entre les tentatives en secondes
    ]
];

// Journalisation
$logFile = __DIR__ . '/sync_log_' . date('Y-m-d') . '.log';
function logMessage($message) {
    global $logFile;
    $timestamp = date('[Y-m-d H:i:s]');
    file_put_contents($logFile, "$timestamp $message\n", FILE_APPEND);
    echo "$message\n";
}

// Fonction de connexion robuste
function connectWithRetry($host, $port, $username, $password, $dbname = null, $options = []) {
    $maxRetries = $options['max_retries'] ?? 3;
    $retryDelay = $options['retry_delay'] ?? 2;
    
    $dsn = "mysql:host=$host;port=$port;";
    if ($dbname) $dsn .= "dbname=$dbname;";
    $dsn .= "charset=" . ($options['charset'] ?? 'utf8mb4');
    
    for ($attempt = 1; $attempt <= $maxRetries; $attempt++) {
        try {
            $pdo = new PDO($dsn, $username, $password, [
                PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
                PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
                PDO::ATTR_EMULATE_PREPARES => false
            ]);
            logMessage("Connexion réussie à  $host (tentative $attempt/$maxRetries)");
            return $pdo;
        } catch (PDOException $e) {
            logMessage("échec de connexion (tentative $attempt/$maxRetries): " . $e->getMessage());
            if ($attempt < $maxRetries) {
                sleep($retryDelay);
            } else {
                throw $e;
            }
        }
    }
}

try {
    logMessage("Début de la synchronisation");
    
    // étape 1: Connexion au serveur source
    $sourceDb = connectWithRetry(
        $config['source']['host'],
        $config['source']['port'],
        $config['source']['username'],
        $config['source']['password'],
        $config['source']['database'],
        ['charset' => $config['source']['charset']]
    );

    // étape 2: Connexion au serveur cible (avec création de la BDD si nécessaire)
    try {
        $targetDb = connectWithRetry(
            $config['target']['host'],
            $config['target']['port'],
            $config['target']['username'],
            $config['target']['password'],
            $config['target']['database'],
            ['charset' => $config['target']['charset']]
        );
    } catch (PDOException $e) {
        if ($config['options']['create_database'] && strpos($e->getMessage(), 'Unknown database') !== false) {
            logMessage("La base de données cible n'existe pas, tentative de création...");
            $targetServer = connectWithRetry(
                $config['target']['host'],
                $config['target']['port'],
                $config['target']['username'],
                $config['target']['password']
            );
            $targetServer->exec("CREATE DATABASE IF NOT EXISTS `{$config['target']['database']}` CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci");
            logMessage("Base de données '{$config['target']['database']}' crée avec succès");
            
            // Reconnexion à  la nouvelle base
            $targetDb = connectWithRetry(
                $config['target']['host'],
                $config['target']['port'],
                $config['target']['username'],
                $config['target']['password'],
                $config['target']['database']
            );
        } else {
            throw $e;
        }
    }

    // étape 3: Vérification/création des tables
    if ($config['options']['create_tables']) {
        foreach ($config['sync_rules'] as $tableName => $rule) {
            try {
                // Vérification simple de l'existence de la table
                $targetDb->query("SELECT 1 FROM `$tableName` LIMIT 1");
                logMessage("Table $tableName existe déjà ");
            } catch (PDOException $e) {
                // Si la table n'existe pas, on récupère la structure depuis la source
                logMessage("Table $tableName n'existe pas, tentative de création...");
                
                $createTableSql = $sourceDb->query("SHOW CREATE TABLE `$tableName`")->fetch()['Create Table'];
                $targetDb->exec($createTableSql);
                logMessage("Table $tableName créée avec succès");
            }
        }
    }

    // étape 4: Synchronisation des données
    foreach ($config['sync_rules'] as $tableName => $rule) {
        logMessage("Début synchronisation table: $tableName");
        
        // Préparation de la requête source
        $sourceQuery = "SELECT * FROM `$tableName`";
        $params = [];
        
        if ($rule['strategy'] === 'partial') {
            $sourceQuery .= " WHERE `{$rule['filter_field']}` = :filter_value";
            $params[':filter_value'] = $rule['filter_value'];
        }
        
        $sourceStmt = $sourceDb->prepare($sourceQuery);
        $sourceStmt->execute($params);
        
        // Récupération des champs
        $fields = [];
        for ($i = 0; $i < $sourceStmt->columnCount(); $i++) {
            $col = $sourceStmt->getColumnMeta($i);
            $fields[] = $col['name'];
        }
        
        // Préparation requête d'insertion
        $placeholders = implode(', ', array_fill(0, count($fields), '?'));
        $insertQuery = "INSERT INTO `$tableName` (`" . implode('`, `', $fields) . "`) 
                       VALUES ($placeholders) ON DUPLICATE KEY UPDATE ";
        
        $updates = [];
        foreach ($fields as $field) {
            if ($field !== $rule['key_field']) {
                $updates[] = "`$field` = VALUES(`$field`)";
            }
        }
        $insertQuery .= implode(', ', $updates);
        
        $targetStmt = $targetDb->prepare($insertQuery);
        
        // Traitement par lots pour les grosses tables
        $batchSize = 1000;
        $count = 0;
        
        // Liste des IDs à conserver
        $idsToKeep = [];
        
        $targetDb->beginTransaction();
        while ($row = $sourceStmt->fetch(PDO::FETCH_ASSOC)) {
            $targetStmt->execute(array_values($row));
            $idsToKeep[] = $row[$rule['key_field']];
            $count++;
            
            if ($count % $batchSize === 0) {
                $targetDb->commit();
                logMessage("... $count lignes traitées");
                $targetDb->beginTransaction();
            }
        }
        $targetDb->commit();
        
        // Suppression des entrées qui n'existent plus dans la source
        if ($tableName === 'evenements' && !empty($idsToKeep)) {
            try {
                $placeholders = implode(',', array_fill(0, count($idsToKeep), '?'));
                
                $deleteQuery = "DELETE FROM `evenements` 
                               WHERE `{$rule['filter_field']}` = ? 
                               AND `{$rule['key_field']}` NOT IN ($placeholders)";
                
                $deleteParams = array_merge([$rule['filter_value']], $idsToKeep);
                
                $deleteStmt = $targetDb->prepare($deleteQuery);
                $deletedCount = $deleteStmt->execute($deleteParams) ? $deleteStmt->rowCount() : 0;
                
                logMessage("Suppression de $deletedCount événements obsolètes");
            } catch (PDOException $e) {
                logMessage("ERREUR suppression événements: " . $e->getMessage());
            }
        }
        logMessage("Synchronisation $tableName terminée: $count lignes transférées");
    }
    
    logMessage("Synchronisation complétée avec succès");
    exit(0);
    
} catch (PDOException $e) {
    logMessage("ERREUR CRITIQUE: " . $e->getMessage());
    logMessage("Trace: " . $e->getTraceAsString());
    exit(1);
} catch (Exception $e) {
    logMessage("ERREUR: " . $e->getMessage());
    exit(1);
}