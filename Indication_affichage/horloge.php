<?php
    // Inclut le fichier de connexion à la base de données
    require_once("bdd.php");
    // Établit la connexion à la base de données
    bddConnect();
?>
<!DOCTYPE html>
<html lang="en">
    <head>
        <!-- Définit l'encodage des caractères -->
        <meta charset="UTF-8">
        <!-- Configuration pour le responsive design -->
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <!-- Titre de la page -->
        <title>Horloge</title>
        <!-- Inclusion de Chart.js pour les graphiques -->
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <!-- Inclusion de jQuery pour les fonctionnalités AJAX -->
        <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
        <!-- Lien vers la feuille de style CSS -->
        <link href="horloge.css?" rel="stylesheet" type="text/css">
        <!-- Icône de la page -->
        <link rel="icon" type="image/x-icon" href="horloge.png">
    </head>
    <body>
        <!-- Conteneur principal de l'horloge -->
        <div class="horloge-container">
            <!-- Face de l'horloge -->
            <div class="horloge-face">
                <!-- Canvas pour le graphique en anneau -->
                <canvas id="myDoughnutChart"></canvas>
                <!-- Canvas pour le graphique en camembert -->
                <canvas id="myPieChart"></canvas>
                <!-- Inclusion du script JavaScript avec un numéro aléatoire pour éviter la mise en cache -->
                <script src="horloge.js?<?php $randomNumber = rand(1, 100); echo $randomNumber;?>"></script>
        
                <!-- Aiguilles de l'horloge et point central -->
                <div class="aiguille aiguille-heure"></div>
                <div class="aiguille aiguille-minute"></div>
                <div class="aiguille aiguille-seconde"></div>
                <div class="centre-point"></div>
                
                <!-- Conteneur pour afficher les pictogrammes -->
                <div id="afficherPictogrammes">
                    <?php afficherPictogrammes(); ?>
                </div>
            </div>
            <!-- Appel à la fonction PHP pour afficher l'horloge -->
            <?php afficherHorloge(); ?>

            <!-- Affichage de l'heure digitale -->
            <div class="horloge-digital">--:--:--</div>

            <!-- Affichage de la date -->
            <div class="affichage-date">--/--/----</div>

            <!-- Conteneur des jours de la semaine -->
            <div class="jour-container">
                <div class="jour-box" id="jour-0">Lun</div>
                <div class="jour-box" id="jour-1">Mar</div>
                <div class="jour-box" id="jour-2">Mer</div>
                <div class="jour-box" id="jour-3">Jeu</div>
                <div class="jour-box" id="jour-4">Ven</div>
                <div class="jour-box" id="jour-5">Sam</div>
                <div class="jour-box" id="jour-6">Dim</div>
            </div>
            <!-- Cache pour l'effet de progression du temps -->
            <div class="cache-horloge" id="cacheHorloge"></div>
        </div>
    </body>
</html>