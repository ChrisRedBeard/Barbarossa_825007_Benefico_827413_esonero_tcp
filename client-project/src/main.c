/*
 * main.c
 *
 * TCP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP client
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

#define NO_ERROR 0 //già presente in winsock
#define INDIRIZZO_IP_SERVER "10.121.54.85"


void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *error_message) {
	printf("%s",error_message);
}

int main(int argc, char *argv[]) {


	// TODO: Implement client logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;

	/*la funzione MAKEWORD(2,2) specifica la versione del socket
	 *&wsa_data viene riempita con informazioni
	 *sullï¿½implementazione di Winsock installata nel sistema.*/
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);

	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket; //socket del client

	// TODO: Create socket
	 my_socket = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);

	 //in caso di errore la funzione socket restituisce il messaggio riportato
	 if (my_socket < 0) {
	  errorhandler("socket creation failed.\n");
	  closesocket(my_socket);
	  clearwinsock();
	  return -1;
	 }

	// TODO: Configure server address
	 struct sockaddr_in server_addr;
     memset(&server_addr,0,sizeof(server_addr));
	 server_addr.sin_family = AF_INET;
	 server_addr.sin_addr.s_addr = inet_addr(INDIRIZZO_IP_SERVER);
	 server_addr.sin_port = htons(SERVER_PORT);


	// TODO: Connect to server
	 //stabilisce una connessione ad una socket specificata
	 if (connect(my_socket,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	 {
	  errorhandler( "Failed to connect.\n" );
	  closesocket(my_socket);
	  clearwinsock();
	 return 0;
	 }
	 char* input_string = "Sono fortissimo!"; // Stringa da inviare
	// int string_len = strlen(input_string); // Determina la lunghezza


	 weather_request_t req;
	 sscanf(argv[2], "%c %[^\n]", &req.type, req.city);
	 printf("tipo richiesta: %c\n",req.type);
	 printf("nome citta': %s\n",req.city);


	 // TODO: Implement communication logic
	 //invia dati ad una socket connessa
	 if (send(my_socket, (char*)&req, sizeof(weather_request_t), 0) != sizeof(weather_request_t))
	 {
		 errorhandler("send() sent a different number of bytes than expected");
		 closesocket(my_socket);
		 clearwinsock();
		 return -1;
	 }

	 // RICEVERE DATI DAL SERVER
	 int bytes_rcvd;
	 int total_bytes_rcvd = 0;
	 char buf[BUFFER_SIZE]; // buffer for data from the server
	 memset(buf, 0, BUFFER_SIZE); // ensures extra bytes contain 0
	 printf("Received: "); // Setup to print the echoed string
	 //riceve dati da una socket connessa
	 while (total_bytes_rcvd < sizeof(weather_request_t)) {
	 if ((bytes_rcvd = recv(my_socket, buf, BUFFER_SIZE - 1, 0)) <= 0)
	 {
		 errorhandler("recv() failed or connection closed prematurely");
		 closesocket(my_socket);
		 clearwinsock();
		 return -1;
	 }
	 total_bytes_rcvd += bytes_rcvd; // Keep tally of total bytes
	 }
	 buf[bytes_rcvd] = '\0'; // ridondante se si fa memset prima
	 printf("%s", buf); // Print the echo buffer

	// TODO: Close socket
	 closesocket(my_socket);
	 clearwinsock();
	printf("\nClient terminated. 1231\n");


	return 0;
} // main end
