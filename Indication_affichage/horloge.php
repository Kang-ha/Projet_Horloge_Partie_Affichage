<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Horloge</title>
    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
    <script src="horloge1.js"></script>
    <link rel="stylesheet" href="horloge.css">
</head>
<body>
    <div class="clock-container">
        <!-- Conteneur principal relatif -->
        <div class="clock-face">
            <canvas id="camembert" width="460" height="460"></canvas>
            <div class="analog-clock">
                <div class="hand hour-hand"></div>
                <div class="hand minute-hand"></div>
                <div class="hand second-hand"></div>
                <div class="center-dot"></div>
            </div>
            
            <!-- Conteneur des pictogrammes (position absolue par rapport à clock-face) -->
            <div id="pictogrammes-container"></div>
        </div>
        
        <div class="digital-clock">--:--:--</div>
        <div class="date-display">--/--/----</div>
        <div class="day-container">
            <div class="day-box" id="day-0">Lun</div>
            <div class="day-box" id="day-1">Mar</div>
            <div class="day-box" id="day-2">Mer</div>
            <div class="day-box" id="day-3">Jeu</div>
            <div class="day-box" id="day-4">Ven</div>
            <div class="day-box" id="day-5">Sam</div>
            <div class="day-box" id="day-6">Dim</div>
        </div>
    </div>
</body>
</html>