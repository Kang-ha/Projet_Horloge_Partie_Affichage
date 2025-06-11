// Variable globale pour stocker l'instance du graphique en camembert
var myPieChart;

// Fonction pour initialiser l'horloge avec les périodes de la journée
function horloge(labelNuit, nuit_debut_journee, nuit_fin_journee, couleurnuit, 
                 labelMatin, matin, couleurmatin,
                 labelMidi, midi, couleurmidi,
                 labelApresMidi, apres_midi, couleurApresMidi,
                 labelSoir, soir, couleursoir){    
    // Données pour le graphique en camembert
    var data = {
        labels: [labelNuit, labelMatin, labelMidi, labelApresMidi, labelSoir, labelNuit],
        datasets: [{
            data: [nuit_debut_journee, matin, midi, apres_midi, soir, nuit_fin_journee],
            backgroundColor: [couleurnuit, couleurmatin, couleurmidi, couleurApresMidi, couleursoir, couleurnuit],
            borderWidth: 0 // Pas de bordure entre les sections
        }]
    };

    // Options de configuration du graphique
    var options = {
        responsive: false,          // Empêche le graphique de s'adapter automatiquement à la taille de l'écran ou de la fenêtre, cela signifie que le graphique conservera la taille spécifiée dans le <canvas> HTML.
        maintainAspectRatio: false, // Désactive le maintien du ratio hauteur/largeur par défaut, cela permet de forcer une taille personnalisée (utile quand 'responsive' est false).
        plugins: {
            legend: {
                display: false // Masque la légende
            },
            tooltip: {
                enabled: false // Désactive les infobulles
            }
        },
        cutout: '0%', // Graphique plein (pas en anneau)
        circumference: 360 // Cercle complet
    };

    // Crée la légende avec les couleurs et labels
    createLegend([
        {label: labelNuit, color: couleurnuit},
        {label: labelMatin, color: couleurmatin},
        {label: labelMidi, color: couleurmidi},
        {label: labelApresMidi, color: couleurApresMidi},
        {label: labelSoir, color: couleursoir}
    ]);

    // Récupère le contexte du canvas
    var ctx = document.getElementById("myPieChart").getContext('2d');
    
    // Détruit l'ancien graphique s'il existe
    if (myPieChart) {
        myPieChart.destroy();   
    }

    // Crée un nouveau graphique en camembert (pie chart) avec Chart.js
    myPieChart = new Chart(ctx, {  // 'ctx' est le contexte du canevas HTML où le graphique sera dessiné (souvent obtenu via getContext('2d'))
        type: 'pie',               // Spécifie que le type de graphique est un camembert (pie chart)
        data: data,                // Les données à afficher dans le graphique (labels, datasets, etc.)
        options: options           // Les options de configuration du graphique (titres, légendes, animations, etc.)
    });
    // Ajoute le soleil et la lune après la création du graphique
    addSunAndMoon(nuit_debut_journee, matin, midi, apres_midi, soir, nuit_fin_journee);
}

// Fonction pour afficher les nombres d'heures autour de l'horloge
function nombreHeureHorloge() {
    const horlogeFace = $('.horloge-face');
    const rayon = 280; // Rayon du cercle des nombres

    // Supprime les anciens nombres
    $('.nombre-heure').remove();

    // Ajoute les nombres de 1 à 24
    for (let i = 1; i <= 24; i++) {
        const angle = (i * 15) - 90; // Angle en degrés (15° par heure)
        const radian = (angle * Math.PI) / 180; // Conversion en radians
        
        // Calcul des positions x et y
        const x = Math.cos(radian) * rayon;
        const y = Math.sin(radian) * rayon;
        
        // Crée un élément div pour le nombre
        const nombreDiv = $('<div>')          // Crée un nouvel élément <div> avec jQuery
            .addClass('nombre-heure')         // Ajoute une classe CSS appelée 'nombre-heure' (utile pour le style)
            .text(i === 24 ? 0 : i)           // Définit le texte affiché dans le div : si i vaut 24, on affiche 0, sinon on affiche i (utile pour représenter les heures sur une horloge de 24h, où 24 = 0h)
            .css({                            // Applique des styles CSS directement à l'élément :
                'left': `${250 + x}px`,       // Position horizontale en pixels, décalée à partir du centre (250 + x)
                'top': `${250 + y}px`,        // Position verticale en pixels, également décalée du centre
                'transform': `translate(-50%, -50%)` // Centre le div sur ses coordonnées (utile pour un positionnement précis en cercle)
            });

        // Ajoute le nombre à la face de l'horloge
        horlogeFace.append(nombreDiv);
    }
}

