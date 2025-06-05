var myPieChart;
function horloge(labelNuit, nuit_debut_journee, nuit_fin_journee, couleurnuit, 
                 labelMatin, matin, couleurmatin,
                 labelMidi, midi, couleurmidi,
                 labelApresMidi, apres_midi, couleurApresMidi,
                 labelSoir, soir, couleursoir){    
    var data = {
        labels: [labelNuit, labelMatin, labelMidi, labelApresMidi, labelSoir,labelNuit],
        datasets: [{
            data: [nuit_debut_journee, matin, midi, apres_midi, soir, nuit_fin_journee],
            backgroundColor: [couleurnuit, couleurmatin, couleurmidi, couleurApresMidi, couleursoir, couleurnuit],
            borderWidth: 0 // Aucune bordure entre les parties
        }]
    };
    // Configuration du camembert
    var options = {
        responsive: false,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                display: false // Masquer la légende
            },
            tooltip: {
                enabled: false // Désactiver les info-bulles au survol
            }
        },
        cutout: '0%', // Pour un pie chart plein
        circumference: 360 // Cercle complet
    };

    // Création de la légende
    createLegend([
        {label: labelNuit, color: couleurnuit},
        {label: labelMatin, color: couleurmatin},
        {label: labelMidi, color: couleurmidi},
        {label: labelApresMidi, color: couleurApresMidi},
        {label: labelSoir, color: couleursoir}
    ]);

    // Récupération du contexte du canevas
    var ctx = document.getElementById("myPieChart").getContext('2d');
    
    if (myPieChart) {
        myPieChart.destroy();   
    }

    // Création du camembert
    myPieChart = new Chart(ctx, {
        type: 'pie', // Utiliser un diagramme en "pie"
        data: data,
        options: options
    });
}

function createLegend(items) {
    // Supprimer l'ancienne légende si elle existe
    const oldLegend = document.querySelector('.legend-container');
    if (oldLegend) oldLegend.remove();

    const legendContainer = document.createElement('div');
    legendContainer.className = 'legend-container';

    items.forEach(item => {
        const legendItem = document.createElement('div');
        legendItem.className = 'legend-item';

        const colorBox = document.createElement('div');
        colorBox.className = 'legend-color';
        colorBox.style.backgroundColor = item.color;

        const label = document.createElement('span');
        label.className = 'legend-label';
        label.textContent = item.label;

        legendItem.appendChild(colorBox);
        legendItem.appendChild(label);
        legendContainer.appendChild(legendItem);
    });

    document.querySelector('.horloge-container').appendChild(legendContainer);
}

function afficherHorloge() {
    //titre_matin = document.getElementById("titre_matin").textContent;
    debut_matin = document.getElementById("matin1").value;
    debut_matin_minute = parseInt(debut_matin.split(":")[0])*60 + parseInt(debut_matin.split(":")[1]);
    couleur_matin = document.getElementById("couleur_matin").value;

    titre_midi = document.getElementById("titre_midi").textContent;
    debut_midi = document.getElementById("midi1").value;
    debut_midi_minute = parseInt(debut_midi.split(":")[0])*60 + parseInt(debut_midi.split(":")[1]);
    couleur_midi = document.getElementById("couleur_midi").value;

    titre_apresmidi = document.getElementById("titre_apres_midi").textContent;
    debut_apresmidi = document.getElementById("apres_midi1").value;
    debut_apresmidi_minute = parseInt(debut_apresmidi.split(":")[0])*60 + parseInt(debut_apresmidi.split(":")[1]);
    couleur_apresmidi = document.getElementById("couleur_apres_midi").value;

    titre_soir = document.getElementById("titre_soir").textContent;
    debut_soir = document.getElementById("soir1").value;
    debut_soir_minute = parseInt(debut_soir.split(":")[0])*60 + parseInt(debut_soir.split(":")[1]);
    couleur_soir = document.getElementById("couleur_soir").value;

    titre_nuit = document.getElementById("titre_nuit").textContent;
    debut_nuit = document.getElementById("nuit1").value;
    debut_nuit_minute = parseInt(debut_nuit.split(":")[0])*60 + parseInt(debut_nuit.split(":")[1]);
    couleur_nuit = document.getElementById("couleur_nuit").value;

    if (debut_matin_minute < debut_midi_minute && debut_midi_minute < debut_apresmidi_minute && debut_apresmidi_minute < debut_soir_minute && debut_soir_minute < debut_nuit_minute) {
        duree_nuit_debut_journee = debut_matin_minute;
        duree_matin = debut_midi_minute - debut_matin_minute;
        duree_midi = debut_apresmidi_minute - debut_midi_minute;
        duree_apresmidi = debut_soir_minute - debut_apresmidi_minute;
        duree_soir = debut_nuit_minute - debut_soir_minute;
        duree_nuit_fin_journee = 1440 - debut_nuit_minute;

        //console.log([titre_nuit, titre_matin, titre_midi, titre_apresmidi, titre_soir, titre_nuit]);
        //console.log([duree_nuit_debut_journee, duree_matin, duree_midi, duree_apresmidi, duree_soir, duree_nuit_fin_journee]);
        //console.log([couleur_nuit, couleur_matin, couleur_midi, couleur_apresmidi, couleur_soir, couleur_nuit]);

        // update les données
        myPieChart.data.labels = [titre_nuit, titre_matin, titre_midi, titre_apresmidi, titre_soir, titre_nuit];
        myPieChart.data.datasets[0].data = [duree_nuit_debut_journee, duree_matin, duree_midi, duree_apresmidi, duree_soir, duree_nuit_fin_journee];
        myPieChart.data.datasets[0].backgroundColor = [couleur_nuit, couleur_matin, couleur_midi, couleur_apresmidi, couleur_soir, couleur_nuit];
        myPieChart.update();

        $.ajax({
            type: "POST",
            url: "bdd.php",
            data: {
                action: 'majHoraireHorloge',
                matin1: debut_matin,
                couleur_matin: couleur_matin,
                midi1: debut_midi,
                couleur_midi: couleur_midi,
                apres_midi1: debut_apresmidi,
                couleur_apres_midi: couleur_apresmidi,
                soir1: debut_soir,update
            },
            success: function(response) {
                // Fonction à exécuter lorsque la requête réussit
                //console.log("Requête réussie !");
                // console.log(response); // Affiche la réponse renvoyée par le serveur
            },
            error: function(xhr, status, error) {
                // Fonction à exécuter en cas d'erreur de la requête
                console.error("Erreur lors de la requête :", error);
            }
        });
    } else {
        document.getElementById("Periodes_err").textContent = "Vérifier le début de chaque périodes";
        document.getElementById("Periodes_err").style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            document.getElementById("Periodes_err").textContent = "";
            document.getElementById("Periodes_err").style.border = "";
        }, 5000);
    }
}

