<?php
    /**
     * @file horloge/bdd.php
     */
    require_once("../config.php");
    session_start();
    //lien vers base de données
    $lien = null;

    //Connexion à la base de données
    function bddConnect() {
        global $lien;
        try {
            $lien = new PDO("mysql:dbname=".BDD.";charset=utf8",USER, PWD,
                            array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION));
        } catch (Exception $e) {
            echo ("Base de données en maintenance");
            $lien = null;
            die();
        }
    }

    /*****************************************************************************/
    /*                               REQUETTE AJAX                               */
    /*****************************************************************************/

    if (isset($_POST['action'])) {
        $action = $_POST['action'];
        // $action contient le nom de la fonction à exécuter
        
        /**
         * Traitement de l'action 'getPeriode'
         * Récupère la période en fonction du temps en minutes et de l'ID de l'horloge.
         * Exige que 'temps_minute' soit fourni dans la requête POST.
         */
        if ($action == 'getPeriode') {
            if (isset($_POST['temps_minute'])) {
                $temps_minute = $_POST['temps_minute'];
                $id_horloge = $_SESSION['id_horloge'];
                $result = getPeriode($id_horloge, $temps_minute);
                echo $result;
            }
        }
    
        /**
         * Traitement de l'action 'getEvent'
         * Récupère un événement spécifique en fonction de son titre.
         * Exige que 'titre' soit fourni dans la requête POST.
         */
        if ($action == 'getEvent') {
            if (isset($_POST['titre'])) {
                $titre = $_POST['titre'];
                $result = getEvent($titre);
                echo $result;
            }
        }
    
        /**
         * Traitement de l'action 'getAllEvent'
         * Récupère tous les événements.
         */
        if ($action == 'getAllEvent') {
            $result = getAllEvent();
            echo $result;
        }
    
        /**
         * Traitement de l'action 'afficherEvent'
         * Affiche les événements.
         */
        if ($action == 'afficherEvent') {
            $result = afficherEvent();
            echo $result;
        }
    
        /**
         * Traitement de l'action 'ajouterEvent'
         * Ajoute un nouvel événement avec les paramètres spécifiés.
         * Exige que plusieurs paramètres soient fournis dans la requête POST, tels que 'titre', 'couleur', 'heureDebut', 'heureFin', 'description', 'pictogramme', 'jour_semaine', 'interval_semaine', et 'date_fin'.
         */
        if ($action == 'ajouterEvent') {
            if (isset($_POST['titre'])) {
                $titre = $_POST['titre'];
                $couleur = $_POST['couleur'];
                $heure_debut = $_POST['heureDebut'];
                $heure_fin = $_POST['heureFin'];
                $description = $_POST['description'];
                $pictogramme = $_POST['pictogramme'];
                $jour_semaine = $_POST['jour_semaine'];
                $interval_semaine = $_POST['interval_semaine'];
                $date_fin = $_POST['date_fin'];
				$audio = !empty($_POST['audio']) ? 1 : 0;
				$odeur = !empty($_POST['odeur']) ? 1 : 0;
                
                $result = ajouterEvent($titre, $couleur, $heure_debut, $heure_fin, $description, $pictogramme, $jour_semaine, $interval_semaine, $date_fin, $audio, $odeur);
                echo $result;
            }
        }
    
        /**
         * Traitement de l'action 'afficherPictogrammes'
         * Affiche les pictogrammes disponibles.
         */
        if ($action == 'afficherPictogrammes') {
            $result = afficherPictogrammes();
            echo $result;
        }
    
        /**
         * Traitement de l'action 'editEvent'
         * Modifie un événement existant avec les paramètres spécifiés.
         * Exige que plusieurs paramètres soient fournis dans la requête POST, tels que 'id', 'titre', 'ancien_titre', 'couleur', 'heure_debut', 'heure_fin', 'description', 'pictogramme', 'jour_semaine', 'interval_semaine', et 'date_fin'.
         */
        if ($action == 'editEvent') {
            if (isset($_POST['id'])) {
                $id = $_POST['id'];
                $titre = $_POST['titre'];
                $ancien_titre = $_POST['ancien_titre'];
                $couleur = $_POST['couleur'];
                $heure_debut = $_POST['heure_debut'];
                $heure_fin = $_POST['heure_fin'];
                $description = $_POST['description'];
                $pictogramme = $_POST['pictogramme'];
                $jour_semaine = $_POST['jour_semaine'];
                $interval_semaine = $_POST['interval_semaine'];
                $date_fin = $_POST['date_fin'];
                $audio = $_POST['audio'];
                $odeur = $_POST['odeur'];
    
                $result = editEvent($id, $titre, $ancien_titre, $couleur, $heure_debut, $heure_fin, $description, $pictogramme, $jour_semaine, $interval_semaine, $date_fin, $audio, $odeur);
                echo $result;
            }
        }
    
        /**
         * Traitement de l'action 'deleteEvent'
         * Supprime un événement spécifique en fonction de son titre.
         * Exige que 'titre' soit fourni dans la requête POST.
         */
        if ($action == 'deleteEvent') {
            if (isset($_POST['titre'])) {
                $titre = $_POST['titre'];
                $result = deleteEvent($titre);
                echo $result;
            }
        }
    
        /**
         * Traitement de l'action 'updateHoraireHorloge'
         * Met à jour l'horaire de l'horloge avec les paramètres spécifiés.
         * Exige que plusieurs paramètres soient fournis dans la requête POST, tels que 'matin1', 'couleur_matin', 'midi1', 'couleur_midi', 'apres_midi1', 'couleur_apres_midi', 'soir1', 'couleur_soir', et 'nuit1'.
         */
        if ($action == 'updateHoraireHorloge') {
            if (isset($_POST['matin1'])) {
                $matin1 = $_POST['matin1'];
                $couleur_matin = $_POST['couleur_matin'];
                $midi1 = $_POST['midi1'];
                $couleur_midi = $_POST['couleur_midi'];
                $apres_midi1 = $_POST['apres_midi1'];
                $couleur_apres_midi = $_POST['couleur_apres_midi'];
                $soir1 = $_POST['soir1'];
                $couleur_soir = $_POST['couleur_soir'];
                $nuit1 = $_POST['nuit1'];
                $couleur_nuit = $_POST['couleur_nuit'];
                
                $result = updateHoraireHorloge($matin1, $couleur_matin, $midi1, $couleur_midi, $apres_midi1, $couleur_apres_midi, $soir1, $couleur_soir, $nuit1, $couleur_nuit);
                echo $result;
            }
        }
    }
    

    /*****************************************************************************/
    /*                     FONCTION AFFICHAGE DES FORMULAIRES                    */
    /*****************************************************************************/

    /**
     * Affiche le tableau des périodes pour une horloge donnée.
     *
     * @param int $id_horloge L'identifiant de l'horloge.
     */
    function afficherTableauPeriode($id_horloge){
        global $lien;
        try {
            // Requête pour sélectionner les périodes et les configurations associées à l'utilisateur.
            $req = "SELECT * FROM periodes JOIN configurations ON periodes.id_configuration = configurations.id WHERE configurations.id_utilisateur = '$id_horloge' ORDER BY debut ASC";
            $res = $lien->query($req);
            
            $donnees = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau.
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }

            // Formater les périodes et leurs couleurs respectives.
            $matin = sprintf("%02d", floor($donnees[1]['debut'] / 60)) . ":" . (sprintf("%02d", $donnees[1]['debut'] % 60));
            $couleur_matin = $donnees[1]['couleur'];

            $midi = sprintf("%02d", floor($donnees[2]['debut'] / 60)) . ":" . (sprintf("%02d", $donnees[2]['debut'] % 60));
            $couleur_midi = $donnees[2]['couleur'];

            $apres_midi = sprintf("%02d", floor($donnees[3]['debut'] / 60)) . ":" . (sprintf("%02d", $donnees[3]['debut'] % 60));
            $couleur_apres_midi = $donnees[3]['couleur'];

            $soir = sprintf("%02d", floor($donnees[4]['debut'] / 60)) . ":" . (sprintf("%02d", $donnees[4]['debut'] % 60));
            $couleur_soir = $donnees[4]['couleur'];

            $nuit = sprintf("%02d", floor($donnees[5]['debut'] / 60)) . ":" . (sprintf("%02d", $donnees[5]['debut'] % 60));
            $couleur_nuit = $donnees[5]['couleur'];

            // Afficher chaque période avec ses valeurs formatées.
            lignePeriode("matin", $matin, $couleur_matin);
            lignePeriode("midi", $midi, $couleur_midi);
            lignePeriode("apres_midi", $apres_midi, $couleur_apres_midi);
            lignePeriode("soir", $soir, $couleur_soir);
            lignePeriode("nuit", $nuit, $couleur_nuit);
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Affiche une ligne de période avec le titre, la valeur et la couleur spécifiés.
     *
     * @param string $titre Le titre de la période.
     * @param string $valeur La valeur de la période au format hh:mm.
     * @param string $couleur La couleur associée à la période.
     */
    function lignePeriode($titre, $valeur, $couleur){
        // Traduire le titre pour l'affichage.
        if ($titre == "matin") {$titre2 = "Matin";}
        if ($titre == "midi") {$titre2 = "Midi";}
        if ($titre == "apres_midi") {$titre2 = "Après midi";}
        if ($titre == "soir") {$titre2 = "Soir";}
        if ($titre == "nuit") {$titre2 = "Nuit";}

        echo "<div>\n";
        echo "\t<label class='label-tab' id='titre_".$titre."'>".$titre2." :</label>\n";
        echo "\t<input name='".$titre."1' id='".$titre."1' value='".$valeur."' type='time'>\n";
        echo "\t<input type='color' id='couleur_".$titre."' name='couleur_".$titre."' value=".$couleur." class='select-tab'>\n";
        echo "</div>\n";
    }

    /**
     * Affiche une liste de pictogrammes disponibles.
     *
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function afficherListePictogrammes(){
        global $lien;
        $nbLigne = 0;
        try {
            // Requête pour sélectionner tous les pictogrammes.
            $req = "SELECT * FROM `pictogrammes`";
            $res = $lien->query($req);

            $donnees = array();
        
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau.
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }
            
            // Afficher les options du select pour les pictogrammes.
            echo "<select id='pictogrammes' name='pictogrammes' size='4'>";
            echo "<option value=''>--choisissez un pictogramme--</option>\n";
            for ($i = 1; $i <= $nbLigne; $i++) { 
                echo "<option value='".$i."' title='".$donnees[$i-1]['image']."'>".$donnees[$i-1]['nom']."</option>\n";
            }
            echo "</select>";

            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
            return 0;
        }
    }

    /*****************************************************************************/
    /*                         FONCTION LIER AUX PERIODES                        */
    /*****************************************************************************/
    
    /**
     * Affiche l'horloge avec les périodes et couleurs associées pour une horloge donnée.
     *
     * @param int $id_horloge L'identifiant de l'horloge.
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function afficherHorloge($id_horloge) {
        global $lien;
        try {
            // Requête pour sélectionner les périodes et les configurations associées à l'utilisateur.
            $req = "SELECT * FROM periodes JOIN configurations ON configurations.id = periodes.id_configuration WHERE configurations.id_utilisateur = '$id_horloge' ORDER BY debut ASC;";
            $res = $lien->query($req);
            
            $donnees = array();
            
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau.
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }
            
            //print_r($donnees);

            // Extraction et formatage des données pour chaque période.
            $labelMatin = $donnees[1]['nom'];
            $d_matin = $donnees[1]['duree'] / 60;
            $couleurmatin = $donnees[1]['couleur'];
            
            $labelMidi = $donnees[2]['nom'];
            $d_midi = $donnees[2]['duree'] / 60;
            $couleurmidi = $donnees[2]['couleur'];
            
            $labelApresMidi = $donnees[3]['nom'];
            $d_apres_midi = $donnees[3]['duree'] / 60;
            $couleurapresmidi = $donnees[3]['couleur'];
            
            $labelSoir = $donnees[4]['nom'];
            $d_soir = $donnees[4]['duree'] / 60;
            $couleursoir = $donnees[4]['couleur'];
            
            $labelNuit = "nuit";
            $d_nuit_debut_journee = $donnees[0]['duree'] / 60;
            $d_nuit_fin_journee = $donnees[5]['duree'] / 60;
            $couleurnuit = $donnees[5]['couleur'];

            // Génération du script JavaScript pour afficher l'horloge.
            echo "<script>";
            echo "horloge('$labelNuit', $d_nuit_debut_journee, $d_nuit_fin_journee, '$couleurnuit', '$labelMatin', $d_matin, '$couleurmatin', '$labelMidi', $d_midi, '$couleurmidi', '$labelApresMidi', $d_apres_midi, '$couleurapresmidi', '$labelSoir', $d_soir, '$couleursoir');";
            echo "</script>\n";
            
            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
            return 0;
        }
    }

    /**
     * Met à jour les horaires de l'horloge pour les périodes spécifiées.
     *
     * @param string $matin1 L'heure de début du matin au format hh:mm.
     * @param string $couleur_matin La couleur de la période du matin.
     * @param string $midi1 L'heure de début du midi au format hh:mm.
     * @param string $couleur_midi La couleur de la période du midi.
     * @param string $apres_midi1 L'heure de début de l'après-midi au format hh:mm.
     * @param string $couleur_apres_midi La couleur de la période de l'après-midi.
     * @param string $soir1 L'heure de début du soir au format hh:mm.
     * @param string $couleur_soir La couleur de la période du soir.
     * @param string $nuit1 L'heure de début de la nuit au format hh:mm.
     * @param string $couleur_nuit La couleur de la période de la nuit.
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function updateHoraireHorloge($matin1, $couleur_matin, $midi1, $couleur_midi, $apres_midi1, $couleur_apres_midi, $soir1, $couleur_soir, $nuit1, $couleur_nuit) {
        global $lien;
        bddConnect();
        $id_horloge = $_SESSION['id_horloge'];
        try {
            // Conversion des horaires au format minutes depuis minuit.
            $matin = intval(explode(":", $matin1)[0]) * 60 + intval(explode(":", $matin1)[1]);
            $d_matin = (intval(explode(":", $midi1)[0]) * 60 + intval(explode(":", $midi1)[1])) - $matin;

            $midi = intval(explode(":", $midi1)[0]) * 60 + intval(explode(":", $midi1)[1]);
            $d_midi = (intval(explode(":", $apres_midi1)[0]) * 60 + intval(explode(":", $apres_midi1)[1])) - $midi;

            $apres_midi = intval(explode(":", $apres_midi1)[0]) * 60 + intval(explode(":", $apres_midi1)[1]);
            $d_apres_midi = (intval(explode(":", $soir1)[0]) * 60 + intval(explode(":", $soir1)[1])) - $apres_midi;

            $soir = intval(explode(":", $soir1)[0]) * 60 + intval(explode(":", $soir1)[1]);
            $d_soir = (intval(explode(":", $nuit1)[0]) * 60 + intval(explode(":", $nuit1)[1])) - $soir;

            $nuit_debut_journee = 0;
            $d_nuit_debut_journee = $matin;

            $nuit_fin_journee = intval(explode(":", $nuit1)[0]) * 60 + intval(explode(":", $nuit1)[1]);
            $d_nuit_fin_journee = 24 * 60 - $nuit_fin_journee;

            // Requêtes pour mettre à jour les périodes dans la base de données.
            $req = "UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration 
                    SET periodes.nom = 'nuit_debut_journee', periodes.debut = '$nuit_debut_journee', periodes.duree = '$d_nuit_debut_journee', periodes.couleur = '$couleur_nuit' 
                    WHERE configurations.id_utilisateur = '$id_horloge' AND periodes.nom = 'nuit_debut_journee';
                    
                    UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration 
                    SET periodes.nom = 'matin', periodes.debut = '$matin', periodes.duree = '$d_matin', periodes.couleur = '$couleur_matin' 
                    WHERE configurations.id_utilisateur = '$id_horloge' AND periodes.nom = 'matin';
                    
                    UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration 
                    SET periodes.nom = 'midi', periodes.debut = '$midi', periodes.duree = '$d_midi', periodes.couleur = '$couleur_midi' 
                    WHERE configurations.id_utilisateur = '$id_horloge' AND periodes.nom = 'midi';
                    
                    UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration 
                    SET periodes.nom = 'apres_midi', periodes.debut = '$apres_midi', periodes.duree = '$d_apres_midi', periodes.couleur = '$couleur_apres_midi' 
                    WHERE configurations.id_utilisateur = '$id_horloge' AND periodes.nom = 'apres_midi';
                    
                    UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration 
                    SET periodes.nom = 'soir', periodes.debut = '$soir', periodes.duree = '$d_soir', periodes.couleur = '$couleur_soir' 
                    WHERE configurations.id_utilisateur = '$id_horloge' AND periodes.nom = 'soir';
                    
                    UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration 
                    SET periodes.nom = 'nuit_fin_journee', periodes.debut = '$nuit_fin_journee', periodes.duree = '$d_nuit_fin_journee', periodes.couleur = '$couleur_nuit' 
                    WHERE configurations.id_utilisateur = '$id_horloge' AND periodes.nom = 'nuit_fin_journee';";

            // Exécution de la requête.
            $res = $lien->query($req);
            
            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
            return 0;
        }
    }

    /**
     * Récupère la période actuelle en fonction de l'heure donnée pour une horloge spécifiée.
     *
     * @param int $id_horloge L'identifiant de l'horloge.
     * @param int $temps_minute Le temps en minutes depuis minuit.
     * @return string Le nom de la période actuelle.
     */
    function getPeriode($id_horloge, $temps_minute) {
        global $lien;
        bddConnect();
        try {
            // Vérifiez que la connexion à la base de données est établie.
            if (!$lien) {
                throw new Exception("La connexion à la base de données n'est pas établie");
            }
            // Requête pour sélectionner les périodes et les configurations associées à l'utilisateur.
            $req = "SELECT * FROM periodes JOIN configurations on configurations.id = periodes.id_configuration WHERE configurations.id_utilisateur = '$id_horloge' ORDER BY debut ASC";
            $res = $lien->query($req);

            $donnees = array();
        
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau.
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }

            $periode_actuelle = null;

            // Détermination de la période actuelle en fonction du temps donné.
            for ($i = 0; $i < count($donnees); $i++) {
                if ($temps_minute >= $donnees[$i]["debut"] && $temps_minute < ($donnees[$i]["debut"] + $donnees[$i]["duree"])) {
                    $periode_actuelle = $donnees[$i]['nom'];
                    break;
                }
            }

            // Si aucune période n'a été trouvée, cela signifie que le temps est après la dernière période,
            // donc nous sommes dans la première période de la journée suivante.
            if ($periode_actuelle === null) {
                $periode_actuelle = $donnees[0]['nom'];
            }

            // Si la période est une période spéciale de nuit, la remplacer par "nuit".
            if ($periode_actuelle == "nuit_debut_journee" || $periode_actuelle == "nuit_fin_journee") {
                $periode_actuelle = "nuit";
            }
            return $periode_actuelle;

        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /*****************************************************************************/
    /*                        FONCTION LIER AUX EVENEMENT                        */
    /*****************************************************************************/

    /**
     * Ajoute un événement à l'horloge de l'utilisateur connecté.
     *
     * @param string $titre Le titre de l'événement.
     * @param string $couleur La couleur associée à l'événement.
     * @param string $heureDebut L'heure de début de l'événement au format hh:mm.
     * @param string $heureFin L'heure de fin de l'événement au format hh:mm.
     * @param string $description La description de l'événement.
     * @param string $pictogramme Le pictogramme associé à l'événement.
     * @param int $jour_semaine Le jour de la semaine pour l'événement (0 pour dimanche, 1 pour lundi, etc.).
     * @param int $interval_semaine L'intervalle de semaines pour la répétition de l'événement.
     * @param string $date_fin La date de fin de la répétition de l'événement au format YYYY-MM-DD.
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function ajouterEvent($titre, $couleur, $heureDebut, $heureFin, $description, $pictogramme, $jour_semaine, $interval_semaine, $date_fin, $audio, $odeur) {
        global $lien;
        $nbLigne = 0;
        bddConnect();
        $id_horloge = $_SESSION['id_horloge'];
        try {
            /* récup des données pour la verification */
            $req="SELECT nom, debut, duree FROM evenements WHERE id_utilisateur = '$id_horloge' ORDER BY debut ASC";
            $res = $lien->query($req);
            
            $donnees = array();
        
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }
            
            /* Les données à envoyer */
            $minuteDebut = intval(explode(":", $heureDebut)[0]) * 60 + intval(explode(":", $heureDebut)[1]);
            $minuteFin = intval(explode(":", $heureFin)[0]) * 60 + intval(explode(":", $heureFin)[1]);
            if ($minuteFin == 0) {
                $minuteFin = 1440;
            }
            
            //permet de gérer les ' pour prévenir des bugs avec les requetes
            //je rajoute un \avant le ' pour qu'il soit compté comme char sans sa fonction de fin de chaine de char
            $titre_verif = "";
            $description_verif = "";

            for ($i = 0; $i < strlen($description); $i++) {
                if ($description[$i] === "'") {
                    $description_verif .= "\\";
                }
                $description_verif .= $description[$i];
            }

            for ($i = 0; $i < strlen($titre); $i++) {
                if ($titre[$i] === "'") {
                    $titre_verif .= "\\";
                }
                $titre_verif .= $titre[$i];
            }
            /* vérif pour l'envoie des données */

            for ($i = 0; $i < $nbLigne; $i++) {
                if ($donnees[$i]['nom'] == $titre) {
                    echo "Un événement avec le même titre a déjà été enregistré";
                    return;
                }
                if ($minuteDebut >= $donnees[$i]['debut'] && $minuteDebut < $donnees[$i]['debut'] + $donnees[$i]['duree'] || $minuteDebut < $donnees[$i]['debut'] + $donnees[$i]['duree'] && $minuteFin > $donnees[$i]['debut']) {
                    echo "L'événement que vous essaiyer d'ajouter se chevaucge avec un autre";
                    return;
                }
            }
            
            //utilisation d'une durée et pas de le fin de l'event 
            $dureeEvent = $minuteFin - $minuteDebut;

            /* evoie si bon */
            $req="INSERT INTO evenements (id_utilisateur, nom, debut, duree, couleur, commentaire, ecart, jour_semaine, fin_repetition, audio, odeur, pictogramme) VALUES ('$id_horloge', '$titre_verif','$minuteDebut', '$dureeEvent', '$couleur', '$description_verif', '$interval_semaine', '$jour_semaine', '$date_fin', '$audio', '$odeur', '$pictogramme')";
            $res = $lien->query($req);
            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Affiche les événements pour l'utilisateur connecté.
     */
    function afficherEvent() {
        global $lien;
        $nbLigne=0;
        bddConnect();
        $id_horloge = $_SESSION['id_horloge'];
        try {
            $req="SELECT id, nom, debut, duree, commentaire FROM evenements WHERE id_utilisateur = $id_horloge ORDER BY debut ASC";
            $res = $lien->query($req);
            
            $donnees = array();
        
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }

            if ($nbLigne == 0){
                return "erreur";
            }

            for ($i=0; $i < $nbLigne; $i++) { 
                $fin = $donnees[$i]['debut'] + $donnees[$i]['duree'];
                $heure = intval($donnees[$i]['debut'] / 60) . "h" .  sprintf("%02d", $donnees[$i]['debut'] % 60) . " - " . intval($fin / 60) . "h" . sprintf("%02d", $fin % 60);
                
                echo "<div id='div_event".$i."' class='div_event'>\r";
                echo "\t\t\t\t\t\t<img src='images/edition.png' id='img_edit".$i."' onclick='editEvent(".$i.")' class='img-edit'>\r";
                echo "\t\t\t\t\t\t<img src='images/delet.png' id='img_delete".$i."' onclick='deleteEvent(".$i.")' class='img-delete'>\r";
                echo "\t\t\t\t\t\t<div contenteditable='false' lang='fr' id='titre".$i."' class='titre'>".$donnees[$i]['nom']."</div>\r";
                echo "\t\t\t\t\t\t<div contenteditable='false' lang='fr' id='heure".$i."' class='heure'>".$heure."</div>\r";
                echo "\t\t\t\t\t\t<div contenteditable='false' lang='fr' id='commentaire".$i."' class='commentaire'>".nl2br($donnees[$i]['commentaire'])."</div>\r";
                echo "\t\t\t\t\t</div>\r\t\t\t\t\t";
            }

        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Affiche les événements sur l'horloge pour une horloge spécifiée.
     *
     * @param int $id_horloge L'identifiant de l'horloge.
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function afficherHorlogeEvent($id_horloge) {
        global $lien;
        $nbLigne = 0;
        try {
            $req="SELECT nom, debut, duree, couleur FROM evenements WHERE id_utilisateur = '$id_horloge' ORDER BY debut ASC";
            $res = $lien->query($req);
            
            $donnees = array();
        
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }

            if ($nbLigne == 0){
                return "";
            }

            $count=0;
            for ($i = 0; $i < $nbLigne; $i++) {
                // Ajouter le début de l'événement actuel
                if ($i == 0) {
                    $datas[] = $donnees[$i]['debut'];
                    $datas[] = $donnees[$i]['duree'];
                    $count = $donnees[$i]['debut'] + $donnees[$i]['duree'];
                }
            
                // Si ce n'est pas le premier événement, calculer la différence avec la fin de l'événement précédent
                if ($i > 0) {
                    $datas[] = $donnees[$i]['debut'] - $donnees[$i-1]['debut'] - $donnees[$i-1]['duree'];
                    $datas[] = $donnees[$i]['duree'];
                    $count += $donnees[$i]['debut'] - $donnees[$i-1]['debut'] - $donnees[$i-1]['duree'] + $donnees[$i]['duree'];
                }

                if ($i == $nbLigne - 1) {
                    $datas[] = 1440 - $count;
                }
            }

            for ($i=0; $i < count($datas); $i++) { 
                $couleurs[] = 'rgba(0, 255, 0, 0)';
            }

            // Afficher les données générées
            echo "<script>";
            echo    "\r\t\t\t\t\t\tvar datas = ".json_encode($datas).";"; // Convertir le tableau PHP en JSON
            echo    "\r\t\t\t\t\t\tvar couleurs = ".json_encode($couleurs).";"; // Assurez-vous que $couleurs est une chaîne valide
            echo    "\r\t\t\t\t\t\thorlogeEvent(datas, couleurs);"; // Appel de la fonction JavaScript avec les données
            echo "\r\t\t\t\t\t</script>\r";
            
            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Récupère les détails d'un événement par titre et l'ensemble des pictogrammes.
     *
     * @param string $titre Le titre de l'événement.
     * @return string JSON contenant les détails de l'événement et les pictogrammes.
     */
    function getEvent($titre) {
        global $lien;
        bddConnect();
        $id_horloge = $_SESSION['id_horloge'];
        try {
            $req1 = "SELECT * FROM evenements WHERE nom = '$titre' AND id_utilisateur = '$id_horloge';";
            $res1 = $lien->query($req1);

            $donnees1 = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res1->fetch(PDO::FETCH_ASSOC)) {
                $donnees1[] = $row;
            }

            $req2 = "SELECT * FROM pictogrammes;";
            $res2 = $lien->query($req2);

            $donnees2 = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res2->fetch(PDO::FETCH_ASSOC)) {
                $donnees2[] = $row;
            }

            return json_encode(array("evenements" => $donnees1, "pictogrammes" => $donnees2));
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Modifie un événement existant.
     *
     * @param int $id L'identifiant de l'événement.
     * @param string $titre Le nouveau titre de l'événement.
     * @param string $ancien_titre L'ancien titre de l'événement.
     * @param string $couleur La nouvelle couleur de l'événement.
     * @param string $heure_debut La nouvelle heure de début de l'événement au format hh:mm.
     * @param string $heure_fin La nouvelle heure de fin de l'événement au format hh:mm.
     * @param string $description La nouvelle description de l'événement.
     * @param string $pictogramme Le nouveau pictogramme associé à l'événement.
     * @param int $jour_semaine Le nouveau jour de la semaine pour l'événement.
     * @param int $interval_semaine Le nouvel intervalle de semaines pour la répétition de l'événement.
     * @param string $date_fin La nouvelle date de fin de la répétition de l'événement.
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function editEvent($id, $titre, $ancien_titre, $couleur, $heure_debut, $heure_fin, $description, $pictogramme, $jour_semaine, $interval_semaine, $date_fin, $audio, $odeur) {
        global $lien;
        bddConnect();
        $nbLigne = 0;
        $id_horloge = $_SESSION['id_horloge'];
        try {
            $debut = intval(explode(":", $heure_debut)[0])*60 + explode(":", $heure_debut)[1];
            $fin = intval(explode(":", $heure_fin)[0])*60 + explode(":", $heure_fin)[1];
            $duree = $fin - $debut;

            //permet de gérer les ' pour contrer le bug de la requette sql
            //je rajoute un \avant le ' pour qu'il soit compté comme char sans sa fonction de fin de chaine de char
            $titre_verif = "";
            $description_verif = "";

            for ($i = 0; $i < strlen($description); $i++) {
                if ($description[$i] === "'") {
                    $description_verif .= "\\";
                }
                $description_verif .= $description[$i];
            }
            for ($i = 0; $i < strlen($titre); $i++) {
                if ($titre[$i] === "'") {
                    $titre_verif .= "\\";
                }
                $titre_verif .= $titre[$i];
            }
            
            /* récup des données pour verif */
            $req = "SELECT id, nom, debut, duree, couleur, commentaire, ecart, jour_semaine, fin_repetition, audio, odeur, pictogramme FROM evenements WHERE id_utilisateur = $id_horloge ORDER BY debut ASC";
            $res = $lien->query($req);

            $donnees = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }

            for ($i = 0; $i < $nbLigne; $i++) {
                if ($donnees[$i]['nom'] != $ancien_titre) {
                    if ($donnees[$i]['nom'] == $titre_verif) {
                        echo "Un événement avec le même titre a déjà été enregistré";
                        return;
                    }
                    if (($debut >= $donnees[$i]['debut'] && $debut < $donnees[$i]['debut'] + $donnees[$i]['duree']) || ($debut < $donnees[$i]['debut'] + $donnees[$i]['duree'] && $fin > $donnees[$i]['debut'])) {
                        echo "L'événement que vous essayez d'ajouter se chevauche avec un autre";
                        return;
                    }
                }
            }

            /* update si bon */
            $req = "UPDATE evenements SET nom = '$titre_verif', debut = $debut, duree = $duree, couleur = '$couleur', commentaire = '$description_verif', ecart = '$interval_semaine', jour_semaine = '$jour_semaine', fin_repetition = '$date_fin', audio = '$audio', odeur = '$odeur', pictogramme = '$pictogramme' WHERE id_utilisateur = $id_horloge AND id = '$id'";
            $res = $lien->query($req);
            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Supprime un événement par son titre.
     *
     * @param string $titre Le titre de l'événement à supprimer.
     * @return int 1 en cas de succès, 0 en cas d'échec.
     */
    function deleteEvent($titre) {
        global $lien;
        bddConnect();
        $id_horloge = $_SESSION['id_horloge'];
        try {
            $req = "DELETE FROM evenements WHERE nom = '$titre' AND id_utilisateur = $id_horloge ";
            $res = $lien->query($req);
            return 1;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /**
     * Récupère tous les événements pour l'utilisateur connecté.
     *
     * @return string JSON contenant tous les événements de l'utilisateur.
     */
    function getAllEvent() {
        global $lien;
        bddConnect();
        $id_horloge = $_SESSION['id_horloge'];
        try {
            $req = "SELECT * FROM `evenements` WHERE id_utilisateur = $id_horloge ORDER BY debut ASC";
            $res = $lien->query($req);

            $donnees = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }

            return json_encode($donnees);
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    /*****************************************************************************/
    /*                       FONCTION LIER AUX PCITOGRAMMES                      */
    /*****************************************************************************/

    /**
     * Affiche les pictogrammes associés aux événements de l'utilisateur connecté.
     *
     * Cette fonction récupère les événements et leurs pictogrammes associés depuis la base de données,
     * les affiche en HTML et appelle une fonction JavaScript pour chaque pictogramme.
     *
     * @global object $lien La connexion à la base de données.
     * @global int $_SESSION['id_horloge'] L'identifiant de l'utilisateur connecté.
     * @return void
     */
    function afficherPictogrammes() {
        global $lien;
        $id_horloge = $_SESSION['id_horloge'];
        $nbLigne = 0;
        bddConnect();
        try {
            $req = "SELECT evenements.id AS id_event, debut, duree, pictogrammes.id AS id_img, pictogrammes.image, pictogrammes.nom FROM `evenements` JOIN pictogrammes ON pictogrammes.id = evenements.pictogramme JOIN configurations ON configurations.id_utilisateur = evenements.id_utilisateur WHERE configurations.id_utilisateur = $id_horloge ORDER BY debut ASC;";
            $res = $lien->query($req);
            $donnees = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }

            for ($i = 0; $i < $nbLigne; $i++) {
                if ($i != 0) {
                    echo "\t\t\t\t\t\t\t";
                }
                echo "<div class='pictogramme' id='div-picto" . $i . "' style='transform-origin: left center;'>\n";
                echo "\t\t\t\t\t\t\t<div class='pre-svg'>\n";
                echo "\t\t\t\t\t\t\t\t" . $donnees[$i]['image'] . "\n";
                echo "\t\t\t\t\t\t\t</div>\n";
                echo "\t\t\t\t\t\t\t</div>\n";
            }

            echo "\t\t\t\t\t\t\t<script>\n";
            for ($i = 0; $i < $nbLigne; $i++) {
                echo "\t\t\t\t\t\t\t\tpictogramme(" . $i . "," . $donnees[$i]['debut'] . ", " . ($donnees[$i]['debut'] + $donnees[$i]['duree']) . ");\n";
            }
            echo "\t\t\t\t\t\t\t</script>\n";

            return;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }
