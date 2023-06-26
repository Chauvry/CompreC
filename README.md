# CompréC

CompréC est un outil en ligne de commande écrit en C qui permet de gérer les fichiers zip. Il offre diverses fonctionnalités pour manipuler et interagir avec les archives zip cryptées. 

## Caractéristiques

- **Attaque Bruteforce Password** : CompréC propose une option pour tenter des attaques bruteforce sur les fichiers zip protégés par mot de passe, avec ou sans dictionnaire. Après une bruteforce réussie, le mot de passe découvert sera affiché.
- **Déchiffrer un fichier Zip** : Vous pouvez déchiffrer un fichier zip en utilisant un mot de passe spécifié.
- **Exploration interactive** : Une fois le fichier déchiffré, CompréC permet une exploration interactive de l'archive.
- **Extraction de fichier** : Vous pouvez demander l'extraction d'un fichier de l'archive zip, qui sera sauvegardé sur le système de fichiers local.
- **Inclusion de fichier** : CompréC vous permet d'inclure un fichier dans l'archive zip actuellement explorée en fournissant son chemin.
- **Aide** : Le programme propose une section "aide" complète qui explique comment l'utiliser efficacement.

## Restrictions

L'utilisation de la commande système est strictement interdite.

## Dépendances

Pour gérer les archives zip, CompréC utilise la bibliothèque libzip. Pour améliorer l'expérience utilisateur, vous devez choisir l'une des bibliothèques suivantes pour aider avec l'interface:

- ncurses


## Comment installer

Avant de pouvoir compiler et utiliser CompréC, vous devez installer les bibliothèques requises. Voici comment vous pouvez le faire sur un système basé sur Debian:

```
sudo apt-get update
sudo apt-get install libzip-dev libncurses5-dev 
```

## Comment compiler

Après avoir cloné le dépôt et installé les dépendances, vous pouvez compiler le projet en utilisant la commande `make` dans le répertoire racine du projet.

```
make
```

## Utilisation

Après avoir compilé le projet, vous pouvez l'exécuter à partir de la ligne de commande. Pour plus d'informations sur l'utilisation de CompréC, veuillez consulter la section d'aide du programme.

```
./comprec -h
```
