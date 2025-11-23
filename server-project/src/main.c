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
typedef int socklen_t;
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
#include <ctype.h> // necessario per tolower()

#include "protocol.h"

#define NO_ERROR 0 //gia' definita in WINSOCK
#define INDIRIZZO_IP_SERVER "127.0.0.1"

#ifndef WIN32
char* strlwr(char *s) {
    for (char *p = s; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    return s;
}
#endif


void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

float get_temperature(void) { // Range: -10.0 to 40.0 gradi C
    return (-10.0) + (float) rand() / RAND_MAX * ((40.0) - (-10.0));
}

float get_humidity(void) { // Range: 20.0 to 100.0 %
    return (20.0) + (float) rand() / RAND_MAX * ((100.0) - (20.0));
}

float get_wind(void) { // Range: 0.0 to 100.0 km/h
    return (float) rand() / RAND_MAX * (100.0);
}

float get_pressure(void) { // Range: 950.0 to 1050.0 hPa
    return (950.0) + (float) rand() / RAND_MAX * ((1050.0) - (950.0));
}

int findString(const char *target) {
    char *lista[] = { "ancona", "bari", "bologna", "cagliari", "catania",
            "firenze", "genova", "milano", "napoli", "palermo", "perugia",
            "pisa", "reggio calabria", "roma", "taranto", "torino", "trento",
            "trieste", "venezia", "verona" };
    int size = 20;
    for (int i = 0; i < size; i++) {
        if (strcmp(lista[i], target) == 0)
            return VALID_REQ; // trovato
    }
    return INVALID_CITY; // non trovato
}

void errorhandler(char *error_message) {
    printf("%s", error_message);
}

void handleClientConnection(int client_socket);

void handleClientConnection(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    weather_response_t response;
    weather_request_t request;

    printf("Client connesso! In attesa di messaggi...\n");

    while (1) {

        bytes_received = recv(client_socket, (char*) &request,
                sizeof(weather_request_t), 0);

        // Conversione in minuscolo PORTABILE
        strlwr(request.city);
        request.type = tolower((unsigned char)request.type);

        response.status = findString(request.city);

        if (response.status == VALID_REQ) {
            switch (request.type) {
            case 't':
                response.value = get_temperature();
                response.type = request.type;
                break;
            case 'p':
                response.value = get_pressure();
                response.type = request.type;
                break;
            case 'h':
                response.value = get_humidity();
                response.type = request.type;
                break;
            case 'w':
                response.value = get_wind();
                response.type = request.type;
                break;
            default:
                response.status = INVALID_REQ;
                break;
            };
        } else {
            response.value = 0.0;
        };

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            printf("Tipo richiesto ricevuta: %c\n", request.type);
            printf("Città richiesta ricevuta: %s\n\n", request.city);
            printf("INVIO RISPOSTA IN CORSO...\n");

            if(response.status==VALID_REQ){
                printf("\033[32mStatus:0\033[0m\n");
            }else if(response.status==INVALID_REQ){
                printf("\033[31mStatus:2\033[0m\n");
            }else if(response.status==INVALID_CITY){
                printf("\033[31mStatus:1\033[0m\n");
            };
            printf("invio risposta:Tipo: %c\n", response.type);
            printf("invio risposta:Valore: %.2f\n", response.value);

            if (send(client_socket, (char*) &response,
                    sizeof(weather_response_t), 0)
                    != sizeof(weather_response_t)) {
                errorhandler(
                        "send() sent a different number of bytes than expected");
                closesocket(client_socket);
                clearwinsock();
            }

            if (strcmp(buffer, "quit") == 0) {
                printf("Client ha richiesto la disconnessione.\n");
                break;
            }
        } else if (bytes_received == 0) {
            printf("Client disconnesso.\n");
            break;
        } else {
            errorhandler("recv() failed.\n");
            break;
        }
    }

    closesocket(client_socket);
}

int main(int argc, char *argv[]) {

    int port = SERVER_PROTOPORT;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                port = atoi(argv[i + 1]);
                if (port <= 0) {
                    printf("Errore: porta non valida.\n\n");
                    printf("Uso corretto:\n  main.exe [-p porta]\n\n");
                    return 1;
                }
                i++;
            } else {
                printf("Errore: manca il numero di porta dopo -p.\n\n");
                printf("Uso corretto:\n  main.exe [-p porta]\n\n");
                return 1;
            }
        } else {
            printf("Errore: argomento sconosciuto: %s\n\n", argv[i]);
            printf("Uso corretto:\n  main.exe [-p porta]\n\n");
            return 1;
        }
    }

    printf("Porta selezionata: %d\n", port);

#if defined WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

    if (result != NO_ERROR) {
        printf("Errore in WSAStartup()\n");
        return 0;
    }
#endif

    int my_socket;

    my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (my_socket < 0) {
        errorhandler("creazione del socket fallita.\n");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(my_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        errorhandler("bind() failed.\n");
        closesocket(my_socket);
        return -1;
    }

    if (listen(my_socket, QUEUE_SIZE) < 0) {
        errorhandler("funzione listen() fallita.\n");
        closesocket(my_socket);
        clearwinsock();
        return -1;
    }

    printf("===========================================\n");
    printf("Server TCP avviato sulla porta %d\n", port);
    printf("In attesa di connessioni...\n");
    printf("===========================================\n");

    struct sockaddr_in cad;
    int client_socket;

    while (1) {
        socklen_t client_len = sizeof(cad);

        if ((client_socket = accept(my_socket, (struct sockaddr*) &cad,
                &client_len)) < 0) {
            errorhandler("funzione accept() fallita.\n");
            closesocket(my_socket);
            clearwinsock();
            return -1;
        }
        printf("\033[32mClient Trovato!\033[0m\n");
        printf("Indirizzo IP client: %s\n", inet_ntoa(cad.sin_addr));

        handleClientConnection(client_socket);

        printf(">>> Connessione terminata.\n");
        printf("In attesa di nuove connessioni...\n\n");
    }

    closesocket(my_socket);
    clearwinsock();
    return 0;
}
