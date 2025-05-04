// Attendre que le document HTML soit complètement chargé
$(document).ready(function() {
    // Afficher un message dans la console pour vérifier que le script démarre
    console.log("Document ready"); // Vérification que le script démarre

    // Fonction pour positionner les nombres sur l'horloge analogique
    function setupClockNumbers() {
        // Sélectionner l'élément de l'horloge analogique
        const clockFace = $('.analog-clock');
        // Définir le rayon pour positionner les nombres des heures
        const radius = 195; // Rayon pour les heures
    
        // Supprimer les anciens nombres d'heures s'ils existent
        $('.hour-number').remove();
    
        // Boucle pour ajouter les nombres de 1 à 24 autour de l'horloge
        for (let i = 1; i <= 24; i++) {
            // Calculer l'angle pour chaque heure (15 degrés par heure)
            const angle = (i * 15) - 90; // 15 degrés par heure (360/24)
            // Convertir l'angle en radians
            const radian = (angle * Math.PI) / 180;
            
            // Calculer les positions x et y en utilisant la trigonométrie
            const x = Math.cos(radian) * radius;
            const y = Math.sin(radian) * radius;
    
            // Créer un élément div pour le nombre
            const numberDiv = $('<div>')
                // Ajouter la classe CSS
                .addClass('hour-number')
                // Définir le texte (le numéro de l'heure)
                .text(i)
                // Positionner absolument l'élément
                .css({
                    'left': `${220 + x}px`, // 220 est la moitié de la largeur de l'horloge (440/2)
                    'top': `${220 + y}px`   // 220 est la moitié de la hauteur de l'horloge (440/2)
                });
    
            // Ajouter le nombre à l'horloge
            clockFace.append(numberDiv);
        }
    }
    
    // Fonction pour configurer les marqueurs des minutes
    function setupMinuteMarkers() {
        // Sélectionner l'élément de l'horloge analogique
        const clockFace = $('.analog-clock');
        // Définir un rayon plus grand que celui des heures
        const radius = 230; // Un peu plus grand que le rayon des heures
        
        // Supprimer les anciens marqueurs s'ils existent
        $('.minute-marker, .minute-number').remove();
    
        // Boucle pour les 60 minutes
        for (let i = 0; i < 60; i++) {
            // Calculer l'angle pour chaque minute (6 degrés par minute)
            const angle = (i * 6) - 90; // 6 degrés par minute (360/60)
            // Convertir en radians
            const radian = (angle * Math.PI) / 180;
    
            // Créer un marqueur pour chaque minute
            const minuteMarker = $('<div>')
                .addClass('minute-marker')
                // Positionner et faire pivoter le marqueur
                .css({
                    'transform': `rotate(${angle + 90}deg)`,
                    'left': '219px',
                    'top': '-5px'
                });
    
            // Pour chaque 5 minutes (marques principales)
            if (i % 5 === 0) {
                // Définir un rayon plus grand pour les nombres
                const numberRadius = radius + 15;
                // Calculer la position du nombre
                const numberX = Math.cos(radian) * numberRadius;
                const numberY = Math.sin(radian) * numberRadius;
                
                // Créer un élément pour le nombre
                const minuteNumber = $('<div>')
                    .addClass('minute-number')
                    .text(i)
                    .css({
                        'left': `${220 + numberX}px`,
                        'top': `${220 + numberY}px`
                    });
    
                // Ajouter le nombre à l'horloge
                clockFace.append(minuteNumber);
                // Rendre le marqueur plus long pour les marques principales
                minuteMarker.css('height', '15px');
            }
    
            // Ajouter le marqueur à l'horloge
            clockFace.append(minuteMarker);
        }
    }
    
    // Fonction pour dessiner le diagramme circulaire (camembert)
    function drawCamembert() {
        // Récupérer l'élément canvas
        const canvas = document.getElementById('camembert');
        // Obtenir le contexte de dessin 2D
        const ctx = canvas.getContext('2d');    //2d = graphique
        // Calculer le centre du canvas
        const centerX = canvas.width / 2;
        const centerY = canvas.height / 2;
        // Définir le rayon du camembert
        const radius = canvas.width / 2 - 10;
        
        // Effacer tout le canvas
        ctx.clearRect(0, 0, canvas.width, canvas.height);
    
        // Fonction pour convertir les heures en radians
        function hoursToRadians(hours) {
            // Convertir les heures en angle (360 degrés / 24 heures = 15 degrés par heure)
            // Soustraire 90 degrés (π/2) pour aligner sur midi
            return (hours * 15 - 90) * Math.PI / 180;
        }

        // Fonction pour obtenir un point sur le cercle
        function getPointOnCircle(hours, radius) {
            const angle = hoursToRadians(hours);
            return {
                x: centerX + radius * Math.cos(angle),
                y: centerY + radius * Math.sin(angle)
            };
        }
    
        // Définir les segments du camembert avec leurs heures et couleurs
        const segments = [
            { start: 22, end: 8, color: 'blue' },    // Nuit : 22h à 8h (bleu)
            { start: 8, end: 12, color: 'green' },    // Matin : 8h à 12h (vert)
            { start: 12, end: 14, color: 'yellow' }, // Midi : 12h à 14h (jaune)
            { start: 14, end: 18, color: 'orange' }, // Après-midi : 14h à 18h (orange)
            { start: 18, end: 22, color: 'red' },    // Soir : 18h à 22h (rouge)
            { start: 22, end: 24, color: 'blue' }    // Nuit : 22h à 24h
        ];
        
        // Fonction pour dessiner un soleil simplifié
        function drawSimpleSun(x, y) {
            // Sauvegarder l'état du contexte
            ctx.save();
            // Déplacer l'origine au point spécifié
            ctx.translate(x, y);
            
            // Dessiner le cercle du soleil
            ctx.beginPath();
            ctx.arc(0, 0, 15, 0, Math.PI * 2);
            ctx.fillStyle = '#FFD700'; // Couleur or
            ctx.fill();
            
            // Dessiner les rayons du soleil
            ctx.strokeStyle = '#FFD700';
            ctx.lineWidth = 2;
            // 8 rayons
            for (let i = 0; i < 8; i++) {
                const angle = (i * Math.PI) / 4;
                ctx.beginPath();
                ctx.moveTo(Math.cos(angle) * 15, Math.sin(angle) * 15);
                ctx.lineTo(Math.cos(angle) * 22, Math.sin(angle) * 22);
                ctx.stroke();
            }
            
            // Restaurer l'état du contexte
            ctx.restore();
        }

        // Fonction pour dessiner une lune simplifiée
        function drawSimpleMoon(x, y) {
            ctx.save();
            ctx.translate(x, y);
            
            // Dessiner le cercle de la lune
            ctx.beginPath();
            ctx.arc(0, 0, 15, 0, Math.PI * 2);
            ctx.fillStyle = '#FFFFFF'; // Blanc
            ctx.fill();
            
            // Dessiner une ombre pour créer un effet de croissant
            ctx.beginPath();
            ctx.arc(-3, 0, 13, 0, Math.PI * 2);
            ctx.fillStyle = '#1a237e'; // Bleu foncé
            ctx.fill();
            
            ctx.restore();
        }

        // Calculer les positions et dessiner les icônes
        const sunPosition = getPointOnCircle(8, radius + 20);
        const moonPosition = getPointOnCircle(22, radius + 15);
        
        // Dessiner le soleil et la lune
        drawSimpleSun(sunPosition.x, sunPosition.y);
        drawSimpleMoon(moonPosition.x, moonPosition.y);
    
        // Dessiner chaque segment du camembert
        segments.forEach(segment => {
            ctx.beginPath();
            ctx.moveTo(centerX, centerY);
    
            let startAngle = hoursToRadians(segment.start);
            let endAngle = hoursToRadians(segment.end);

            // Gérer le segment qui traverse minuit (22h-8h)
            if (segment.start > segment.end) {
                // Dessiner en deux parties
                // Première partie : de l'heure de début à minuit
                ctx.arc(centerX, centerY, radius, startAngle, hoursToRadians(24));
                ctx.lineTo(centerX, centerY);
                ctx.fill();
    
                // Deuxième partie : de minuit à l'heure de fin
                ctx.beginPath();
                ctx.moveTo(centerX, centerY);
                ctx.arc(centerX, centerY, radius, hoursToRadians(0), endAngle);
            } else {
                // Segment normal
                ctx.arc(centerX, centerY, radius, startAngle, endAngle);
            }
    
            ctx.lineTo(centerX, centerY);
            ctx.fillStyle = segment.color;
            ctx.fill();
        });
    }
    
    // Fonction pour mettre à jour l'horloge (affichage numérique et analogique)
    function updateClockSimple() {
        // Obtenir la date et l'heure actuelles
        const now = new Date();
        console.log("Mise à jour de l'horloge avec:", now);

        // Formater l'heure au format 24h
        const timeStr = now.toLocaleTimeString('fr-FR', {
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit',
            hour12: false
        });

        // Formater la date
        const dateStr = now.toLocaleDateString('fr-FR', {
            weekday: 'long',
            year: 'numeric',
            month: 'long',
            day: 'numeric'
        });

        console.log("Heure formatée:", timeStr);
        console.log("Date formatée:", dateStr);

        // Mettre à jour l'affichage numérique
        document.querySelector('.digital-clock').textContent = timeStr;
        document.querySelector('.date-display').textContent = dateStr;

        // Obtenir les heures, minutes et secondes
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const seconds = now.getSeconds();

        // Calculer les angles pour les aiguilles
        const hourDeg = (hours * 15) + (minutes * 0.25); // 15 degrés par heure + 0.25 degré par minute
        const minuteDeg = minutes * 6; // 6 degrés par minute
        const secondDeg = seconds * 6; // 6 degrés par seconde

        // Faire pivoter les aiguilles
        document.querySelector('.hour-hand').style.transform = `rotate(${hourDeg}deg)`;
        document.querySelector('.minute-hand').style.transform = `rotate(${minuteDeg}deg)`;
        document.querySelector('.second-hand').style.transform = `rotate(${secondDeg}deg)`;

        // Mettre en surbrillance le jour actuel dans la semaine
        const dayIndex = now.getDay(); // 0 (Dimanche) à 6 (Samedi)
        $('.day-box').removeClass('selected');
        $(`#day-${(dayIndex + 6) % 7}`).addClass('selected'); // Ajustement pour commencer à Lundi
    }

    // Initialisation au chargement de la page
    $(document).ready(function() {
        console.log("Document ready");
        
        // Dessiner le camembert
        drawCamembert();
        
        // Configurer les nombres d'heures
        setupClockNumbers();
        
        // Configurer les marqueurs de minutes
        setupMinuteMarkers();
        
        // Première mise à jour de l'horloge
        updateClockSimple();
        
        // Mettre à jour l'horloge toutes les secondes
        setInterval(updateClockSimple, 1000);
    });

    // Vérifier que les éléments HTML existent
    console.log("Éléments HTML présents :", {
        digitalClock: document.querySelector('.digital-clock') !== null,
        dateDisplay: document.querySelector('.date-display') !== null,
        hourHand: document.querySelector('.hour-hand') !== null,
        minuteHand: document.querySelector('.minute-hand') !== null,
        secondHand: document.querySelector('.second-hand') !== null
    });

    // Appel initial avec gestion des erreurs
    try {
        console.log("Premier appel de updateClockSimple");
        updateClockSimple();
    } catch (error) {
        console.error("Erreur lors de la première mise à jour:", error);
    }

    // Configurer la mise à jour périodique toutes les secondes
    setInterval(updateClockSimple, 1000);
});