// Fonction pour mettre à jour l'horloge (analogique et digitale)
function majHorlogeSimple() {
    const now = new Date();
    console.log("Mise à jour de l'horloge avec:", now);

    // Formatage de l'heure
    const timeStr = now.toLocaleTimeString('fr-FR', {
        hour: '2-digit',       // Affiche l'heure sur 2 chiffres (ex : "08" au lieu de "8")
        minute: '2-digit',     // Affiche les minutes sur 2 chiffres
        second: '2-digit',     // Affiche les secondes sur 2 chiffres
        hour12: false
    });

    // Formatage de la date
    const dateStr = now.toLocaleDateString('fr-FR', {
        weekday: 'long',
        year: 'numeric',
        month: 'long',
        day: 'numeric'
    });

    console.log("Heure formatée:", timeStr);
    console.log("Date formatée:", dateStr);

    // Met à jour l'affichage digital
    document.querySelector('.horloge-digital').textContent = timeStr;
    document.querySelector('.affichage-date').textContent = dateStr;

    // Récupère les heures, minutes, secondes
    const hours = now.getHours();
    const minutes = now.getMinutes();
    const seconds = now.getSeconds();

    // Calcule les angles pour les aiguilles
    const hourDeg = (hours * 15) + (minutes * 0.25); // 15° par heure + 0.25° par minute
    const minuteDeg = minutes * 6; // 6° par minute
    const secondDeg = seconds * 6; // 6° par seconde

    // Applique la rotation aux aiguilles
    document.querySelector('.aiguille-heure').style.transform = `rotate(${hourDeg}deg)`;
    document.querySelector('.aiguille-minute').style.transform = `rotate(${minuteDeg}deg)`;
    document.querySelector('.aiguille-seconde').style.transform = `rotate(${secondDeg}deg)`;

    // Met en surbrillance le jour actuel
    const dayIndex = now.getDay(); // 0=Dimanche, 6=Samedi
    $('.jour-box').removeClass('selected');
    $(`#jour-${(dayIndex + 6) % 7}`).addClass('selected'); // Ajuste pour commencer à Lundi

    // Met à jour le cache de progression
    updateCache(now);
}

// Initialisation lorsque le document est prêt
$(document).ready(function() {
    console.log("Document ready");
    nombreHeureHorloge(); // Affiche les nombres d'heures
    setInterval(majHorlogeSimple, 1000); // Met à jour l'horloge toutes les secondes
});

// Fonction pour positionner un pictogramme sur l'horloge
function pictogramme(id_picto, minuteDebut, minuteFin){
    var div_pictogramme = document.getElementById("div-picto"+id_picto);
    var pictogramme = document.getElementById("div-picto"+id_picto).getElementsByTagName("svg")[0];
    
    // Convertit les minutes en heures
    var heureDebut = minuteDebut / 60;
    var heureFin = minuteFin / 60;
    
    // Calcule l'heure moyenne de l'événement
    var heureMoyenne = heureDebut + (heureFin - heureDebut) / 2;
    
    // Calcule l'angle en degrés (0° = minuit, sens horaire)
    var angleDegres = (heureMoyenne * 15) - 90; // -90 pour commencer à minuit en haut
    
    var rayon = 220; // Rayon pour positionner les pictogrammes
    
    // Conversion en radians
    var angleRadians = (angleDegres * Math.PI) / 180;
    
    // Calcule les positions x et y
    var x = Math.cos(angleRadians) * rayon;
    var y = Math.sin(angleRadians) * rayon;
    
    // Positionne le pictogramme
    div_pictogramme.style.position = "absolute";
    div_pictogramme.style.left = (250 + x) + "px"; // 250 = centre de l'horloge
    div_pictogramme.style.top = (250 + y) + "px";
    div_pictogramme.style.transform = "translate(-50%, -50%)";
    div_pictogramme.style.transformOrigin = "center";
    
    // Garde le pictogramme droit
    if (pictogramme) {
        pictogramme.style.transform = "rotate(0deg)";
        pictogramme.style.width = "100%";
        pictogramme.style.height = "100%";
    }
}

// Fonction pour afficher les pictogrammes via AJAX
function afficherPictogrammes(){
    var afficherPictogrammes = document.getElementById("afficherPictogrammes");
    var afficherpictogrammes = $('#afficherPictogrammes');
    $.ajax({
        type: "POST",
        url: "bdd.php",
        data: {
            action: 'afficherPictogrammes'
        },
        success: function(response) {
            // Met à jour le contenu avec la réponse
            afficherPictogrammes.innerHTML = '';
            afficherPictogrammes.innerHTML = response;
            // Exécute les scripts inclus dans la réponse
            afficherpictogrammes.find('script').each(function() {
                eval(this.innerHTML);
            });
        },
        error: function(xhr, status, error) {                                             
            console.error("Erreur lors de la requête :", error);
        }
    });
}

