<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Définition du fuseau horaire
date_default_timezone_set('Europe/Paris');

header('Content-Type: application/json');
header('Cache-Control: no-cache, must-revalidate');
header('Access-Control-Allow-Origin: *');

try {
    $response = array(
        'timestamp' => time(),
        'formatted_time' => date('Y-m-d H:i:s'),
        'timezone' => date_default_timezone_get(),
        'status' => 'success'
    );
    
    echo json_encode($response);
} catch (Exception $e) {
    http_response_code(500);
    echo json_encode(array(
        'status' => 'error',
        'message' => $e->getMessage()
    ));
}
?>