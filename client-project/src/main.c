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

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement client logic

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

	// TODO: Configure server address
	// struct sockaddr_in server_addr;
	// ...

	// TODO: Connect to server
	// connect(...);

	// TODO: Implement communication logic
	// send(...);
	// recv(...);

	// TODO: Close socket
	// closesocket(my_socket);

	printf("Client terminated. 1231\n");

	clearwinsock();
	return 0;
} // main end
