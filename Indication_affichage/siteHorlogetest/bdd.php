<?php
    require_once("config.php");
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

    // Vérification de la présence d'une requette ajax
    if (isset($_POST['action'])) {
        $action = $_POST['action'];
        // $action contient le nom de la fonction à éxecuter

        if ($action == 'afficherPictogrammes') {
            // Appeler la fonction getPeriode avec les paramètres appropriés
            $result = afficherPictogrammes();

            // Vous pouvez renvoyer une réponse au JavaScript si nécessaire
            echo $result;
        }

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
                
                // Appeler la fonction getPeriode avec les paramètres appropriés
                $result = updateHoraireHorloge($matin1, $couleur_matin, $midi1, $couleur_midi, $apres_midi1, $couleur_apres_midi, $soir1, $couleur_soir, $nuit1, $couleur_nuit);
                
                // Vous pouvez renvoyer une réponse au JavaScript si nécessaire
                echo $result;
            }
        }
    }

    /*****************************************************************************/
    /*                         FONCTION LIER AUX PERIODES                        */
    /*****************************************************************************/
    function afficherHorloge(){
        global $lien;
        try {
            $req="SELECT * FROM periodes JOIN configurations ON configurations.id = periodes.id_configuration ORDER BY debut ASC;";
            $res = $lien->query($req);
            
            $donnees = array();
        
            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
            }
            //print_r($donnees);

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
            

            echo "<script>";
            echo        "horloge('".$labelNuit."',".$d_nuit_debut_journee.",".$d_nuit_fin_journee.",'".$couleurnuit."','".$labelMatin."',".$d_matin.",'".$couleurmatin."','".$labelMidi."',".$d_midi.",'".$couleurmidi."','".$labelApresMidi."',".$d_apres_midi.",'".$couleurapresmidi."','".$labelSoir."',".$d_soir.",'".$couleursoir."');";
            echo "</script>\n";
            
            return 1;
        } catch (Exception $e) {
            $lien = null;
        }
    }
    
    function updateHoraireHorloge($matin1, $couleur_matin, $midi1, $couleur_midi, $apres_midi1, $couleur_apres_midi, $soir1, $couleur_soir, $nuit1, $couleur_nuit){
        global $lien;
        bddConnect();
        try {
            $matin = intval(explode(":", $matin1)[0]) * 60 + intval(explode(":", $matin1)[1]);
            $d_matin = (intval(explode(":", $midi1)[0]) * 60 + intval(explode(":", $midi1)[1]))-(intval(explode(":", $matin1)[0]) * 60 + intval(explode(":", $matin1)[1]));

            $midi = intval(explode(":", $midi1)[0]) * 60 + intval(explode(":", $midi1)[1]);
            $d_midi = (intval(explode(":", $apres_midi1)[0]) * 60 + intval(explode(":", $apres_midi1)[1]))-(intval(explode(":", $midi1)[0]) * 60 + intval(explode(":", $midi1)[1]));

            $apres_midi = intval(explode(":", $apres_midi1)[0]) * 60 + intval(explode(":", $apres_midi1)[1]);
            $d_apres_midi = (intval(explode(":", $soir1)[0]) * 60 + intval(explode(":", $soir1)[1]))-(intval(explode(":", $apres_midi1)[0]) * 60 + intval(explode(":", $apres_midi1)[1]));

            $soir = intval(explode(":", $soir1)[0]) * 60 + intval(explode(":", $soir1)[1]);
            $d_soir = (intval(explode(":", $nuit1)[0]) * 60 + intval(explode(":", $nuit1)[1]))-(intval(explode(":", $soir1)[0]) * 60 + intval(explode(":", $soir1)[1]));

            $nuit_debut_journee = 0;
            $d_nuit_debut_journee = $matin;

            $nuit_fin_journee = intval(explode(":", $nuit1)[0]) * 60 + intval(explode(":", $nuit1)[1]);
            $d_nuit_fin_journee = 24*60-$nuit_fin_journee;

            $req="UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration SET periodes.nom = 'nuit_debut_journee', periodes.debut = '$nuit_debut_journee', periodes.duree = '$d_nuit_debut_journee', periodes.couleur = '$couleur_nuit' AND periodes.nom = 'nuit_debut_journee';
                  UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration SET periodes.nom = 'matin', periodes.debut = '$matin', periodes.duree = '$d_matin', periodes.couleur = '$couleur_matin' AND periodes.nom = 'matin';
                  UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration SET periodes.nom = 'midi', periodes.debut = '$midi', periodes.duree = '$d_midi', periodes.couleur = '$couleur_midi' AND periodes.nom = 'midi';
                  UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration SET periodes.nom = 'apres_midi', periodes.debut = '$apres_midi', periodes.duree = '$d_apres_midi', periodes.couleur = '$couleur_apres_midi' AND periodes.nom = 'apres_midi';
                  UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration SET periodes.nom = 'soir', periodes.debut = '$soir', periodes.duree = '$d_soir', periodes.couleur = '$couleur_soir' AND periodes.nom = 'soir';
                  UPDATE periodes JOIN configurations ON configurations.id = periodes.id_configuration SET periodes.nom = 'nuit_fin_journee', periodes.debut = '$nuit_fin_journee', periodes.duree = '$d_nuit_fin_journee', periodes.couleur = '$couleur_nuit' AND periodes.nom = 'nuit_fin_journee';
                  ";
            echo $req;
            $res = $lien->query($req);
            
            return 1;
        } catch (Exception $e) {
            $lien = null;
        }
    }

    /*****************************************************************************/
    /*                       FONCTION LIER AUX PCITOGRAMMES                      */
    /*****************************************************************************/
    function afficherPictogrammes() {
        global $lien;
        $nbLigne = 0;
        bddConnect();
        try {
            $req = "SELECT evenements.id AS id_event, debut, duree, pictogrammes.id AS id_img, pictogrammes.image, pictogrammes.nom FROM `evenements` JOIN pictogrammes ON pictogrammes.id = evenements.pictogramme JOIN configurations ON configurations.id_utilisateur = evenements.id_utilisateur ORDER BY debut ASC;";
            // echo $req;
            $res = $lien->query($req);
            $donnees = array();

            // Parcourir les lignes du résultat et stocker chaque ligne dans un tableau
            while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
                $donnees[] = $row;
                $nbLigne++;
            }
            // echo $nbLigne;
            // print_r($donnees);

            for ($i=0; $i < $nbLigne; $i++) { 
                if($i!=0){echo "\t\t\t\t\t\t\t";}
                echo "<div class='pictogramme' id='div-picto".$i."' style='transform-origin: left center;'>\n";
                echo "\t\t\t\t\t\t\t<div class='pre-svg'>\n";
                echo "\t\t\t\t\t\t\t\t".$donnees[$i]['image']."\n";
                echo "\t\t\t\t\t\t\t</div>\n";
                echo "\t\t\t\t\t\t\t</div>\n";
            }

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

    // TODO : gestion des events (ajout, modification, suppression) -> ajax pour mettre des messages d'érreurs