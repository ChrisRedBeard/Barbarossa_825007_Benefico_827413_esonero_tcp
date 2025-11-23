
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
#include <windows.h>
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
#define INDIRIZZO_IP_SERVER "127.0.0.1"


void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *error_message) {
	printf("%s",error_message);
}


char* valueToString(char tipo,float value){
	static char temp[40];
	switch (tipo) {
			case 't':
			     snprintf(temp,sizeof(temp),"Temperatura = %.1f \u00B0C",value);
				break;
			case 'p':
				snprintf(temp,sizeof(temp),"Pressione = %.1f hPA",value);
				break;
			case 'h':
				snprintf(temp,sizeof(temp),"Umidita' = %.1f %%",value);
				break;
			case 'w':
				snprintf(temp,sizeof(temp),"Vento = %.1f Km/h",value);
				break;
	}
	return temp;
}



int main(int argc, char *argv[]) {


	// ---------------- PARSING ARGOMENTI ----------------
	char server_ip[64] = INDIRIZZO_IP_SERVER;   // default
	int port = SERVER_PROTOPORT;                     // default
	weather_request_t req;

	if (argc < 3) {
	    fprintf(stderr, "Uso: %s [-s server] [-p port] -r \"type city\"\n", argv[0]);
	    return -1;
	}

	int i = 1;
	int found_r = 0;

	while (i < argc) {

	    // -s server
	    if (strcmp(argv[i], "-s") == 0) {
	        if (i + 1 >= argc) {
	            fprintf(stderr, "Errore: manca il valore dopo -s\n");
	            return -1;
	        }
	        snprintf(server_ip, sizeof(server_ip), "%s", argv[i+1]);
	        i += 2;
	        continue;
	    }

	    // -p port
	    if (strcmp(argv[i], "-p") == 0) {
	        if (i + 1 >= argc) {
	            fprintf(stderr, "Errore: manca la porta dopo -p\n");
	            return -1;
	        }
	        port = atoi(argv[i+1]);
	        i += 2;
	        continue;
	    }

	    // -r "type city"
	    if (strcmp(argv[i], "-r") == 0) {
	        if (i + 1 >= argc) {
	            fprintf(stderr, "Errore: manca la stringa dopo -r\n");
	            return -1;
	        }

	        if (sscanf(argv[i+1], "%c %39[^\n]", &req.type, req.city) != 2) {
	            fprintf(stderr, "Errore: formato richiesto deve essere \"type city\"\n");
	            return -1;
	        }

	        found_r = 1;
	        i += 2;
	        continue;
	    }

	    fprintf(stderr, "Parametro sconosciuto: %s\n", argv[i]);
	    return -1;
	}

	if (!found_r) {
	    fprintf(stderr, "Errore: il parametro -r e' obbligatorio\n");
	    return -1;
	}




	// TODO: Implement client logic

#if defined WIN32
	SetConsoleOutputCP(CP_UTF8);

	// Initialize Winsock
	WSADATA wsa_data;

	/*la funzione MAKEWORD(2,2) specifica la versione del socket
	 *&wsa_data viene riempita con informazioni
	 *sull' implementazione di Winsock installata nel sistema.*/
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
	 server_addr.sin_addr.s_addr = inet_addr(server_ip);
	 server_addr.sin_port = htons(port);


	// TODO: Connect to server
	 //stabilisce una connessione ad una socket specificata
	 if (connect(my_socket,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	 {
	  errorhandler( "Failed to connect.\n" );
	  closesocket(my_socket);
	  clearwinsock();
	 return 0;
	 }


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
	 unsigned long int total_bytes_rcvd = 0;
	 char buf[BUFFER_SIZE]; // buffer for data from the server
	 weather_response_t response;
	 memset(buf, 0, BUFFER_SIZE); // ensures extra bytes contain 0

	 //riceve dati da una socket connessa
	 while (total_bytes_rcvd < sizeof(weather_response_t)) {

	     bytes_rcvd = recv(
	         my_socket,
	         ((char*)&response) + total_bytes_rcvd,
	         sizeof(weather_response_t) - total_bytes_rcvd,
	         0
	     );

	     if (bytes_rcvd <= 0) {
	         errorhandler("recv() failed or connection closed prematurely");
	         closesocket(my_socket);
	         clearwinsock();
	         return -1;
	     }

	     total_bytes_rcvd += bytes_rcvd;
	 }
	 switch(response.status){
	  case 1: printf("Ricevuto risultato dal server ip %s Citta' non disponibile",INDIRIZZO_IP_SERVER);
	  	  	  break;
	  case 2: printf("Ricevuto risultato dal server ip %s Richiesta non valida",INDIRIZZO_IP_SERVER);
	  	  	  break;
	  default:	printf("Ricevuto risultato dal server ip %s %s: %s",server_ip,req.city,valueToString(response.type,response.value));
	  	  	  break;
	 }


	// TODO: Close socket
	 closesocket(my_socket);
	 clearwinsock();
	printf("\nClient terminato.\n");


	return 0;
} // main end

