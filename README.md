# PenduProjet
Projet de pendu en C réalisé dans le cadre d'une SAE du 7 fevrier au 10 fevrier.

# Membres du groupe
Samuel CARON  <br>
Marina FLAMENT  <br>
Laurane MOURONVAL <br>

# Version 0
Pour lancer cette version : 

## D'abord lancer le serveur PT3N_serveur_V0.c avec les commandes : 
     gcc PT3N_serveur_V0.c -o serveur
     ./serveur

## Ensuite lancer le client PT3N_client_V0.c avec les commandes : 
     gcc PT3N_client_V0.c -o client 
     ./client 127.0.0.1 5050
       
<br>
<br>

# Version 1
Pour lancer cette version avec 1 serveur et 2 client : 

## D'abord lancer le serveur PT3N_serveur_V0.c avec les commandes : 
     gcc PT3N_serveur_V0.c -o serveur
     ./serveur

## Ensuite lancer le client PT3N_client_V0.c avec les commandes : 
     gcc PT3N_client_V0.c -o client 
     ./client 127.0.0.1 5050
     
## Faire de même pour le second client :
     ./client 127.0.0.1 5050
       
# Fonctionnement du jeu
Le jeu se joue en tour par tour, le joueur 1 envoie une lettre, puis le joueur 2 envoie une lettre à son tour. 
Le jeu s'arrete lorsqu'un des deux joueurs à gagné ou perdu.
