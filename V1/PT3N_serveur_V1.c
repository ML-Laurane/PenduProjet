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


bool lettreDejaChoisie(char* lettresChoisies, char* lettre){
    // si la lettre a déjà été choisie par le joueur, renvoie true, sinon renvoie false
	for (int i = 0; i < strlen(lettresChoisies); i++){
		if ((lettre[0] == lettresChoisies[i])){
			return false;
		}
	}
	return true;
}


bool jeuGagne(char* motCache){
	// renvoie true si le joueur a deviné toute les lettres, false sinon
	for (int i = 0; i < strlen(motCache); i++){
		if ((motCache[i] == '_')){
			return false;
		}
	}
	return true;
}

void placeLettre(char* lettre, char* motClair, char* motCache){
	// place la lettre proposée par le joueur dans le mot caché
	for (int i = 0; i < strlen(motClair); i++){
		if ((lettre[0] == motClair[i])){
			motCache[i] = lettre[0];
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
	for (int i = 0; i < longueurMot; i++){
		if ((lettre[0] == motClair[i])){
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
	int opt = 1;
	int max_sd, sd, activity, newSocket, addrlen;
	char message = "LALALALALALA";

	fd_set readfds;
	

	// char motClair [LG_MOT+1] = {'B', 'O', 'N', 'J', 'O', 'U', 'R'};
	// char motCache [LG_MOT+1];
	// char lettresChoisies [27];
	// int compteur = 11;
	// char codeErreur [100] = "";
	// bool fini = false;


	for (int i = 0; i < maxClients; i++) {  
		// initialise tous les clientSocket à 0
        clientSocket[i] = 0;  
    }  

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); 

	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale



	if( setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
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

	// On fixe la taille de la file d’attente à 2 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 2) < 0){
   		perror("listen");
   		exit(-3);
	}
	// printf("Socket placee en ecoute passive ...\n\n\n");
	


	// boucle d’attente de connexion : 
	while(1){
		
		//clear the socket set 
		FD_ZERO(&readfds);  

		//add master socket to set 
        FD_SET(socketEcoute, &readfds);  
        max_sd = socketEcoute; 

		//add child sockets to set 
        for (int i = 0 ; i < maxClients ; i++) {  
            //socket descriptor 
            sd = clientSocket[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  	

		//wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR)) {  
            printf("select error");  
        }

		//If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(socketEcoute, &readfds))  {  
            if ((newSocket = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, (socklen_t*)&addrlen))<0) {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
             
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newSocket , inet_ntoa(pointDeRencontreLocal.sin_addr) , ntohs
                  (pointDeRencontreLocal.sin_port));  
           
            //send new connection greeting message 
            if( send(newSocket, message, strlen(message), 0) != strlen(message) )  {  
                perror("send");  
            }  
                 
            puts("Welcome message sent successfully");  
                 
            //add new socket to array of sockets 
            for (int i = 0; i < maxClients; i++) {  
                //if position is empty 
                if( clientSocket[i] == 0 ){  
                    clientSocket[i] = newSocket;  
                    printf("Adding to list of sockets as %d\n" , i);
                    break;  
                }  
            }
			
			//else its some IO operation on some other socket
			for (int i = 0; i < max_clients; i++)  
			{  
            sd = client_socket[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&pointDeRencontreLocal , \
                        (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(pointDeRencontreLocal.sin_addr) , ntohs(pointDeRencontreLocal.sin_port));  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                    send(sd , buffer , strlen(buffer) , 0 );  
                }  
            }  
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
		
		printf("Client connecté\n\n");

		
		// // affiche le mot caché au client : "______"
		// int longueurMot = strlen(motClair);
		// for (int i = 0; i < longueurMot; i++){
		// 	motCache[i] = '_';
		// }

		// sprintf(messageEnvoi, "%s  \nLe mot à trouver :  %s", "start 7", motCache);

		// ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi)); 
		// switch(ecrits){
		// 	case -1 : /* une erreur ! */
		// 		perror("write");
		// 		close(socketDialogue);
		// 		exit(-6);
		// 	case 0 :  /* la socket est fermée */
		// 		fprintf(stderr, "La socket a été fermee par le client !\n\n");
		// 		close(socketDialogue);
		// 		return 0;
		// 	default:  /* envoi de n octets */
		// 		printf("Message envoye :   %s\n\n", messageEnvoi);
		// }


		// // boucle de jeu 
		// while(!(fini)){
		// 	bzero(messageRecu, 256);
		// 	bzero(codeErreur, 100);

		// 	// le serveur recoit la lettre 
		// 	switch(lus = read(socketDialogue, messageRecu, LG_MESSAGE)) {
		// 		case -1 : /* une erreur ! */ 
		// 			perror("read"); 
		// 			close(socketDialogue); 
		// 			exit(-4);
		// 		case 0  : /* la socket est fermée */
		// 			fprintf(stderr, "La socket a ete fermee par le client !\n\n");
		// 			close(socketDialogue);
		// 			return 0;
		// 		default:  /* réception de n octets */
		// 			messageRecu[lus]='\0';
		// 			printf("Message recu :   %s \n\n", messageRecu);
		// 	}

		// 	// le serveur fait ses verifications sur la lettre (moteur de jeu)
		// 	if ((verifLettre(messageRecu))){
		// 		messageRecu[0] = toupper(messageRecu[0]);
		// 		lettresChoisies[strlen(lettresChoisies)] = messageRecu[0];
		// 		lettresChoisies[strlen(lettresChoisies)+1] = '\0';

		// 		if (!(lettreDejaChoisie(lettresChoisies, messageRecu))){
		// 			if (lettreDansMot(messageRecu, motClair)){
		// 				placeLettre(messageRecu, motClair, &motCache);
		// 			} else {
		// 				compteur--;
		// 				sprintf(codeErreur, "Raté, il vous reste %d chances !\n", compteur);
		// 			}
		// 		}
		// 	} else {
		// 		strcpy(codeErreur, "Caractere différent d'une lettre");
		// 	}

		// 	bzero(messageEnvoi, 256);

		// 	// Etat du jeu : 
		// 	if (compteur == 0){
		// 		strcpy(messageEnvoi, "perdu");
		// 		fini = true;
		// 	} else if ((jeuGagne(motCache))){
		// 		strcpy(messageEnvoi, "gagne");
		// 		fini = true;
		// 	} else if (strlen(codeErreur)>10) {
		// 		sprintf(messageEnvoi, "%s\n%s", codeErreur, motCache);
		// 	} else {
		// 		strcpy(messageEnvoi, motCache);
		// 	}

		// 	// le serveur envoie sa réponse pour la lettre + def de l'etat ju jeu
		// 	ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi)); 
		// 	switch(ecrits){
		// 		case -1 : /* une erreur ! */
		// 			perror("write");
		// 			close(socketDialogue);
		// 			exit(-6);
		// 		case 0 :  /* la socket est fermée */
		// 			fprintf(stderr, "La socket a été fermee par le client !\n\n");
		// 			close(socketDialogue);
		// 			return 0;
		// 		default:  /* envoi de n octets */
		// 			printf("Message envoye :   %s\n\n", messageEnvoi);
					
		// 	}
		// }
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
