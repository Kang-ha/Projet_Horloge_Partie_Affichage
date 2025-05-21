# Mise à jour et gestion du service Diffuseur

Ce document décrit les commandes à utiliser pour arrêter, mettre à jour, recompiler et gérer le service `diffuseur.service`, ainsi que pour suivre ses logs.

## 1. Arrêter le service

```bash
sudo systemctl stop diffuseur.service
```

> Arrête proprement le service avant toute modification du binaire.

## 2. Recompiler le binaire

1. Placez-vous dans le répertoire contenant le code source :

   ```bash
   cd ~/Horloge
   ```
2. Lancez la compilation :

   ```bash
   g++ -o diffuseur_control diffuseur_control.cpp -lmysqlcppconn -lpaho-mqttpp3 -lpaho-mqtt3as
   ```

> Assurez-vous d’avoir installé les paquets de développement nécessaires :
>
> ```bash
> sudo apt-get install libmysqlcppconn-dev libpaho-mqttpp3-dev
> ```

## 3. Tester manuellement le binaire

```bash
./diffuseur_control --help
```

> Vérifiez le bon fonctionnement ou les options disponibles, puis terminez (Ctrl+C).

## 4. Redémarrer le service

```bash
sudo systemctl start diffuseur.service
```

> Démarre le nouveau binaire compilé.

## 5. Vérifier le statut du service

* Afficher l’état général :

  ```bash
  sudo systemctl status diffuseur.service
  ```
* Afficher les logs journald :

  ```bash
  journalctl -u diffuseur.service --since "5 minutes ago"
  ```

## 6. Suivre les logs d’exécution en temps réel

```bash
tail -f /home/horloge/Horloge/logs/diffuseur.log
```

> Vous verrez en direct les messages que le service écrit dans son fichier de log.

## 7. Autres commandes utiles

* **Relancer** le service (stop + start) :

  ```bash
  sudo systemctl restart diffuseur.service
  ```
* **Désactiver** le démarrage automatique au boot :

  ```bash
  sudo systemctl disable diffuseur.service
  ```
* **Activer** le démarrage automatique :

  ```bash
  sudo systemctl enable diffuseur.service
  ```
* **Afficher les dernières lignes** des logs via journald :

  ```bash
  journalctl -u diffuseur.service -n 50
  ```

---

© Projet Horloge • Partie Affichage – Documentation de gestion du service
