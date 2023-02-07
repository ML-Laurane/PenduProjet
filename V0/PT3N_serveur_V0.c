#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <stdbool.h>
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256

#define LG_MOT 7

// void lireLettre(char* lettre){
    // reçoit la lettre du joueur
	// FILE *fpipe;
	
	// fpipe = popen("lettre'+%X'","r");
	// if (fpipe == NULL){
    // 	perror("popen");
    // 	exit(-1);
  	// }
	// fgets(lettre, LG_MESSAGE, fpipe);
	// pclose(fpipe);
// }

// bool lettreDejaChoisie(char* lettre){
//     // si la lettre a déjà été choisie par le joueur, renvoie true, sinon renvoie false
// }


// void affichage(){
	//  // affichage du jeu
// }

void placeLettre(char* lettre){
	// place la lettre dans le mot caché
	
}

bool verifLettre(char* lettre){
	// si la lettre est présente dans le mot caché renvoie true, sinon renvoie false
	if (strlen(lettre) != 1){
		printf("Une seule lettre souhaitée");
		perror("longueur");
		return false;
	}
	else if (!((65 <= *lettre) && (*lettre <= 90)) || !((97 <= *lettre) && (*lettre <= 122))){
		printf("Une seule lettre souhaitée");
		perror("longueur");
		return false;
	}
	return true;
}

// void convertirMot(char* motClair, char motCache []){
// 	// cree le mot caché : exemple avec "bonjour" en mot clair, mot cache = "_______"
// 	int longueurMot = strlen(motClair);
// 	// char motCache[longueurMot];

// 	for (int i = 0; i < longueurMot; i++){
// 		motCache[i] = '_';
// 	}
// 	printf(" motCache ===========> %s!\n\n", motCache);
// 	// return motCache;
// }

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	char messageEnvoi[LG_MESSAGE];
	int ecrits, lus; /* nb d’octets ecrits et lus */
	// int retour;

	char motClair [LG_MOT+1] = {'b', 'o', 'n', 'j', 'o', 'u', 'r'};
	char motCache [LG_MOT+1];
	bool jeuActif = false; 

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
	


	



	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 
	while(1){
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
		
		printf("Client connecte\n\n");

		// Déterminer le message à envoyer en fonction de la demande du client
		// if(strlen(messageRecu) == 1)
		// 	lireLettre(messageEnvoi);
		// else
		// 	sprintf(messageEnvoi, "Commande non reconnue");


		// Au lancemant du serveur, on envoit start 7, 7 correspond à la longueur du mot à deviner
		if (!(jeuActif)){
			
			int longueurMot = strlen(motClair);
			for (int i = 0; i < longueurMot; i++){
				motCache[i] = '_';
			}

			strcpy(messageEnvoi, motCache);

			ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi)); 
			switch(ecrits){
				case -1 : /* une erreur ! */
					perror("write");
					close(socketDialogue);
					exit(-6);
				case 0 :  /* la socket est fermée */
					fprintf(stderr, "La socket a été fermee par le client !\n\n");
					close(socketDialogue);
					return 0;
				default:  /* envoi de n octets */
					printf("Message envoye :   %s\n\n", messageEnvoi);
					jeuActif = true;
					// On ferme la socket de dialogue et on se replace en attente ...
					close(socketDialogue);
			}
		} else {
			// On réceptionne les données du client (cf. protocole)
			// lus = nbre d'octets
			lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
			switch(lus) {
				case -1 : /* une erreur ! */ 
					perror("read"); 
					close(socketDialogue); 
					exit(-5);
				case 0  : /* la socket est fermée */
					fprintf(stderr, "La socket a ete fermee par le client !\n\n");
					close(socketDialogue);
					return 0;
				default:  /* réception de n octets */
					printf("Message recu :   %s \n\n", messageRecu);

			}
		}
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
