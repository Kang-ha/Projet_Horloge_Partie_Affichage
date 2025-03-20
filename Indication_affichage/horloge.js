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
        const radius = canvas.width / 2 - 10; // Réduire légèrement le rayon pour éviter le débordement
    
        // ... reste de la fonction drawCamembert ...
    }

    // Fonction pour dessiner le camembert
    function drawCamembert() {
        const canvas = document.getElementById('camembert');
        const ctx = canvas.getContext('2d');
        const segments = [
            { start: 22, end: 8, color: 'blue' },   // 22h à 8h
            { start: 8, end: 12, color: 'green' },  // 8h à 12h
            { start: 12, end: 14, color: 'yellow' },// 12h à 14h
            { start: 14, end: 18, color: 'orange' },// 14h à 18h
            { start: 18, end: 22, color: 'red' }    // 18h à 22h
        ];

        const centerX = canvas.width / 2;
        const centerY = canvas.height / 2;
        const radius = canvas.width / 2;

        function drawSegment(startHour, endHour, color) {
            const startAngle = (startHour - 3) * (Math.PI / 12);
            const endAngle = (endHour - 3) * (Math.PI / 12);

            ctx.beginPath();
            ctx.moveTo(centerX, centerY);
            ctx.arc(centerX, centerY, radius, startAngle, endAngle, false);
            ctx.closePath();
            ctx.fillStyle = color;
            ctx.fill();
        }

        segments.forEach(segment => {
            const start = segment.start;
            const end = segment.end;

            if (start < end) {
                drawSegment(start, end, segment.color);
            } else {
                drawSegment(start, 24, segment.color);
                drawSegment(0, end, segment.color);
            }
        });
    }

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