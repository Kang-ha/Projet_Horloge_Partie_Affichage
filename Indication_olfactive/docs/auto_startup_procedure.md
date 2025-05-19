## Démarrage automatique d’un script sur Raspberry Pi

Ce document décrit deux méthodes pour exécuter un script automatiquement au démarrage de la Raspberry Pi : via **crontab** et via **systemd**.

---

### 1. Méthode Crontab (@reboot)

1. Éditez le crontab de l’utilisateur `pi` :

   ```bash
   crontab -e
   ```
2. Ajoutez la ligne suivante en fin de fichier pour lancer votre script au boot :

   ```cron
   @reboot /home/horloge/Horloge/lancer_diffuseur.sh
   ```

   * `@reboot` : directive pour exécuter la commande au démarrage
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

1. Créez un fichier de service dans `/etc/systemd/system/diffuseur.service` :

   ```ini
   [Unit]
   Description=Diffuseur Olfactif Intelligent
   After=network.target

   [Service]
   Type=simple
   ExecStart=/home/horloge/Horloge/diffuseur_control
   WorkingDirectory=/home/horloge/Horloge
   User=horloge
   Restart=on-failure
   RestartSec=5
   StandardOutput=append:/home/horloge/Horloge/logs/diffuseur.log
   StandardError=append:/home/horloge/Horloge/logs/diffuseur_error.log
   StandardOutput=append:/home/horloge/Horloge/logs/diffuseur.log
   StandardError=append:/home/horloge/Horloge/logs/diffuseur.log

   [Install]
   WantedBy=multi-user.target
   ```
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
   journalctl -u mon_script.service --since "5 minutes ago"
   ```
6. Redémarrez pour valider :

   ```bash
   sudo reboot
   ```

---

© Projet Horloge • Partie Affichage - Procédure Automatisation de Script au Boot
