/* ##################################################################################

	Groupe : 	Marina Flament
				Samuel Caron
				Laurane Mouronval
	
	Version 1 - Serveur

################################################################################## */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <stdbool.h>
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256
#define LG_MOT 7


bool lettreDejaChoisie(char *lettresChoisies, char* lettre){
    // si la lettre a déjà été choisie par le joueur, renvoie true, sinon renvoie false
	for (int j = 0; j < strlen(lettresChoisies); j++){
		if ((lettre[0] == lettresChoisies[j])){
			return false;
		}
	}
	return true;
}


bool jeuGagne(char* motCache){
	// renvoie true si le joueur a deviné toute les lettres, false sinon
	for (int j = 0; j < strlen(motCache); j++){
		if ((motCache[j] == '_')){
			return false;
		}
	}
	return true;
}

void placeLettre(char* lettre, char* motClair, char *motCache){
	// place la lettre proposée par le joueur dans le mot caché
	for (int j = 0; j < strlen(motClair); j++){
		if ((lettre[0] == motClair[j])){
			motCache[j] = lettre[0];
		}
	}
}

bool verifLettre(char* lettre){
	// vérifie que le premier caractere du texte envoyé par le joueur est une lettre 
	if (((65 <= lettre[0]) && (lettre[0] <= 90)) || ((97 <= lettre[0]) && (lettre[0] <= 122))){
		return true;
	}
	printf("le caractere doit etre une lettre");
	return false;
}

bool lettreDansMot(char* lettre, char* motClair){
	// renvoie true si la lettre est dans le mot, false sinon
	int longueurMot = strlen(motClair);
	for (int j = 0; j < longueurMot; j++){
		if ((lettre[0] == motClair[j])){
			return true;
		}
	}
	return false;
}



