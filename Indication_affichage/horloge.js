$(document).ready(function() {
    console.log("Document ready"); // Vérification que le script démarre

    // Fonction pour positionner les nombres sur l'horloge
    function setupClockNumbers() {
        const clockFace = $('.analog-clock');
        const radius = 195; // Rayon pour les heures
    
        // Supprimer les anciens nombres s'ils existent
        $('.hour-number').remove();
    
        // Ajouter les nombres 1-24
        for (let i = 1; i <= 24; i++) {
            const angle = (i * 15) - 90; // 15 degrés par heure (360/24)
            const radian = (angle * Math.PI) / 180;
            
            // Calculer les positions x et y
            const x = Math.cos(radian) * radius;
            const y = Math.sin(radian) * radius;
    
            const numberDiv = $('<div>')
                .addClass('hour-number')
                .text(i)
                .css({
                    'left': `${220 + x}px`, // 220 est la moitié de la largeur de l'horloge (440/2)
                    'top': `${220 + y}px`   // 220 est la moitié de la hauteur de l'horloge (440/2)
                });
    
            clockFace.append(numberDiv);
        }
    }
    
    // Modifier aussi la fonction setupMinuteMarkers pour s'assurer qu'elle n'interfère pas
    function setupMinuteMarkers() {
        const clockFace = $('.analog-clock');
        const radius = 230; // Un peu plus grand que le rayon des heures
        
        // Supprimer les anciens marqueurs s'ils existent
        $('.minute-marker, .minute-number').remove();
    
        for (let i = 0; i < 60; i++) {
            const angle = (i * 6) - 90; // 6 degrés par minute (360/60)
            const radian = (angle * Math.PI) / 180;
    
            const minuteMarker = $('<div>')
                .addClass('minute-marker')
                .css({
                    'transform': `rotate(${angle + 90}deg)`,
                    'left': '219px',
                    'top': '-5px'
                });
    
            if (i % 5 === 0) {
                const numberRadius = radius + 15;
                const numberX = Math.cos(radian) * numberRadius;
                const numberY = Math.sin(radian) * numberRadius;
                
                const minuteNumber = $('<div>')
                    .addClass('minute-number')
                    .text(i)
                    .css({
                        'left': `${220 + numberX}px`,
                        'top': `${220 + numberY}px`
                    });
    
                clockFace.append(minuteNumber);
                minuteMarker.css('height', '15px');
            }
    
            clockFace.append(minuteMarker);
        }
    }
    
    // Assurez-vous que ces fonctions sont appelées dans le bon ordre
    $(document).ready(function() {
        console.log("Document ready");
        
        // D'abord dessiner le camembert
        drawCamembert();
        
        // Ensuite configurer les nombres d'heures
        setupClockNumbers();
        
        // Puis ajouter les marqueurs de minutes
        setupMinuteMarkers();
        
        // Première mise à jour de l'horloge
        updateClockSimple();
        
        // Mise à jour toutes les secondes
        setInterval(updateClockSimple, 1000);
    });
    
    // Ajuster aussi la fonction drawCamembert pour la nouvelle taille
    function drawCamembert() {
        const canvas = document.getElementById('camembert');
        const ctx = canvas.getContext('2d');
        const centerX = canvas.width / 2;
        const centerY = canvas.height / 2;
        const radius = canvas.width / 2 - 10;
    
        // Effacer le canvas
        ctx.clearRect(0, 0, canvas.width, canvas.height);
    
        // Fonction pour convertir les heures en angles (en radians)
        function hoursToRadians(hours) {
            // Convertir les heures en angle (360 degrés / 24 heures = 15 degrés par heure)
            // Soustraire 90 degrés (π/2) pour aligner sur midi
            return (hours * 15 - 90) * Math.PI / 180;
        }

        function getPointOnCircle(hours, radius) {
            const angle = hoursToRadians(hours);
            return {
                x: centerX + radius * Math.cos(angle),
                y: centerY + radius * Math.sin(angle)
            };
        }
    
        // Définir les segments avec leurs heures exactes
        const segments = [
            { start: 0, end: 8, color: 'rgb(0, 0, 255)' },    // Nuit : 24h à 8h (bleu)
            { start: 8, end: 12, color: 'rgb(0, 255, 0)' },    // Matin : 8h à 12h (vert)
            { start: 12, end: 14, color: 'rgb(255, 255, 0)' }, // Midi : 12h à 14h (jaune)
            { start: 14, end: 18, color: 'rgb(255, 166, 0)' }, // Après-midi : 14h à 18h (orange)
            { start: 18, end: 22, color: 'rgb(255, 0, 0)' },    // Soir : 18h à 22h (rouge)
            { start: 22, end: 24, color: 'rgb(0, 0, 255)' }    // Nuit : 22h à 24h
        ];
        
        // Dessiner chaque segment
        segments.forEach(segment => {
            ctx.beginPath();
            ctx.moveTo(centerX, centerY);
    
            let startAngle = hoursToRadians(segment.start);
            let endAngle = hoursToRadians(segment.end);
    
            // Ajuster pour le segment qui traverse minuit
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
        function drawSimpleSun(x, y) {
            ctx.save();
            ctx.translate(x, y);
        
            // Cercle du soleil
            ctx.beginPath();
            ctx.arc(0, 0, 15, 0, Math.PI * 2);
            ctx.fillStyle = '#FFD700';
            ctx.fill();
        
            // Rayons simples
            ctx.strokeStyle = '#FFD700';
            ctx.lineWidth = 2;
            for (let i = 0; i < 8; i++) {
                const angle = (i * Math.PI) / 4;
                ctx.beginPath();
                ctx.moveTo(Math.cos(angle) * 15, Math.sin(angle) * 15);
                ctx.lineTo(Math.cos(angle) * 22, Math.sin(angle) * 22);
                ctx.stroke();
            }
        
            ctx.restore();
        }

    // Fonction pour dessiner une simple lune (22h)
    function drawSimpleMoon(x, y) {
        ctx.save();
        ctx.translate(x, y);
        
        // Cercle de la lune
        ctx.beginPath();
        ctx.arc(0, 0, 15, 0, Math.PI * 2);
        ctx.fillStyle = '#FFFFFF';
        ctx.fill();
        
        // Ombre pour créer le croissant
        ctx.beginPath();
        ctx.arc(-3, 0, 13, 0, Math.PI * 2);
        ctx.fillStyle = '#1a237e';
        ctx.fill();
        
        ctx.restore();
    }
    
        // Calculer les positions du soleil et de la lune
        const sunrisePos = getPointOnCircle(8, radius + 10);
        const moonrisePos = getPointOnCircle(22, radius + 10);
        
        // Dessiner le soleil levant et la lune levante
        drawSunrise(sunrisePos.x, sunrisePos.y);
        drawMoonrise(moonrisePos.x, moonrisePos.y);
    }
    
    // Assurez-vous d'appeler drawCamembert() au chargement et à chaque mise à jour
    $(document).ready(function() {
        console.log("Document ready");
        drawCamembert();
        setupClockNumbers();
        setupMinuteMarkers();
        updateClockSimple();
        setInterval(updateClockSimple, 1000);
    });

    // Fonction simplifiée pour mettre à jour l'horloge
    function updateClockSimple() {
        const now = new Date();
        console.log("Mise à jour de l'horloge avec:", now);

        // Format de l'heure (24h)
        const timeStr = now.toLocaleTimeString('fr-FR', {
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit',
            hour12: false
        });

        // Format de la date
        const dateStr = now.toLocaleDateString('fr-FR', {
            weekday: 'long',
            year: 'numeric',
            month: 'long',
            day: 'numeric'
        });

        console.log("Heure formatée:", timeStr);
        console.log("Date formatée:", dateStr);

        // Mise à jour directe de l'affichage
        document.querySelector('.digital-clock').textContent = timeStr;
        document.querySelector('.date-display').textContent = dateStr;

        // Calculs pour l'horloge analogique
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const seconds = now.getSeconds();

        // Calcul des angles
        const hourDeg = (hours * 15) + (minutes * 0.25);
        const minuteDeg = minutes * 6;
        const secondDeg = seconds * 6;

        // Mise à jour des aiguilles
        document.querySelector('.hour-hand').style.transform = `rotate(${hourDeg}deg)`;
        document.querySelector('.minute-hand').style.transform = `rotate(${minuteDeg}deg)`;
        document.querySelector('.second-hand').style.transform = `rotate(${secondDeg}deg)`;

        // Sélectionner le jour actuel
        const dayIndex = now.getDay(); // 0 (Dimanche) à 6 (Samedi)
        $('.day-box').removeClass('selected');
        $(`#day-${(dayIndex + 6) % 7}`).addClass('selected'); // Ajustement pour commencer à Lundi
    }

    // Dessiner le camembert
    drawCamembert();

    // Test initial pour vérifier que les éléments existent
    console.log("Éléments HTML présents :", {
        digitalClock: document.querySelector('.digital-clock') !== null,
        dateDisplay: document.querySelector('.date-display') !== null,
        hourHand: document.querySelector('.hour-hand') !== null,
        minuteHand: document.querySelector('.minute-hand') !== null,
        secondHand: document.querySelector('.second-hand') !== null
    });

    // Appel initial
    try {
        console.log("Premier appel de updateClockSimple");
        updateClockSimple();
    } catch (error) {
        console.error("Erreur lors de la première mise à jour:", error);
    }

    // Mise à jour toutes les secondes
    setInterval(updateClockSimple, 1000);
});