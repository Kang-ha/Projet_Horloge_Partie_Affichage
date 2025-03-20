document.addEventListener("DOMContentLoaded", function() {
    const numbersContainer = document.getElementById("nombre");
    const radius = 175;
    // Créer et positionner les chiffres autour du cadran de l'horloge
    for (let i = 0; i < 24; i++) {
        let angle = (i * 15) - 90;
        let x = 200 + radius * Math.cos(angle * Math.PI / 180);
        let y = 200 + radius * Math.sin(angle * Math.PI / 180);
        let numberElement = document.createElement("div");
        numberElement.style.position = "absolute";
        numberElement.style.top = `${y}px`;
        numberElement.style.left = `${x}px`;
        numberElement.style.transform = "translate(-50%, -50%)"; 
        numberElement.textContent = i;
        numbersContainer.appendChild(numberElement);
    }

    // Fonction de mise à jour des aiguilles de l'horloge et de l'affichage numérique de l'heure
    function updateClock() {
        const now = new Date();
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const seconds = now.getSeconds();

        // Mise à jour de l'horloge numérique
        document.getElementById('clock').textContent = `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;

        // Calculer les degrés de rotation pour chaque hand
        const hourDeg = (hours % 24) * 15 + (minutes / 4);
        const minuteDeg = (minutes * 6) + (seconds / 10);
        const secondDeg = seconds * 6;

        // Rotate the clock hands
        document.getElementById("heure").style.transform = `rotate(${hourDeg}deg)`;
        document.getElementById("min").style.transform = `rotate(${minuteDeg}deg)`;
        document.getElementById("sec").style.transform = `rotate(${secondDeg}deg)`;
    }
    setInterval(updateClock, 1000); // Mise à jour de l'horloge toutes les secondes
    updateClock(); // Appel initial pour une mise à l'heure immédiate

    // Tableau des jours de la semaine
    const jours = ["Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"];
    const aujourdHui = new Date().getDay();
    // Affichage du jour en cours
    document.getElementById('jour-actuel').innerText = "Aujourd'hui, c'est " + jours[aujourdHui-1] + ".";

    const sectionJour = document.getElementById("section-jour");
    // Créer et ajouter des éléments de jour à la section
    jours.forEach((jour, index) => {
        let jourDiv = document.createElement("div");
        jourDiv.classList.add("section-item");
        if (index === aujourdHui-1) {
            jourDiv.classList.add("active");
        }
        jourDiv.innerText = jour;
        sectionJour.appendChild(jourDiv);
    });

    // Fonction pour dessiner un camembert
    function pie(ctx, w, h, datalist, colist) {
        var radius = h / 2 - 5;
        var centerx = w / 2;
        var centery = h / 2;
        var total = datalist.reduce((a, b) => a + b, 0);
        var lastend = 0;
        var offset = Math.PI / 2;
      
        // Dessinez chaque segment du camembert
        for (let x = 0; x < datalist.length; x++) {
            var thispart = datalist[x]; 
            var arcsector = Math.PI * (2 * thispart / total);
            ctx.beginPath();
            ctx.fillStyle = colist[x];
            ctx.moveTo(centerx, centery);
            ctx.arc(centerx, centery, radius, lastend - offset, lastend + arcsector - offset, false);
            ctx.lineTo(centerx, centery);
            ctx.fill();
            ctx.closePath();
            lastend += arcsector;
        }
    }

    var datalist = [33.333, 16.666, 8.333, 16.666, 12.5, 12.5]; 
    var colist = ['blue', 'green', 'yellow', 'orange', 'red', 'blue'];
    var canvas = document.getElementById("canvas3"); 
    if (canvas) {
        var ctx = canvas.getContext('2d');
        pie(ctx, canvas.width, canvas.height, datalist, colist);
    }
});
