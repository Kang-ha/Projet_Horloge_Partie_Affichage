## Démarrage automatique d’un script sur Raspberry Pi

Ce document décrit deux méthodes pour exécuter un script automatiquement au démarrage de la Raspberry Pi : via **crontab** et via **systemd**.

---

### 1. Méthode Crontab (@reboot)

1. Éditez le crontab de l’utilisateur (par exemple `pi` ou celui sous lequel tourne votre projet) :

   ```bash
   crontab -e
   ```
2. Ajoutez la ligne suivante en fin de fichier pour lancer votre script au boot, en utilisant une référence relative à votre répertoire cloné :

   ```cron
   @reboot /bin/bash -c 'cd "$HOME/Projet_Horloge_Partie_Affichage/Indication_olfactive/src/Raspberry" && ./lancer_diffuseur.sh >> "$HOME/Projet_Horloge_Partie_Affichage/Indication_olfactive/logs/diffuseur.log" 2>&1'
   ```

   * `@reboot` : directive pour exécuter la commande au démarrage
   * `cd "$HOME/..."` : change le répertoire vers l’endroit où le dépôt est cloné, quelle que soit la machine
   * `>> ... 2>&1` : redirige la sortie standard et d’erreur vers un fichier de log
3. Enregistrez et quittez (`Ctrl+O`, `Entrée`, `Ctrl+X` sous nano).
4. Vérifiez la présence de la tâche :

   ```bash
   crontab -l
   ```
5. Redémarrez pour valider :

   ```bash
   sudo reboot
   ```

---

### 2. Méthode systemd

1. Créez ou modifiez le fichier de service dans `/etc/systemd/system/diffuseur.service` :

   ```ini
   [Unit]
   Description=Diffuseur Olfactif Intelligent
   After=network.target

   [Service]
   Type=simple
   # Exécution du binaire de contrôle depuis le dossier cloné
   ExecStart=/bin/bash -c 'cd "$HOME/Projet_Horloge_Partie_Affichage/Indication_olfactive/src/Raspberry" && ./diffuseur_control'
   WorkingDirectory=$HOME/Projet_Horloge_Partie_Affichage/Indication_olfactive/src/Raspberry
   User=horloge
   Restart=on-failure
   RestartSec=5
   StandardOutput=append:$HOME/Projet_Horloge_Partie_Affichage/Indication_olfactive/logs/diffuseur.log
   StandardError=append:$HOME/Projet_Horloge_Partie_Affichage/Indication_olfactive/logs/diffuseur_error.log

   [Install]
   WantedBy=multi-user.target
   ```

   * Les variables `$HOME` et `cd` garantissent que le chemin s’adapte à l’emplacement du dépôt de chaque clone.
2. Rechargez la configuration systemd :

   ```bash
   sudo systemctl daemon-reload
   ```
3. Activez le service au démarrage :

   ```bash
   sudo systemctl enable diffuseur.service
   ```
4. Démarrez le service immédiatement (test) :

   ```bash
   sudo systemctl start diffuseur.service
   ```
5. Vérifiez le statut et les logs :

   ```bash
   sudo systemctl status diffuseur.service
   journalctl -u diffuseur.service --since "5 minutes ago"
   ```
6. Redémarrez pour valider :

   ```bash
   sudo reboot
   ```

---

© Projet Horloge • Partie Affichage – Procédure Automatisation de Script au Boot