// Fonction pour mettre à jour le cache de progression du temps
function updateCache(now) {
    const cache = document.getElementById('cacheHorloge');
    const hours = now.getHours();
    const minutes = now.getMinutes();
    const seconds = now.getSeconds();

    // Calcule le temps total en minutes
    const totalMinutes = (hours * 60) + minutes + (seconds / 60);
    // Calcule l'angle correspondant
    const angle = (totalMinutes / (24 * 60)) * 360;

    // Met à jour l'angle du cache
    cache.style.setProperty('--angle-cache', `${angle}deg`);
}

// Fonction pour ajouter le soleil et la lune sur le camembert
function addSunAndMoon(nuit_debut_journee, matin, midi, apres_midi, soir, nuit_fin_journee) {
    // Supprime les anciens éléments s'ils existent
    const oldSun = document.querySelector('.sun-icon');
    const oldMoon = document.querySelector('.moon-icon');
    if (oldSun) oldSun.remove();
    if (oldMoon) oldMoon.remove();

    const horlogeFace = document.querySelector('.horloge-face');
    const rayon = 200; // Rayon pour positionner les icônes

    // Calcul de l'angle pour le lever du soleil (transition nuit -> matin)
    const debutMatin = nuit_debut_journee; // fin de la nuit du début = début du matin
    const angleMatin = (debutMatin / 24) * 360 - 90; // -90 pour commencer à minuit en haut
    const angleMatinRadians = (angleMatin * Math.PI) / 180;
    
    // Calcul de l'angle pour la lune (transition soir -> nuit de fin)
    const finSoir = nuit_debut_journee + matin + midi + apres_midi + soir;
    const angleLune = (finSoir / 24) * 360 - 90; // Position à la fin du soir (début de la nuit de fin)
    const angleLuneRadians = (angleLune * Math.PI) / 180;

    // Position du soleil
    const xSoleil = Math.cos(angleMatinRadians) * rayon;
    const ySoleil = Math.sin(angleMatinRadians) * rayon;

    // Position de la lune
    const xLune = Math.cos(angleLuneRadians) * rayon;
    const yLune = Math.sin(angleLuneRadians) * rayon;

    // Création du soleil avec SVG
    const soleil = document.createElement('div');
    soleil.className = 'sun-icon';
    soleil.innerHTML = `
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <circle cx="12" cy="12" r="4" fill="#FFD700" stroke="#FFA500" stroke-width="1"/>
            <path d="M12 2V4M12 20V22M4.93 4.93L6.34 6.34M17.66 17.66L19.07 19.07M2 12H4M20 12H22M4.93 19.07L6.34 17.66M17.66 6.34L19.07 4.93" stroke="#FFA500" stroke-width="2" stroke-linecap="round"/>
        </svg>
    `;
    soleil.style.position = 'absolute';
    soleil.style.left = `${250 + xSoleil}px`;
    soleil.style.top = `${250 + ySoleil}px`;
    soleil.style.transform = 'translate(-50%, -50%)';
    soleil.style.zIndex = '30';
    soleil.style.filter = 'drop-shadow(0 0 8px rgba(255, 215, 0, 0.8))';

    // Création de la lune avec SVG
    const lune = document.createElement('div');
    lune.className = 'moon-icon';
    lune.innerHTML = `
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" fill="#E6E6FA" stroke="#C0C0C0" stroke-width="1"/>
        </svg>
    `;
    lune.style.position = 'absolute';
    lune.style.left = `${250 + xLune}px`;
    lune.style.top = `${250 + yLune}px`;
    lune.style.transform = 'translate(-50%, -50%)';
    lune.style.zIndex = '30';
    lune.style.filter = 'drop-shadow(0 0 8px rgba(230, 230, 250, 0.8))';

    // Ajoute les éléments à la face de l'horloge
    horlogeFace.appendChild(soleil);
    horlogeFace.appendChild(lune);
}

// Fonction pour créer une légende
function createLegend(items) {
    // Supprime l'ancienne légende si elle existe
    const oldLegend = document.querySelector('.legend-container');
    if (oldLegend) oldLegend.remove();

    // Crée un conteneur pour la légende
    const legendContainer = document.createElement('div');
    legendContainer.className = 'legend-container';

    // Ajoute chaque élément de la légende
    items.forEach(item => {
        const legendItem = document.createElement('div');
        legendItem.className = 'legend-item';

        // Boîte de couleur
        const colorBox = document.createElement('div');
        colorBox.className = 'legend-color';
        colorBox.style.backgroundColor = item.color;

        // Label texte
        const label = document.createElement('span');
        label.className = 'legend-label';
        label.textContent = item.label;

        // Assemble les éléments
        legendItem.appendChild(colorBox);
        legendItem.appendChild(label);
        legendContainer.appendChild(legendItem);
    });

    // Ajoute la légende au conteneur principal
    document.querySelector('.horloge-container').appendChild(legendContainer);
}