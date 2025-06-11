<?php
    // Inclut le fichier de configuration
    require_once("config.php");
    // Démarre la session
    session_start();
    // Variable pour stocker la connexion à la base de données
    $lien = null;

    // Fonction pour se connecter à la base de données
    function bddConnect() {
        global $lien;
        try {
            // Crée une nouvelle connexion PDO
            $lien = new PDO("mysql:dbname=".BDD.";charset=utf8",USER, PWD,
            array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION));
        } catch (Exception $e) {
            // Affiche un message d'erreur si la connexion échoue
            echo ("Base de données en maintenance");
            $lien = null;
            die();
        }
    }

    /*****************************************************************************/
    /*                               REQUETTE AJAX                               */
    /*****************************************************************************/

    // Vérifie si une requête AJAX est envoyée
    if (isset($_POST['action'])) {
        $action = $_POST['action'];
        // $action contient le nom de la fonction à exécuter
        
        // Si l'action est 'getPeriode'
        if ($action == 'getPeriode') {
            if (isset($_POST['temps_minute'])) {
                $temps_minute = $_POST['temps_minute'];
                // Appelle la fonction getPeriode avec le temps en minutes
                $result = getPeriode($temps_minute);
                // Renvoie le résultat
                echo $result;
            }
        }
        // Si l'action est 'afficherPictogrammes'
        if ($action == 'afficherPictogrammes') {
            // Appelle la fonction afficherPictogrammes
            $result = afficherPictogrammes();
            // Renvoie le résultat
            echo $result;
        }
    }
    /*****************************************************************************/
    /*                                  FONCTION                                 */
    /*****************************************************************************/
    
    // Fonction pour afficher l'horloge avec les périodes de la journée
    function afficherHorloge(){
        global $lien;
        try {
            // Requête SQL pour récupérer les périodes et configurations
            $req="SELECT * FROM periodes JOIN configurations ON configurations.id = periodes.id_configuration";
            $res = $lien->query($req);
            
            $donnees = array();
        
            // Stocke les résultats dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }

            // Récupère les données pour chaque période
            $labelMatin = $donnees[1]['nom'];
            $d_matin = $donnees[1]['duree']/60;
            $couleurmatin = $donnees[1]['couleur'];
            
            $labelMidi = $donnees[2]['nom'];
            $d_midi = $donnees[2]['duree']/60;
            $couleurmidi = $donnees[2]['couleur'];
            
            $labelApresMidi = $donnees[3]['nom'];
            $d_apres_midi = $donnees[3]['duree']/60;
            $couleurapresmidi = $donnees[3]['couleur'];
            
            $labelSoir = $donnees[4]['nom'];
            $d_soir = $donnees[4]['duree']/60;
            $couleursoir = $donnees[4]['couleur'];
            
            $labelNuit = "nuit";
            $d_nuit_debut_journee = $donnees[0]['duree']/60;
            
            $d_nuit_fin_journee = $donnees[5]['duree']/60;
            $couleurnuit = $donnees[5]['couleur'];
            
            // Génère le script JavaScript pour initialiser l'horloge
            echo "<script>";
            echo        "horloge('".$labelNuit."',".$d_nuit_debut_journee.",".$d_nuit_fin_journee.",'".$couleurnuit."','".$labelMatin."',".$d_matin.",'".$couleurmatin."','".$labelMidi."',".$d_midi.",'".$couleurmidi."','".$labelApresMidi."',".$d_apres_midi.",'".$couleurapresmidi."','".$labelSoir."',".$d_soir.",'".$couleursoir."');";
            echo "</script>\n";
            
            return 1;
        } catch (Exception $e) {
            $lien = null;
        }
    }

    // Fonction pour obtenir la période actuelle en fonction du temps en minutes
    function getPeriode($temps_minute){
        global $lien;
        bddConnect();
        try {
            // Vérifie que la connexion est établie
            if (!$lien) {
                throw new Exception("La connexion à la base de données n'est pas établie");
            }
            // Requête SQL pour récupérer les périodes
            $req="SELECT * FROM periodes JOIN configurations on configurations.id = periodes.id_configuration ORDER BY debut ASC";
            $res = $lien->query($req);
    
            $donnees = array();
        
            // Stocke les résultats dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }

            $periode_acctuel = null;

            // Cherche la période correspondant au temps donné
            for ($i = 0; $i < count($donnees); $i++) {
                if ($temps_minute >= $donnees[$i]["debut"] && $temps_minute < ($donnees[$i]["debut"] + $donnees[$i]["duree"])) {
                    $periode_acctuel = $donnees[$i]['nom'];
                    break;
                }
            }

            // Si aucune période n'est trouvée, utilise la première période du jour suivant
            if ($periode_acctuel === null) {
                $periode_acctuel = $donnees[0]['nom'];
            }

            // Remplace les périodes spéciales de nuit par "nuit"
            if ($periode_acctuel == "nuit_debut_journee" || $periode_acctuel == "nuit_fin_journee") {
                $periode_acctuel = "nuit";
            }
            return $periode_acctuel;

        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }

    // Fonction pour afficher les pictogrammes
    function afficherPictogrammes() {
        global $lien;
        $nbLigne = 0;
        bddConnect();
        try {
            // Requête SQL pour récupérer les événements et pictogrammes
            $req = "SELECT evenements.id AS id_event, debut, duree, pictogrammes.id AS id_img, pictogrammes.image, pictogrammes.nom FROM `evenements` JOIN pictogrammes ON pictogrammes.id = evenements.pictogramme JOIN configurations ON configurations.id_utilisateur = evenements.id_utilisateur  ORDER BY debut ASC;";
            $res = $lien->query($req);
            $donnees = array();

            // Stocke les résultats dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }

            // Génère le HTML pour chaque pictogramme
            for ($i=0; $i < $nbLigne; $i++) { 
                if($i!=0){echo "\t\t\t\t\t\t\t";}
                echo "<div class='pictogramme' id='div-picto".$i."' style='transform-origin: left center;'>\n";
                echo "\t\t\t\t\t\t\t<div class='pre-svg'>\n";
                echo "\t\t\t\t\t\t\t\t".$donnees[$i]['image']."\n";
                echo "\t\t\t\t\t\t\t</div>\n";
                echo "\t\t\t\t\t\t\t</div>\n";
            }

            // Génère le JavaScript pour positionner les pictogrammes
            echo "\t\t\t\t\t\t\t<script>\n";
            for ($i=0; $i < $nbLigne; $i++) { 
                echo "\t\t\t\t\t\t\t\tpictogramme(".$i.",".$donnees[$i]['debut'].", ".($donnees[$i]['debut']+$donnees[$i]['duree']).");\n";
            }
            echo "\t\t\t\t\t\t\t</script>\n";
            
            return;
        } catch (Exception $e) {
            echo "Erreur : " . $e->getMessage();
            $lien = null;
        }
    }