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
        cutoutPercentage: 0 // Ajuster la découpe pour remplir le cercle
    };
    
    // Récupération du contexte du canevas
    var ctx = document.getElementById("myPieChart").getContext('2d');
    
    if (myPieChart) {
        myPieChart.destroy();   
    }

    // Création du camembertk
    myPieChart = new Chart(ctx, {
        type: 'pie', // Utiliser un diagramme en "pie"
        data: data,
        options: options
    });
}

function afficherHorloge() {
    titre_matin = document.getElementById("titre_matin").textContent;
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

        // console.log([titre_nuit, titre_matin, titre_midi, titre_apresmidi, titre_soir, titre_nuit]);
        // console.log([duree_nuit_debut_journee, duree_matin, duree_midi, duree_apresmidi, duree_soir, duree_nuit_fin_journee]);
        // console.log([couleur_nuit, couleur_matin, couleur_midi, couleur_apresmidi, couleur_soir, couleur_nuit]);

        // update les données
        myPieChart.data.labels = [titre_nuit, titre_matin, titre_midi, titre_apresmidi, titre_soir, titre_nuit];
        myPieChart.data.datasets[0].data = [duree_nuit_debut_journee, duree_matin, duree_midi, duree_apresmidi, duree_soir, duree_nuit_fin_journee];
        myPieChart.data.datasets[0].backgroundColor = [couleur_nuit, couleur_matin, couleur_midi, couleur_apresmidi, couleur_soir, couleur_nuit];
        myPieChart.update();

        $.ajax({
            type: "POST",
            url: "bdd.php",
            data: {
                action: 'updateHoraireHorloge',
                matin1: debut_matin,
                couleur_matin: couleur_matin,
                midi1: debut_midi,
                couleur_midi: couleur_midi,
                apres_midi1: debut_apresmidi,
                couleur_apres_midi: couleur_apresmidi,
                soir1: debut_soir,
                couleur_soir: couleur_soir,
                nuit1: debut_nuit,
                couleur_nuit: couleur_nuit
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

function aiguille(){
    var aiguilleHeures = document.querySelector(".aiguille-heures");
    var maDate = new Date();
    var valeurh = maDate.getHours();
    var valeurm = maDate.getMinutes();
    var valeurs = maDate.getSeconds();

    var valeur = valeurh + (valeurm / 60) + (valeurs / 3600);
    //console.log(valeur);
    aiguilleHeures.style.transform = "rotate(" + ((valeur*15)-90) + "deg)";   
}

function pictogramme(id_picto, minuteDebut, minuteFin){
    var div_pictogramme = document.getElementById("div-picto"+id_picto);
    var pictogramme = document.getElementById("div-picto"+id_picto).getElementsByTagName("svg")[0];
    
    // console.log(minuteDebut, minuteFin);
    heureDebut=minuteDebut/60;
    heureFin=minuteFin/60;
    // console.log(heureDebut, heureFin);
    
    heure=heureDebut+(heureFin-heureDebut)/3;
    // console.log(heure)
    
    ajustement = (minuteFin-minuteDebut)*0.001;
    if(minuteFin-minuteDebut <= 30) {
        ajustement=-0.03;
    }
    x=(heure*15-90)+ajustement; // -ajustement pour le décalage sufisant en foction de la taille de l'image et la largeur de l'event
    // console.log(x, ajustement);

    div_pictogramme.style.transform = "rotate(" + x +"deg)";
    pictogramme.style.transform ="rotate(" + -x +"deg)";
    pictogramme.style.width = "75%";
    pictogramme.style.height = "75%";
    pictogramme.style.marginTop = "3px";

}


function afficherHeures() {
    var div_time = document.getElementById("div_time");
    var time = new Date();
    var valeurh = time.getHours();
    var valeurm = time.getMinutes();
    var valeurs = time.getSeconds();

    if(valeurh <= 9) {valeurh = "0" + valeurh;}
    if(valeurm <= 9) {valeurm = "0" + valeurm;}
	if(valeurs <= 9) {valeurs = "0" + valeurs;}		

    div_time.textContent = ("Il est : "+valeurh+":"+valeurm+":"+valeurs);
    //console.log("heure = "+valeurh+", minute = "+valeurm+", seconde = "+valeurs);
}

function afficherDate(){
    var div_time = document.getElementById("div_date");
    var date = new Date();

    var l_jours = ["Dimanche","Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi"];
    var l_mois = ["Janvier","Février","Mars","Avril","Mai","Juin","Juillet","Août","Septembre","Octobre","Novembre","Décembre"];
    var jour = l_jours[date.getDay()];
    var nb_jours = date.getDate();
    var mois = l_mois[date.getMonth()];
    var année = date.getFullYear();
    
    div_time.textContent = ("La date du jour est : "+jour+" "+nb_jours+" "+mois+" "+année);
    //console.log(jour+" "+nb_jours+" "+mois+" "+année);
}

function afficherPeriode(){
    var div_periode = document.getElementById("div_periode");
    var time = new Date();
    var valeurh = time.getHours();
    var valeurm = time.getMinutes();

    var temps_minute = (valeurh*60) + valeurm;
    
    //console.log(temps_minute);

    $.ajax({
        type: "POST",
        url: "bdd.php",
        data: {
            action: 'getPeriode',
            temps_minute: temps_minute
        },
        success: function(response) {
            // Fonction à exécuter lorsque la requête réussit
            //console.log("Requête réussie !");
            //console.log(response); // Affiche la réponse renvoyée par le serveur
            if (response == "matin") {response = "Matin";}
            if (response == "midi") {response = "Midi";}
            if (response == "apres_midi") {response = "Après midi";}
            if (response == "soir") {response = "Soir";}
            if (response == "nuit") {response = "Nuit";}
            div_periode.textContent = ("La période de la journée est : "+response);
            // Faites quelque chose avec la réponse ici
        },
        error: function(xhr, status, error) {
            // Fonction à exécuter en cas d'erreur de la requête
            console.error("Erreur lors de la requête :", error);
        }
    });
}

window.setInterval(function() {
    aiguille();
    afficherHeures();
    afficherPeriode();
    afficherDate();
}, 1000);

var myDoughnutChart = null;
function horlogeEvent(donnees, couleurs){
    var data = {
        datasets: [{
            data: donnees,
            backgroundColor: couleurs,
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
        cutoutPercentage: 0 // Ajuster la découpe pour remplir le cercle
    };

    // Récupération du contexte du canevas
    var ctx = document.getElementById("myDoughnutChart").getContext('2d');

    if (myDoughnutChart) {
        myDoughnutChart.destroy();   
    }

    // Création du camembert
    myDoughnutChart = new Chart(ctx, {
        type: 'doughnut', // Utiliser un diagramme en "doughnut"
        data: data,
        options: options
    });

    // Dessiner les textures hachurées
    var patternCanvas1 = document.createElement('canvas');
    var patternContext1 = patternCanvas1.getContext('2d');
    patternCanvas1.width = 10;
    patternCanvas1.height = 10;

    patternContext1.lineWidth = 7.5; // Épaisseur du trait
    patternContext1.strokeStyle = 'rgba(0, 0, 0, 1)';
    patternContext1.beginPath();
    patternContext1.moveTo(0, 0);
    patternContext1.lineTo(10, 0);
    patternContext1.stroke();

    var pattern1 = ctx.createPattern(patternCanvas1, 'repeat');

    var patternCanvas2 = document.createElement('canvas');
    var patternContext2 = patternCanvas2.getContext('2d');
    patternCanvas2.width = 10;
    patternCanvas2.height = 10;

    patternContext2.lineWidth = 7.5; // Épaisseur du trait
    patternContext2.strokeStyle = 'rgba(0, 0, 0, 1)';
    patternContext2.beginPath();
    patternContext2.moveTo(0, 0);
    patternContext2.lineTo(0, 10);
    patternContext2.stroke();

    var pattern2 = ctx.createPattern(patternCanvas2, 'repeat');

    for (var i = 1; i < donnees.length; i += 2) {
        myDoughnutChart.data.datasets[0].backgroundColor[i] = (i % 4 === 1) ? pattern1 : pattern2;
    }
    myDoughnutChart.update();
}

var ancien_titre;
function editEvent(idEvent) {
    ancien_titre = document.getElementById("titre"+idEvent).textContent;
    $.ajax({
        type: "POST",
        url: "bdd.php",
        data: {
            action: 'getEvent',
            titre : document.getElementById("titre"+idEvent).textContent
        },
        success: function(response) {
            // Fonction à exécuter lorsque la requête réussit
            // console.log("Requête réussie !");
            var data = JSON.parse(response);
            // console.log(data); // Affiche la réponse renvoyée par le serveur
            var evenements = data.evenements[0];
            var pictogrammes = data.pictogrammes;
            // console.log(evenements);
            // console.log(pictogrammes);
            // console.log("titre = "+evenements.nom+", couleur = "+evenements.couleur+", debut = "+evenements.debut+", duree = "+evenements.duree+", commentaire = "+evenements.commentaire+", interval-semaine = "+evenements.ecart+", jours = "+evenements.jour_semaine+", fin_repet = "+evenements.fin_repetition+", id_picto = "+evenements.pictogramme);
            afficherDialogEditEvent(evenements.id, evenements.nom, evenements.couleur, evenements.debut, evenements.duree, evenements.commentaire, evenements.jour_semaine, evenements.ecart, evenements.fin_repetition, evenements.pictogramme, pictogrammes);
        },
        error: function(xhr, status, error) {
            // Fonction à exécuter en cas d'erreur de la requête
            console.error("Erreur lors de la requête :", error);
        }
    });
}

function deleteEvent(idEvent){
    var div_event = document.getElementById("div_event"+idEvent);
    var titre = document.getElementById("titre"+idEvent).textContent;

    div_event.style.display = 'none';
    
    if(div_event.style.display == 'none'){
        $.ajax({
            type: "POST",
            url: "bdd.php",
            data: {
                action: 'deleteEvent',
                titre : titre
            },
            success: function(response) {
                // Fonction à exécuter lorsque la requête réussit
                //console.log("Requête réussie !");
                // console.log(response); // Affiche la réponse renvoyée par le serveur
                updateHorlogeEvent();
                afficherPictogrammes();
            },
            error: function(xhr, status, error) {
                // Fonction à exécuter en cas d'erreur de la requête
                console.error("Erreur lors de la requête :", error);
            }
        });
    }
}

function updateHorlogeEvent(){
    $.ajax({
        type: "POST",
        url: "bdd.php",
        data: {
            action: 'getAllEvent',
        },
        success: function(response) {
            // Fonction à exécuter lorsque la requête réussit
            // console.log("Requête réussie !");
            var data = JSON.parse(response);
            // console.log(data); // Affiche la réponse renvoyée par le serveur

            var donnees = [], couleur = [];
            var count = 0;

            for (var i = 0; i < data.length; i++) {
                // Ajouter le début de l'événement actuel
                if (i === 0) {
                    donnees.push(parseInt(data[i]['debut']));
                    donnees.push(parseInt(data[i]['duree']));
                    count = parseInt(data[i]['debut']) + parseInt(data[i]['duree']);
                }

                // Si ce n'est pas le premier événement, calculer la différence avec la fin de l'événement précédent
                if (i > 0) {
                    donnees.push(parseInt(data[i]['debut']) - parseInt(data[i - 1]['debut']) - parseInt(data[i - 1]['duree']));
                    donnees.push(parseInt(data[i]['duree']));
                    count += parseInt(data[i]['debut']) - parseInt(data[i - 1]['debut']) - parseInt(data[i - 1]['duree']) + parseInt(data[i]['duree']);
                }

                // Ajouter le temps restant à la fin de la journée si c'est le dernier événement
                if (i === data.length - 1) {
                    donnees.push(1440 - count);
                }
            }

            for (var i = 0; i < data.length; i++) { 
                couleur.push('rgba(0, 255, 0, 0)');
            }

            // update les données
            myDoughnutChart.data.datasets[0].backgroundColor = couleur;
            myDoughnutChart.data.datasets[0].data = donnees;
            myDoughnutChart.update();

            // Dessiner les textures hachurées
            var patternCanvas1 = document.createElement('canvas');
            var patternContext1 = patternCanvas1.getContext('2d');
            patternCanvas1.width = 10;
            patternCanvas1.height = 10;

            patternContext1.lineWidth = 7.5; // Épaisseur du trait
            patternContext1.strokeStyle = 'rgba(0, 0, 0, 1)';
            patternContext1.beginPath();
            patternContext1.moveTo(0, 0);
            patternContext1.lineTo(10, 0);
            patternContext1.stroke();

            var pattern1 = document.getElementById("myDoughnutChart").getContext('2d').createPattern(patternCanvas1, 'repeat');

            var patternCanvas2 = document.createElement('canvas');
            var patternContext2 = patternCanvas2.getContext('2d');
            patternCanvas2.width = 10;
            patternCanvas2.height = 10;

            patternContext2.lineWidth = 7.5; // Épaisseur du trait
            patternContext2.strokeStyle = 'rgba(0, 0, 0, 1)';
            patternContext2.beginPath();
            patternContext2.moveTo(0, 0);
            patternContext2.lineTo(0, 10);
            patternContext2.stroke();

            var pattern2 = document.getElementById("myDoughnutChart").getContext('2d').createPattern(patternCanvas2, 'repeat');

            for (var i = 1; i < donnees.length; i += 2) {
                myDoughnutChart.data.datasets[0].backgroundColor[i] = (i % 4 === 1) ? pattern1 : pattern2;
            }
            myDoughnutChart.update();
        },
        error: function(xhr, status, error) {
            // Fonction à exécuter en cas d'erreur de la requête
            console.error("Erreur lors de la requête :", error);
        }
    });
}

/* affichage des icones lier au event */ //pour la création
document.addEventListener("DOMContentLoaded", function() {
    const pictogrammes = document.getElementById("pictogrammes");
    const affichage_img_picto = document.getElementById("affichage_img_picto");

    pictogrammes.addEventListener("change", function() {
        const selectedOption = pictogrammes.options[pictogrammes.selectedIndex];
        
        if (selectedOption.title !== '') {
            const selectedImage = selectedOption.title;
            affichage_img_picto.innerHTML = `${selectedImage}`;
        }else{
            affichage_img_picto.innerHTML = ``;
        }
    });
});



function afficherDialogEditEvent(id, titre, couleur, debut_minute, duree_minute, commentaire, jour_semaine, interval_semaine, date_fin, id_picto, pictogrammes, audio, odeur) {
    var dialog = document.querySelector("dialog");
    codeHTML = '';
    var lundi, mardi, mercredi, jeudi, vendredi, samedi, dimanche = ' ';
    var debut_minute = parseInt(debut_minute); // Conversion en nombre
    var duree_minute = parseInt(duree_minute); // Conversion en nombre

    var debut = `${Math.floor(debut_minute/60).toString().padStart(2, '0')}:${Math.floor(debut_minute%60).toString().padStart(2, '0')}`;
    var fin = `${Math.floor((debut_minute+duree_minute)/60).toString().padStart(2, '0')}:${Math.floor((debut_minute+duree_minute)%60).toString().padStart(2, '0')}`;

    if(jour_semaine[0] == 1){lundi='checked'}
    if(jour_semaine[1] == 1){mardi='checked'}
    if(jour_semaine[2] == 1){mercredi='checked'}
    if(jour_semaine[3] == 1){jeudi='checked'}
    if(jour_semaine[4] == 1){vendredi='checked'}
    if(jour_semaine[5] == 1){samedi='checked'}
    if(jour_semaine[6] == 1){dimanche='checked'}
    
    codeHTML = `
        <div class="div-event">
            <div><p>Modification de l'événement</p></div>
            <form method="POST" action="">
                <div>
                    <label for="titre">Nom et couleur de l'événement :</label>
                    <input name="titre" type="text" value="`+titre+`"><input type="color" name="couleur" value="`+couleur+`">
                    <div>
                        <label for="debut">Heure de début :</label>
                        <input name="debut" type="time" value=`+debut+`>
                
                        <label for="fin">Heure de fin :</label>
                        <input name="fin" type="time" value=`+fin+`>
                    </div>
                    <label for="description">Description :</label>
                    <textarea name="description" style="height:100px">`+commentaire+`</textarea>
                </div>
                
                <div>
                    <label for="pictogrammes">Pictogrammes :</label><br>
                    <div class="pictogrammes">
                        <select id='pictogrammesEdit' name='pictogrammes'  size='4'>
                            <option value=''>--choisiser un pictogrammes--</option>`;
                            for (var i = 1; i <= pictogrammes.length; i++) {
                                if(i==id_picto){
                                    var selectionner = 'selected';
                                }else {
                                    selectionner = '';
                                }
                                codeHTML += "<option value='" + i + "' "+selectionner+" title='" + pictogrammes[i - 1]['image'] + "'>" + pictogrammes[i - 1]['nom'] + "</option>";
                            }
                            codeHTML+=`
                        </select>
                        <div id="affichage_img_picto_edit"></div>
                    </div>
                </div>

                <div>
                    <label for="jours">Répéter :</label>
                    <div class="jours-semaine-selecteur">
                        <input type="checkbox" id="Lundi-edit" name="Lundi-edit" class="semaine" `+lundi+`/>
                        <label for="Lundi-edit">Lun</label>
                        <input type="checkbox" id="Mardi-edit" name="Mardi-edit" class="semaine" `+mardi+`/>
                        <label for="Mardi-edit">Mar</label>
                        <input type="checkbox" id="Mercredi-edit" name="Mercredi-edit" class="semaine" `+mercredi+` />
                        <label for="Mercredi-edit">Mer</label>
                        <input type="checkbox" id="jeudi-edit" name="Jeudi-edit" class="semaine" `+jeudi+`/>
                        <label for="jeudi-edit">Jeu</label>
                        <input type="checkbox" id="Vendredi-edit" name="Vendredi-edit" class="semaine" `+vendredi+`/>
                        <label for="Vendredi-edit">Ven</label>
                        <input type="checkbox" id="Samedi-edit" name="Samedi-edit" class="semaine" `+samedi+`/>
                        <label for="Samedi-edit">Sam</label>
                        <input type="checkbox" id="Dimanche-edit" name="Dimanche-edit" class="semaine" `+dimanche+`/>
                        <label for="Dimanche-edit">Dim</label>
                    </div>
                </div>

                <div style="margin-bottom: 10px;">
                    <label for="number">Tous les :</label>
                    <input type="number" id="number" name="interval-semaine" min="0" value="`+interval_semaine+`">
                    <label>semaines</label><br>
                    <label id="information"><img src="images/warning.png" style="width: 10px; height: 10px; color: #FF0000;"> Inserer "0" pour répéter toute les semaines</label>
                </div> 

                <div>
                    <label for="date">Jusqu'au :</label>
                    <input type="date" id="date" name="date-fin" value=`+date_fin+`>
                </div>
				<div>
					<input type="checkbox" id="audio-edit" name="audio-edit" ${audio == 1 ? 'checked' : ''}/>
					<label for="audio-edit">audio</label>
				</div>
        
				<div>
					<input type="checkbox" id="odeur-edit" name="odeur-edit" ${odeur == 1 ? 'checked' : ''}/>
					<label for="odeur-edit">odeur</label>
				</div>
                </form>
            <button onclick="pushEditEvent(`+id+`)">Modifier l'événement</button>
            <div class="Event_err_modif" id="Event_err_modif"></div>
        </div>
        `;
    dialog.innerHTML = codeHTML;
    dialog.showModal();

    /* affichage des icones lier au event */ //pour la modification
    const pictogrammesEdit = document.getElementById("pictogrammesEdit");
    const affichage_img_picto_edit = document.getElementById("affichage_img_picto_edit");
    affichage_img_picto_edit.innerHTML = pictogrammesEdit.options[pictogrammesEdit.selectedIndex].title;

    pictogrammesEdit.addEventListener("change", function() {
        const selectedOptionEdit = pictogrammesEdit.options[pictogrammesEdit.selectedIndex];
        // console.log("test");
        if (selectedOptionEdit.title !== '') {
            const selectedImageEdit = selectedOptionEdit.title;
            affichage_img_picto_edit.innerHTML = `${selectedImageEdit}`;
        }else{
            affichage_img_picto_edit.innerHTML = ``;
        }
    });
}

function pushEditEvent(id_event){
    var titre = document.getElementsByName("titre")[1].value;
    var couleur = document.getElementsByName("couleur")[1].value;
    var heure_debut = document.getElementsByName("debut")[1].value;
    var heure_fin = document.getElementsByName("fin")[1].value;
    var description = document.getElementsByName("description")[1].value;
    var pictogramme = document.getElementsByName("pictogrammes")[1].value;
    if (document.getElementsByName("Lundi-edit")[0].checked){var lundi = "1";}else{var lundi = "0";}
    if (document.getElementsByName("Mardi-edit")[0].checked){var mardi = "1";}else{var mardi = "0";}
    if (document.getElementsByName("Mercredi-edit")[0].checked){var mercredi = "1";}else{var mercredi = "0";}
    if (document.getElementsByName("Jeudi-edit")[0].checked){var jeudi = "1";}else{var jeudi = "0";}
    if (document.getElementsByName("Vendredi-edit")[0].checked){var vendredi = "1";}else{var vendredi = "0";}
    if (document.getElementsByName("Samedi-edit")[0].checked){var samedi = "1";}else{var samedi = "0";}
    if (document.getElementsByName("Dimanche-edit")[0].checked){var dimanche = "1";}else{var dimanche = "0";}
    var jour_semaine = lundi+mardi+mercredi+jeudi+vendredi+samedi+dimanche;
    var interval_semaine = document.getElementsByName("interval-semaine")[1].value;
    var date_fin = document.getElementsByName("date-fin")[1].value;
    var audio = document.getElementById("audio").checked ? "1" : "0";
    var odeur = document.getElementById("odeur").checked ? "1" : "0";
    var err = document.getElementById("Event_err_modif");

    // console.log("titre = ", titre);
    // console.log("couleur = ", couleur);
    // console.log("heure_debut = ", heure_debut);
    // console.log("heure_fin = ", heure_fin);
    // console.log("description = ", description);
    // console.log("pictogramme = ", pictogramme);
    // console.log("jour_semaine = ", jour_semaine);
    // console.log("interval_semaine = ", interval_semaine);
    // console.log("date_fin = ", date_fin);

    if (titre == "" || couleur == "" || heure_debut == "" || heure_fin == "" || description == "" || pictogramme == "" || date == ""){
        err.textContent = "Il faut que tous les champs soient complétés";
        err.style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            err.textContent = "";
            err.style.border = "";
        }, 5000);
    } else if (heure_debut > heure_fin || heureDebut == heure_fin) {
        err.textContent = "L'événement a une durée négative ou nulle";
        err.style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            err.textContent = "";
            err.style.border = "";
        }, 5000);
    } else if (couleur == "#a6a6a6") {
        err.textContent = "Vous devez personnaliser la couleur";
        err.style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            err.textContent = "";
            err.style.border = "";
        }, 5000);
    } else {
        $.ajax({
            type: "POST",
            url: "bdd.php",
            data: {
                action: 'editEvent',
                id: id_event,
                ancien_titre: ancien_titre,
                titre : titre,
                couleur: couleur,
                heure_debut: heure_debut,
                heure_fin: heure_fin,
                description: description,
                pictogramme: pictogramme,
                jour_semaine: jour_semaine,
                interval_semaine: interval_semaine,
                date_fin: date_fin,
                audio: audio,
				odeur: odeur
            },
            success: function(response) {
                // Fonction à exécuter lorsque la requête réussit
                // console.log("Requête réussie !");
                // console.log(response); // Affiche la réponse renvoyée par le serveur
                if(response != "1"){
                    err.textContent = response;
                    err.style.border = "red solid 1px";
                    setTimeout(function() {
                        // Permet d'éffacer le message apres 5 seconde
                        err.textContent = "";
                        err.style.border = "";
                    }, 5000);
                } else {
                    updateHorlogeEvent();
                    afficherPictogrammes();
                    afficherEvent();
                    var dialog = document.querySelector("dialog");
                    dialog.close();
                }
            },
            error: function(xhr, status, error) {
                // Fonction à exécuter en cas d'erreur de la requête
                console.error("Erreur lors de la requête :", error);
            }
        });
    }
}

function ajouterEvent(){
    var titre = document.getElementById("titre").value;
    var couleur = document.getElementById("couleur").value;
    var heureDebut = document.getElementById("debut").value;
    var heureFin = document.getElementById("fin").value;
    var description = document.getElementById("description").value;
    var pictogramme = document.getElementById("pictogrammes").value;
    if (document.getElementById("Lundi").checked){var lundi = "1";}else{var lundi = "0";}
    if (document.getElementById("Mardi").checked){var mardi = "1";}else{var mardi = "0";}
    if (document.getElementById("Mercredi").checked){var mercredi = "1";}else{var mercredi = "0";}
    if (document.getElementById("jeudi").checked){var jeudi = "1";}else{var jeudi = "0";}
    if (document.getElementById("Vendredi").checked){var vendredi = "1";}else{var vendredi = "0";}
    if (document.getElementById("Samedi").checked){var samedi = "1";}else{var samedi = "0";}
    if (document.getElementById("Dimanche").checked){var dimanche = "1";}else{var dimanche = "0";}
    var jour_semaine = lundi+mardi+mercredi+jeudi+vendredi+samedi+dimanche;
    var interval_semaine =document.getElementById("number").value;
    var date = document.getElementById("date").value;
    var err = document.getElementById("Event_err");
    var audio = document.getElementById("audio").checked ? "1" : "0";
    var odeur = document.getElementById("odeur").checked ? "1" : "0";
    
    // console.log("titre = ", titre);
    // console.log("couleur = ", couleur);
    // console.log("heure_debut = ", heureDebut);
    // console.log("heure_fin = ", heureFin);
    // console.log("description = ", description);
    // console.log("pictogramme = ", pictogramme);
    // console.log("jour_semaine = ", jour_semaine);
    // console.log("interval_semaine = ", interval_semaine);
    // console.log("date_fin = ", date);

    if (titre == "" || couleur == "" || heureDebut == "" || heureFin == "" || description == "" || pictogramme == "" || date == ""){
        err.textContent = "Il faut que tous les champs soient complétés";
        err.style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            err.textContent = "";
            err.style.border = "";
        }, 5000);
    } else if (heureDebut > heureFin || heureDebut == heureFin) {
        err.textContent = "L'événement a une durée négative ou nulle";
        err.style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            err.textContent = "";
            err.style.border = "";
        }, 5000);
    } else if (couleur == "#a6a6a6") {
        err.textContent = "Vous devez personnaliser la couleur";
        err.style.border = "red solid 1px";
        setTimeout(function() {
            // Permet d'éffacer le message apres 5 seconde
            err.textContent = "";
            err.style.border = "";
        }, 5000);
    } else {
        $.ajax({
            type: "POST",
            url: "bdd.php",
            data: {
                action: 'ajouterEvent',
                titre : titre,
                couleur: couleur,
                heureDebut: heureDebut,
                heureFin: heureFin,
                description: description,
                pictogramme: pictogramme,
                jour_semaine: jour_semaine,
                interval_semaine: interval_semaine,
                date_fin: date,
                audio: audio,
				odeur: odeur
            },
            success: function(response) {
                // Fonction à exécuter lorsque la requête réussit
                // console.log("Requête réussie !");
                // console.log(response); // Affiche la réponse renvoyée par le serveur
                if(response != "1"){
                    err.textContent = response;
                    err.style.border = "red solid 1px";
                    setTimeout(function() {
                        // Permet d'éffacer le message apres 5 seconde
                        err.textContent = "";
                        err.style.border = "";
                    }, 5000);
                }else{
                    afficherPictogrammes();
                    afficherEvent();
    
                    // remise à zero du form
                    document.getElementById("titre").value = "";
                    document.getElementById("couleur").value = "#a6a6a6";
                    document.getElementById("debut").value = "";
                    document.getElementById("fin").value = "";
                    document.getElementById("description").value = "";
                    document.getElementById("pictogrammes").value = "";
                    document.getElementById("affichage_img_picto").innerHTML = "";
                    document.getElementById("Lundi").checked = "";
                    document.getElementById("Mardi").checked = "";
                    document.getElementById("Mercredi").checked = "";
                    document.getElementById("jeudi").checked = "";
                    document.getElementById("Vendredi").checked = "";
                    document.getElementById("Samedi").checked = "";
                    document.getElementById("Dimanche").checked = "";
                    interval_semaine =document.getElementById("number").value = "";
                    document.getElementById("date").value = "";
                }
            },
            error: function(xhr, status, error) {
                // Fonction à exécuter en cas d'erreur de la requête
                console.error("Erreur lors de la requête :", error);
            }
        });
    }
}

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
            // Fonction à exécuter lorsque la requête réussit
            // console.log("Requête réussie !");
            // console.log(response); // Affiche la réponse renvoyée par le serveur
            afficherPictogrammes.innerHTML = '';
            afficherPictogrammes.innerHTML = response;
            afficherpictogrammes.find('script').each(function() {
                eval(this.innerHTML);
            });
            updateHorlogeEvent();
        },
        error: function(xhr, status, error) {
            // Fonction à exécuter en cas d'erreur de la requête
            console.error("Erreur lors de la requête :", error);
        }
    });
}

function afficherEvent(){
    var afficherEvent = document.getElementById("div_event");
    $.ajax({
        type: "POST",
        url: "bdd.php",
        data: {
            action: 'afficherEvent'
        },
        success: function(response) {
            // Fonction à exécuter lorsque la requête réussit
            // console.log("Requête réussie !");
            // console.log(response); // Affiche la réponse renvoyée par le serveur
            afficherEvent.innerHTML = '';
            if (response != "erreur"){
                afficherEvent.innerHTML = response;
            }
        },
        error: function(xhr, status, error) {
            // Fonction à exécuter en cas d'erreur de la requête
            afficherEvent.innerHTML = '';
            console.error("Erreur lors de la requête :", error);
        }
    });
}