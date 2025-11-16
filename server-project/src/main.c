/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h> // inizializzazione per socket in windows
#else
#include <string.h>
//servono per l'inizialiazzazione del socket su mac/Unix non necessarie per Windows
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0

#define INDIRIZZO_IP_SERVER "127.0.0.1"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *error_message) {
printf("%s",error_message);
}

int main(int argc, char *argv[]) {

	// TODO: Implement server logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	/*la funzione MAKEWORD(2,2) specifica la versione del socket
	 *&wsa_data viene riempita con informazioni
	 *sull’implementazione di Winsock installata nel sistema.*/
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);

	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	// TODO: Create socket
	 my_socket = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);

	 //in caso di errore la funzione socket restituisce il messaggio riportato
	 if (my_socket < 0) {
	  errorhandler("socket creation failed.\n");
	  return -1;
	 }
	 //port e indirizzo IP
	// TODO: Configure server address
	 struct sockaddr_in server_addr;
     memset(&server_addr,0,sizeof(server_addr));
	 server_addr.sin_family = AF_INET;

	 server_addr.sin_port = htons(SERVER_PORT); // converte un numero del formato del computer a quello della rete (Big-Endian)
	 server_addr.sin_addr.s_addr = inet_addr(INDIRIZZO_IP_SERVER);

	// TODO: Bind socket
	 if ( // la funzione bind associa alla socket un indirizzo in modo da poter essere contattata da un client
			 bind(my_socket , (struct sockaddr*) &server_addr,
	 sizeof(server_addr)) <0) {
	 errorhandler("bind() failed.\n");
	  closesocket(my_socket);
	 return -1;
	 }

	// TODO: Set socket to listen

	 if (listen (my_socket, QUEUE_SIZE) < 0) {
	  errorhandler("listen() failed.\n");
	  closesocket(my_socket);
	  clearwinsock();
	  return -1;
	 }

	// TODO: Implement connection acceptance loop
/*
 * Bisogna innanzitutto creare una variabile che possa contenere il descrittore della socket temporanea,
 * quella che verrà utilizzata per comunicare direttamente con il client.
 * Successivamente si avvia un ciclo continuo che verifica costantemente se ci sono nuove richieste di connessione e,
 *  quando arrivano, le accetta tramite la funzione accept().
 *  Una volta stabilita la connessione, si utilizza la socket temporanea per lo scambio di dati con il client,
 *   mentre la socket principale, quella di benvenuto, rimane in ascolto e continua a gestire eventuali nuove connessioni.*/

	struct sockaddr_in cad; //structure for the client address
	int client_socket; //socket descriptor for the client
	int client_len; //the size of the client address
	printf("Waiting for a client to connect...");
	while (1) {
		client_len = sizeof(cad); //set the size of the client address
		if ((client_socket = accept(my_socket, (struct sockaddr *) &cad,
				&client_len)) < 0) {
			errorhandler("accept() failed.\n");
			closesocket(my_socket);
			clearwinsock();
			return -1;
		}
		// clientSocket is connected to a client
		printf("Handling client %s\n", inet_ntoa(cad.sin_addr));
	// andare a cercare come si fa
	//	handleclientconnection(client_socket);
	}

	printf("Server terminated.\n");

	closesocket(my_socket);

	clearwinsock();
	return 0;
} // main end