int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	char messageEnvoi[LG_MESSAGE];
	int ecrits, lus; /* nb d’octets ecrits et lus */

	int maxClients = 2;
	int clientSocket[2];
	int i = 0;
	int longueur;
	int max_sd;

	fd_set readfds;
	
	char motClair [LG_MOT+1] = {'B', 'O', 'N', 'J', 'O', 'U', 'R'};
	char motCacheJ1 [LG_MOT+1];
	char motCacheJ2 [LG_MOT+1];

	char lettresChoisiesJ1 [27];
	char lettresChoisiesJ2 [27];

	int tabCompteur[2] = {11, 11};
	
	char codeErreur [100] = "";
	bool fini = false;
	

	// max_sd = socketEcoute;
	for (int j = 0; j < maxClients; j++) {  
		// initialise tous les clientSocket à 0
        clientSocket[j] = 0; 
    }

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);

	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(5050); // = 5000 ou plus
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2); 
	}
	printf("Socket attachee avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placee en ecoute passive ...\n\n\n");


	// boucle d’attente de connexion : 
	while(1){
		FD_ZERO(&readfds); 

		//add master socket to set 
        FD_SET(socketEcoute, &readfds);  
        max_sd = socketEcoute; 

		//add child sockets to set 
        for (int j = 0 ; j < maxClients ; j++) {  
            //socket descriptor 
            socketDialogue = clientSocket[j];  
                 
            //if valid socket descriptor then add to read list 
            if(socketDialogue > 0)  
                FD_SET( socketDialogue , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(socketDialogue > max_sd)  
                max_sd = socketDialogue;  
        }  	

		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente d'une demande de connexion (quitter avec Ctrl-C)\n\n");
		
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0) {
   			perror("accept");
			close(socketDialogue);
   			close(socketEcoute);
   			exit(-4);
		}

		

		// ajoute la nouvelle socket
		for (int j = 0; j < maxClients; j++) {  
			//client 1 ou client 2
			if( clientSocket[j] == 0 ){  
				clientSocket[j] = socketDialogue;  
				printf("Socket n°%d ajouté à la liste des sockets\n" , j);
				break;  
			}  
		}
		
		printf("Client connecté\n\n");
		
		// affiche le mot caché au client : "______"
		int longueurMot = strlen(motClair);
		for (int j = 0; j < longueurMot; j++){
			motCacheJ1[j] = '_';
			motCacheJ2[j] = '_';
		}

		// tant qu'il n'y a pas 2 joueurs connectés, on ne rentre pas dans la boucle de jeu
		if (clientSocket[1] != 0){
			
			while(!(fini)){
				bzero(messageEnvoi, LG_MESSAGE);
				bzero(messageRecu, LG_MESSAGE);
				bzero(codeErreur, 100);

				if ((i%2 == 0)){
					// le serveur envoie un message au joueur quand c'est son tour
					sprintf(messageEnvoi, "Joueur %d, à toi de jouer !\n Le mot à trouver : %s", 1, motCacheJ1);

					ecrits = write(clientSocket[i%2] , messageEnvoi , strlen(messageEnvoi));  
					switch(ecrits){
						case -1 : /* une erreur ! */
							perror("write");
							close(clientSocket[i%2]);
							exit(-6);
						case 0 :  /* la socket est fermée */
							fprintf(stderr, "La socket a été fermee par le client !\n\n");
							close(clientSocket[i%2]);
							return 0;
						default:  /* envoi de n octets */
							printf("%s\n\n", messageEnvoi);
					
					}

					// le serveur récupère la réponse du joueur et fait evoluer son etat du jeu 
					bzero(messageEnvoi, 256);
					bzero(messageRecu, 256);

					lus = read(clientSocket[i%2] , messageRecu , LG_MESSAGE);  
					switch(lus){
						case -1 : /* une erreur ! */
							perror("read");
							close(clientSocket[i%2]);
							exit(-6);
						case 0 :  /* la socket est fermée */
							fprintf(stderr, "La socket a été fermee par le client !\n\n");
							close(clientSocket[i%2]);
							return 0;
						default:  /* envoi de n octets */
							printf("%s\n\n", messageRecu);
					}

					// le serveur fait ses verifications sur la lettre (moteur de jeu)
					if ((verifLettre(messageRecu))){
						longueur = strlen(lettresChoisiesJ1);
						messageRecu[0] = toupper(messageRecu[0]);

						lettresChoisiesJ1[longueur] = messageRecu[0];
						lettresChoisiesJ1[longueur+1] = '\0';
						
						if (!(lettreDejaChoisie(lettresChoisiesJ1, messageRecu))){
							if (lettreDansMot(messageRecu, motClair)){
								placeLettre(messageRecu, motClair, &motCacheJ1);
							} else {
								tabCompteur[i%2]--;
								sprintf(codeErreur, "Raté, il vous reste %d chances !\n", tabCompteur[i%2]);
							}
						} else {
							strcpy(codeErreur, "Lettre déjà choisie");
							i--;
						}
					} else {
						strcpy(codeErreur, "Caractère différent d'une lettre");
					}
					
					bzero(messageEnvoi, 256);
					bzero(messageRecu, 256);
					// Etat du jeu : 
					if (tabCompteur[i%2] == 0){
						sprintf(messageEnvoi, "Joueur %d a perdu, le mot était : %s\n", i%2, motClair);
						fini = true;

					} else if ((jeuGagne(motCacheJ1))){
						sprintf(messageEnvoi, "Joueur %d a gagne, le mot était : %s\n", i%2, motClair);
						fini = true;

					} else if (strlen(codeErreur)>20) {
						sprintf(messageEnvoi, "%s\n%s\n", codeErreur, motCacheJ1);
					} else {
						sprintf(messageEnvoi, "%s\n", motCacheJ1);
					}

					// le serveur envoie sa réponse pour la lettre + def de l'etat ju jeu
					ecrits = write(clientSocket[i%2], messageEnvoi, strlen(messageEnvoi)); 
					switch(ecrits){
						case -1 : /* une erreur ! */
							perror("write");
							close(clientSocket[i%2]);
							exit(-6);
						case 0 :  /* la socket est fermée */
							fprintf(stderr, "La socket a été fermee par le client !\n\n");
							close(clientSocket[i%2]);
							return 0;
						default:  /* envoi de n octets */
							printf("%s\n\n", messageEnvoi);	
					}


























				} else {
					// le serveur envoie un message au joueur quand c'est son tour
					sprintf(messageEnvoi, "Joueur %d, à toi de jouer !\n Le mot à trouver : %s", 2, motCacheJ2);

					// sprintf(messageEnvoi, "Joueur %d, à toi de jouer !\n", (i%2)+1);
					ecrits = write(clientSocket[i%2] , messageEnvoi , strlen(messageEnvoi));  
					switch(ecrits){
						case -1 : /* une erreur ! */
							perror("write");
							close(clientSocket[i%2]);
							exit(-6);
						case 0 :  /* la socket est fermée */
							fprintf(stderr, "La socket a été fermee par le client !\n\n");
							close(clientSocket[i%2]);
							return 0;
						default:  /* envoi de n octets */
							printf("%s\n\n", messageEnvoi);
					}

					// le serveur récupère la réponse du joueur et fait evoluer son etat du jeu 
					bzero(messageEnvoi, 256);
					bzero(messageRecu, 256);

					lus = read(clientSocket[i%2] , messageRecu , LG_MESSAGE);  
					switch(lus){
						case -1 : /* une erreur ! */
							perror("read");
							close(clientSocket[i%2]);
							exit(-6);
						case 0 :  /* la socket est fermée */
							fprintf(stderr, "La socket a été fermee par le client !\n\n");
							close(clientSocket[i%2]);
							return 0;
						default:  /* envoi de n octets */
							printf("%s\n\n", messageRecu);
					}
					
					// le serveur fait ses verifications sur la lettre (moteur de jeu)
					if ((verifLettre(messageRecu))){
						longueur = strlen(lettresChoisiesJ2);
						messageRecu[0] = toupper(messageRecu[0]);

						lettresChoisiesJ2[longueur] = messageRecu[0];
						lettresChoisiesJ2[longueur+1] = '\0';
						
						if (!(lettreDejaChoisie(lettresChoisiesJ2, messageRecu))){
							if (lettreDansMot(messageRecu, motClair)){
								placeLettre(messageRecu, motClair, &motCacheJ2);
							} else {
								tabCompteur[i%2]--;
								sprintf(codeErreur, "Raté, il vous reste %d chances !\n", tabCompteur[i%2]);
							}
						} else {
							strcpy(codeErreur, "La lettre a déjà été choisie");
						}
					} else {
						strcpy(codeErreur, "Caractere différent d'une lettre\n");
					}

					bzero(messageEnvoi, 256);
					bzero(messageRecu, 256);
					// Etat du jeu : 
					if (tabCompteur[i%2] == 0){
						sprintf(messageEnvoi, "Joueur %d a perdu, le mot était : %s\n", i%2, motClair);
						fini = true;

					} else if ((jeuGagne(motCacheJ1))){
						sprintf(messageEnvoi, "Joueur %d a gagne, le mot était : %s\n", i%2, motClair);
						fini = true;

					} else if (strlen(codeErreur)>10) {
						sprintf(messageEnvoi, "%s\n%s\n", codeErreur, motCacheJ2);
					} else {
						sprintf(messageEnvoi, "%s\n", motCacheJ2);
					}

					// le serveur envoie sa réponse pour la lettre + def de l'etat ju jeu
					ecrits = write(clientSocket[i%2], messageEnvoi, strlen(messageEnvoi)); 
					switch(ecrits){
						case -1 : /* une erreur ! */
							perror("write");
							close(clientSocket[i%2]);
							exit(-6);
						case 0 :  /* la socket est fermée */
							fprintf(stderr, "La socket a été fermee par le client !\n\n");
							close(clientSocket[i%2]);
							return 0;
						default:  /* envoi de n octets */
							printf("%s\n\n", messageEnvoi);	
					}

				}
				i++;
			}
		
			// le serveur envoie sa réponse pour la lettre + def de l'etat ju jeu
			ecrits = write(clientSocket[i%2+1], messageEnvoi, strlen(messageEnvoi)); 
			switch(ecrits){
				case -1 : /* une erreur ! */
					perror("write");
					close(clientSocket[i%2+1]);
					exit(-6);
				case 0 :  /* la socket est fermée */
					fprintf(stderr, "La socket a été fermee par le client !\n\n");
					close(clientSocket[i%2+1]);
					return 0;
				default:  /* envoi de n octets */
					printf("%s\n\n", messageEnvoi);	
			} 
		}
		// close(socketEcoute);
	}
	close(socketEcoute);
	return 0; 
}