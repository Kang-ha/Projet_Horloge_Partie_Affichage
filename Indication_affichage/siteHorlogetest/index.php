<?php
    require_once("bdd.php"); 
    bddConnect();
?>
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Horloge</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
        <link href="horloge.css" rel="stylesheet" type="text/css">
    </head>
    <body>
        <div class="horloge-container">
            <div class="horloge-face">
                <canvas id="myDoughnutChart" width="400" height="400" style="position: absolute"></canvas>
                <canvas id="myPieChart" width="400" height="400"></canvas>
                <script src="horloge.js?<?php $randomNumber = rand(1, 100); echo $randomNumber;?>"></script>
        
                <!-- Aiguilles et point central -->
                <div class="aiguille aiguille-heure"></div>
                <div class="aiguille aiguille-minute"></div>
                <div class="aiguille aiguille-seconde"></div>
                <div class="centre-point"></div>
                
                <div id="afficherPictogrammes">
                    <?php afficherPictogrammes(); ?>
                </div>
            </div>
            <?php afficherHorloge(); ?>

            <!-- Horloge digitale -->
            <div class="horloge-digital">--:--:--</div>

            <!-- Date -->
            <div class="affichage-date">--/--/----</div>

            <!-- Jours de la semaine -->
            <div class="jour-container">
                <div class="jour-box" id="jour-0">Lun</div>
                <div class="jour-box" id="jour-1">Mar</div>
                <div class="jour-box" id="jour-2">Mer</div>
                <div class="jour-box" id="jour-3">Jeu</div>
                <div class="jour-box" id="jour-4">Ven</div>
                <div class="jour-box" id="jour-5">Sam</div>
                <div class="jour-box" id="jour-6">Dim</div>
            </div>
            <div class="cache-horloge" id="cacheHorloge"></div>
        </div>
        
        <script>
            // Mise à jour toutes les 5 secondes sans rechargement
            setInterval(function(){
                afficherHorloge(); // Appelle votre fonction existante
            }, 5000);

            // Initialisation au chargement
            document.addEventListener('DOMContentLoaded', function() {
                // Votre code d'initialisation existant...
                setTimeout(afficherHorloge, 100); // Premier appel rapide
            });
        </script>
    </body>
</html>