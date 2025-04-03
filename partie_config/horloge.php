<?php
    require_once("bdd.php"); 
    bddConnect();

    if(!isset($_SESSION['connexion'])){
        header("Location: ../index.php");
    }
?>
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Horloge</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
        <link href="horloge.css?<?php $randomNumber = rand(1, 100); echo $randomNumber;?>" rel="stylesheet" type="text/css">  
    </head>
    <body>
        
        <div class="container">
            <div><p>Configuration de l'horloge</p></div>
            <div class="case">
                <!--------------------- Affichage de l'horloge --------------------->            
                <div class="div-horloge">
                    <div><p>Prévisualisation de l'horloge</p></div>
                    <div class="horloge_aiguille">
                        <canvas id="myDoughnutChart" width="400" height="400" style="position: absolute"></canvas>
                        <canvas id="myPieChart" width="400" height="400"></canvas>
                        <script src="horloge.js?<?php $randomNumber = rand(1, 100); echo $randomNumber;?>"></script>
                        
                        <div id="horloge">
                            <div class="aiguille-heures"></div>
                        </div>
                        
                        <div id="afficherPictogrammes">
                            <?php
                                afficherPictogrammes();
                            ?>
                        </div>

                        <div class="number">
                            <span style="--i: 0"><p>0</p></span>
                            <span style="--i: 3"><p>3</p></span>
                            <span style="--i: 6"><p>6</p></span>
                            <span style="--i: 9"><p>9</p></span>
                            <span style="--i: 12"><p>12</p></span>
                            <span style="--i: 15"><p>15</p></span>
                            <span style="--i: 18"><p>18</p></span>
                            <span style="--i: 21"><p>21</p></span>
                        </div>
                    </div>
                    <?php
                        afficherHorloge($_SESSION['id_horloge']);
                    ?>
                </div>

                <!--------------------- Affichage des informations liées à l'horloge --------------------->
                <div class="div-periode">
                    <div><p>Information sur l'horloge</p></div>
                    <div id=div_time>
                        <script>afficherHeures();</script>
                    </div>

                    <div id="div_date">
                        <script>afficherDate();</script>
                    </div>
                    
                    <div id="div_periode">
                        <script>afficherPeriode();</script>
                    </div>

                    <div id="div_event">
                        <script>afficherEvent();</script>
                    </div>
                    <?php
                        afficherHorlogeEvent($_SESSION['id_horloge']);
                    ?>
                </div>

                <!--------------------- Affichage du panel de modification des périodes de la journée --------------------->
                <div class="div-config">
                    <div><p>Gestion des périodes</p></div>
                    <form method='POST' action='' >
                        <label for='choix'>Choisissez les heures de début de période de la journée :</label>
                        <?php
                            afficherTableauPeriode($_SESSION['id_horloge']);
                        ?>
                    </form> 
                    <button onclick='afficherHorloge();' style="width: 355px;">Modifier les périodes</button>
                    <div class="Periodes_err" id="Periodes_err"></div>
                </div>

                <!--------------------- Affichage du panel de modification des événements --------------------->
                <div class="div-event">
                    <div><p>Gestion des événements</p></div>
                    <form method="POST" action="">
                        <div>
                            <label for="titre">Nom et couleur de l'événement :</label>
                            <input name="titre" type="text" id="titre"><input type="color" name="couleur" id="couleur" value="#a6a6a6">
                            <div>
                                <label for="debut">Heure de début :</label>
                                <input name="debut" id="debut" type="time">
                        
                                <label for="fin">Heure de fin :</label>
                                <input name="fin" id="fin" type="time">
                            </div>
                            <label for="description">Description :</label>
                            <textarea name="description" id="description" style="height:100px"></textarea>
                        </div>
                        
                        <div>
                            <label for="pictogrammes">Pictogrammes :</label><br>
                            <div class="pictogrammes">
                                <?php afficherListePictogrammes();?>
                                <div id="affichage_img_picto"></div>
                            </div>
                        </div>

                        <div>
                            <label for="jours">Répéter :</label>
                            <div class="jours-semaine-selecteur">
                                <input type="checkbox" id="Lundi" name="Lundi" class="semaine" />
                                <label for="Lundi">Lun</label>
                                <input type="checkbox" id="Mardi" name="Mardi" class="semaine" />
                                <label for="Mardi">Mar</label>
                                <input type="checkbox" id="Mercredi" name="Mercredi" class="semaine" />
                                <label for="Mercredi">Mer</label>
                                <input type="checkbox" id="jeudi" name="jeudi" class="semaine" />
                                <label for="jeudi">Jeu</label>
                                <input type="checkbox" id="Vendredi" name="Vendredi" class="semaine" />
                                <label for="Vendredi">Ven</label>
                                <input type="checkbox" id="Samedi" name="Samedi" class="semaine" />
                                <label for="Samedi">Sam</label>
                                <input type="checkbox" id="Dimanche" name="Dimanche" class="semaine" />
                                <label for="Dimanche">Dim</label>
                            </div>
                        </div>

                        <div style="margin-bottom: 10px;">
                            <label for="number">Tous les :</label>
                            <input type="number" id="number" name="interval-semaine" min="0" value="0">
                            <label>semaines</label><br>
                            <label id="information"><img src="images/warning.png" style="width: 10px; height: 10px; color: red;"> Inserer "0" pour répéter toute les semaines</label>
                        </div> 

                        <div>
                            <label for="date">Jusqu'au :</label>
                            <input type="date" id="date" name="date-fin">
                        </div>
                        
                        <div>
							<input type="checkbox" id="audio" name="audio" <?php echo (isset($evenement['audio']) && $evenement['audio'] == 1) ? 'checked' : ''; ?> />
							<label for="audio">audio</label>
                        </div>
                        
                        <div>
							<input type="checkbox" id="odeur" name="odeur" <?php echo (isset($evenement['odeur']) && $evenement['odeur'] == 1) ? 'checked' : ''; ?> />
							<label for="odeur">odeur</label>
                        </div>

                    </form>
                    <button onclick='ajouterEvent();'>Ajouter un événement</button>
                    <div class="Event_err" id="Event_err"></div>
                </div>
            </div>
            <div>
                <!-- Bouton de redirection vers index.php envoi -->
                <!--  -->
                <button><a href="../../siteHorloge/diapos/">Gestion des images</a></button>   
            </div>
        </div>
        <!-- test -->
        <dialog></dialog>
    </body>
</html>