function nombreHeureHorloge() {
    const horlogeFace = $('.horloge-face');
    const rayon = 200; // Augmenté de 150 à 160

    // Supprimer les anciens nombres s'ils existent
    $('.nombre-heure').remove();

    // Ajouter les nombres 1-24
    for (let i = 1; i <= 24; i++) {
        const angle = (i * 15) - 90; // 15 degrés par heure (360/24)
        const radian = (angle * Math.PI) / 180;
        
        // Calculer les positions x et y
        const x = Math.cos(radian) * rayon;
        const y = Math.sin(radian) * rayon;
        
        const nombreDiv = $('<div>')
            .addClass('nombre-heure')
            .text(i === 24 ? 0 : i)
            .css({
                'left': `${180 + x}px`,        // Centre à 180px + décalage
                'top': `${180 + y}px`,         // Centre à 180px + décalage  
                'transform': `translate(-50%, -50%)` // Centrage précis
            });

        horlogeFace.append(nombreDiv);
    }
}

function majHorlogeSimple() {
    const now = new Date();
    console.log("Mise à jour de l'horloge avec:", now);

    // Format de l'heure (24h)
    const timeStr = now.toLocaleTimeString('fr-FR', {
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
    });

    // Format de la date personnalisé
    const dateStr = now.toLocaleDateString('fr-FR', {
        weekday: 'long',
        year: 'numeric',
        month: 'long',
        day: 'numeric'
    });

    console.log("Heure formatée:", timeStr);
    console.log("Date formatée:", dateStr);

    // Mise à jour directe de l'affichage
    document.querySelector('.horloge-digital').textContent = timeStr;
    document.querySelector('.affichage-date').textContent = dateStr;

    // Calculs pour l'horloge analogique
    const hours = now.getHours();
    const minutes = now.getMinutes();
    const seconds = now.getSeconds();

    // Calcul des angles
    const hourDeg = (hours * 15) + (minutes * 0.25);
    const minuteDeg = minutes * 6;
    const secondDeg = seconds * 6;

    // Mise à jour des aiguilles
    document.querySelector('.aiguille-heure').style.transform = `rotate(${hourDeg}deg)`;
    document.querySelector('.aiguille-minute').style.transform = `rotate(${minuteDeg}deg)`;
    document.querySelector('.aiguille-seconde').style.transform = `rotate(${secondDeg}deg)`;

    // Sélectionner le jour actuel
    const dayIndex = now.getDay(); // 0=Lundi, 6=Dimanche; // 0 (Dimanche) à 6 (Samedi)
    $('.box-jour').removeClass('selected');
    $(`#jour-${(dayIndex + 6) % 7}`).addClass('selected'); // Ajustement pour commencer à Lundi

    updateCache(now);
}

$(document).ready(function() {
    console.log("Document ready");
    nombreHeureHorloge();
    setInterval(majHorlogeSimple, 1000);
    setInterval(afficherHorloge, 5000);

});

function pictogramme(id_picto, minuteDebut, minuteFin){
    var div_pictogramme = document.getElementById("div-picto"+id_picto);
    var pictogramme = document.getElementById("div-picto"+id_picto).getElementsByTagName("svg")[0];
    
    // console.log(minuteDebut, minuteFin);
    heureDebut=minuteDebut/60;
    heureFin=minuteFin/60;
    // console.log(heureDebut, heureFin);
    
    heure=heureDebut+(heureFin-heureDebut)/3;
    // console.log(heure)
    
    ajustement = (minuteFin-minuteDebut)*0.03;
    if(minuteFin-minuteDebut <= 30) {
        ajustement=-1;
    }
    x=(heure*15-90)+ajustement; // -ajustement pour le décalage sufisant en foction de la taille de l'image et la largeur de l'event
    // console.log(x, ajustement);

    div_pictogramme.style.transform = "rotate(" + x +"deg)";
    pictogramme.style.transform ="rotate(" + -x +"deg)";
    pictogramme.style.width = "75%";
    pictogramme.style.height = "75%";
    pictogramme.style.marginTop = "3px";
}

function updateCache(now) {
    const cache = document.getElementById('cacheHorloge');
    const hours = now.getHours();
    const minutes = now.getMinutes();
    const seconds = now.getSeconds();

    const totalMinutes = (hours * 60) + minutes + (seconds / 60);
    const angle = (totalMinutes / (24 * 60)) * 360;

    cache.style.setProperty('--angle-cache', `${angle}deg`);
}