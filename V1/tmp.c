



	// // Crée un socket de communication
	// socketEcoute = socket(PF_INET, SOCK_STREAM, 0); 

	// // Teste la valeur renvoyée par l’appel système socket() 
	// if(socketEcoute < 0){
	// 	perror("socket"); // Affiche le message d’erreur 
	// exit(-1); // On sort en indiquant un code erreur
	// }
	// printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale



	// if( setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {  
    //     perror("setsockopt");  
    //     exit(EXIT_FAILURE);  
    // }  
    
	// // Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	// longueurAdresse = sizeof(pointDeRencontreLocal);

	// memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	// pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	// pointDeRencontreLocal.sin_port = htons(5050); // = 5000 ou plus
	
	// // On demande l’attachement local de la socket
	// if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
	// 	perror("bind");
	// 	exit(-2); 
	// }
	
	// printf("Socket attachee avec succès !\n");

	// // On fixe la taille de la file d’attente à 2 (pour les demandes de connexion non encore traitées)
	// if(listen(socketEcoute, 2) < 0){
   	// 	perror("listen");
   	// 	exit(-3);
	// }
	// // printf("Socket placee en ecoute passive ...\n\n\n");
	


		
        
        
        
        //clear the socket set 
		// FD_ZERO(&readfds);  

		// //add master socket to set 
        // FD_SET(socketEcoute, &readfds);  
        // max_sd = socketEcoute; 

		// //add child sockets to set 
        // for (int i = 0 ; i < maxClients ; i++) {  
        //     //socket descriptor 
        //     socketDialogue = clientSocket[i];  
                 
        //     //if valid socket descriptor then add to read list 
        //     if(socketDialogue > 0)  
        //         FD_SET( socketDialogue , &readfds);  
                 
        //     //highest file descriptor number, need it for the select function 
        //     if(socketDialogue > max_sd)  
        //         max_sd = socketDialogue;  
        // }  	

		// printf("Attente d'une demande de connexion (quitter avec Ctrl-C)\n\n");
		// //wait for an activity on one of the sockets , timeout is NULL , 
        // //so wait indefinitely 
        // activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        // if ((activity < 0) && (errno!=EINTR)) {  
        //     printf("select error");  
        // }

		// //If something happened on the master socket , 
        // //then its an incoming connection 
        // if ((FD_ISSET(socketEcoute, &readfds)))  {  
		// 	socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, &longueurAdresse);
        //     if ((socketDialogue < 0)) {  
        //         perror("accept");  
        //         exit(EXIT_FAILURE);  
        //     }  
             
        //     //inform user of socket number - used in send and receive commands 
        //     printf("Nouvelle connexion , socket fd : %d , ip : %s , port : %d\n" , socketDialogue , inet_ntoa(pointDeRencontreLocal.sin_addr) , ntohs
        //           (pointDeRencontreLocal.sin_port));  
           
        //     //On envoie un message à au nouveau client 
        //     ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi));  
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
			
        //     //ajoute la nouvelle socket
        //     for (int i = 0; i < maxClients; i++) {  
        //         //client 1 ou client 2
        //         if( clientSocket[i] == 0 ){  
        //             clientSocket[i] = socketDialogue;  
		// 			printf("efjOZRNJOZnvJoRN ========= %d", clientSocket[i]);
        //             printf("Socket n°%d ajouté à la liste des sockets\n" , i);
        //             break;  
        //         }  
        //     }
		// }







		// for (int i = 0; i < maxClients; i++)  {  
        //     socketDialogue = clientSocket[i];  
                 
        //     if (FD_ISSET( socketDialogue , &readfds)) {  
        //         //Check if it was for closing , and also read the incoming message 
        //         if ((lus = read(socketDialogue , messageRecu, LG_MESSAGE)) == 0) {  
        //             //Somebody disconnected , get his details and print 
        //             getpeername(socketDialogue , (struct sockaddr*)&pointDeRencontreLocal , \
        //                 (socklen_t*)&addrlen);  
        //             printf("Hote deconnecté, ip %s, port %d\n", inet_ntoa(pointDeRencontreLocal.sin_addr), ntohs(pointDeRencontreLocal.sin_port));  
                         
        //             //Close the socket and mark as 0 in list for reuse 
        //             close( socketDialogue );  
        //             clientSocket[i] = 0;  
        //         }  
                     
        //         //Echo back the message that came in 
        //         else {  
        //             //set the string terminating NULL byte on the end 
        //             //of the data read 
        //             messageRecu[lus] = '\0';  
        //             ecrits = write(socketDialogue , messageRecu , strlen(messageRecu));  
		// 			switch(ecrits){
		// 				case -1 : /* une erreur ! */
		// 					perror("write");
		// 					close(socketDialogue);
		// 					exit(-6);
		// 				case 0 :  /* la socket est fermée */
		// 					fprintf(stderr, "La socket a été fermee par le client !\n\n");
		// 					close(socketDialogue);
		// 					return 0;
		// 				default:  /* envoi de n octets */
		// 					printf("Message envoye :   %s\n\n", messageEnvoi);
		// 			}
        //         }  
        //     }  
          
