#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include "biblioteka.h"

#pragma comment(lib, "ws2_32.lib") // ukljuci winsock library
#define PORT 8080
#define BUFSIZE 1024


typedef struct {
    SOCKET client_fd;
    int broj;
    struct Node* head;
} thread_args_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct Node* head = NULL;

void* thread_function(void* arg){
    //Ucitavanje svih potrebnih parametara
    thread_args_t* args = (thread_args_t*)arg;
    SOCKET client_fd = args->client_fd;
    int broj_igraca = args->broj;
    struct Node* temp = head;

    char buffer[BUFSIZE];
    char poruka[BUFSIZE];
    char msg[BUFSIZE];
    char msg2[BUFSIZE];
    char ime[32];
    char ip[32];
    char gost[32];
    char odgovor[32];

    char reply[BUFSIZE];
    int sent;
    int gfla = 1;
    int flag = 0;
    int receive;

    while(1){
        memset(buffer, 0, BUFSIZE);
        recv(client_fd, buffer, BUFSIZE - 1, 0);
        printf("%s\n", buffer);
        sscanf(buffer, "%d:%[^\0]", &receive, poruka);
        printf("%s\n", poruka);

        switch (receive)
        {
        case 0: //Prijava na listu
            sscanf(poruka, "%[^,],%[^,\n]", ime, ip);
            sent = 0;
            pthread_mutex_lock(&mutex);
            insertAtEnd(&head, ime, ip, 'S', broj_igraca, client_fd);
            print_server(head);
            printf("%d\n", (int)client_fd);
            pthread_mutex_unlock(&mutex);
            break;
        case 1: //Trazenje partnera
            sscanf(poruka, "%[^,],%s", ime, gost);
            sent = 1;
            pthread_mutex_lock(&mutex);
            temp = head;
            while(temp != NULL){
                if(strcmp(temp->ime, ime) == 0){
                    temp->game = 'Z';
                    break;
                } 
                temp = temp->next;
            }
            temp = head;
            int exist = 0;
            while (temp != NULL) {
                if (strcmp(temp->ime, gost) == 0) {
                    exist = 1;
                    temp->game = 'Z';
                    snprintf(msg, BUFSIZE, "%s hoce da igra sa tobom, da li hoces? (odgovori sa DA ili NE): ", ime);
                    snprintf(reply, BUFSIZE, "%d:%s", sent, msg);
                    printf("%s, %s\n", temp->ime, reply);
                    send(temp->socket, reply, strlen(reply), 0);
                    break;
                }
                temp = temp->next;
            }
            if(!exist){
                temp = head;
                while(temp != NULL){
                    if(strcmp(temp->ime, ime) == 0){
                        snprintf(msg, BUFSIZE, "Igrac ne postoji ili je zauzet, pokusaj ponovo: ");
                        snprintf(reply, BUFSIZE, "%d:%s", 4, msg);
                        printf("%s, %s\n", temp->ime, reply);
                        temp->game = 'S';
                        send(temp->socket, reply, strlen(reply), 0);
                        break;
                    } 
                    temp = temp->next;
                }
            }
            pthread_mutex_unlock(&mutex);
            break;
        case 2: //prihvatanje konekcije       da      milos dragan
            sscanf(poruka, "%[^,],%[^,],%s", odgovor, ime, gost);
            sent = 2;
            pthread_mutex_lock(&mutex);
            temp = head;
            while (temp != NULL) {
                if (strcmp(temp->ime, ime) == 0) { //pronalazim milosa da mu potvrdim da hocu
                    printf("Nasao sam ga\n");
                    if (strcmp(odgovor, "DA") == 0){
                        snprintf(msg, BUFSIZE, "%s je prihvatio, igra pocinje", gost); //dragan salje milosu
                    } 
                    else{
                        snprintf(msg, BUFSIZE, "%s je odbio da igra sa tobom, pitaj nekog drugog", gost);
                        temp->game = 'S';
                    }
                    snprintf(reply, BUFSIZE, "%d:%s", sent, msg);
                    printf("%s, %s", temp->ime, reply);
                    send(temp->socket, reply, strlen(reply), 0); //slanje na milosev socket
                    break;
                }
                temp = temp->next;
            }
            pthread_mutex_unlock(&mutex);
            break;
        case 3: //ip
            sscanf(poruka, "%s", ip);
            sent = 3;
            pthread_mutex_lock(&mutex);
            temp = head;
            while (temp != NULL) {
                if (strcmp(temp->ime, gost) == 0) { 
                    snprintf(reply, BUFSIZE, "%d:%s", sent, ip); 
                    send(temp->socket, reply, strlen(reply), 0);
                    break;
                }
                temp = temp->next;
            }
            pthread_mutex_unlock(&mutex);
            break;
        case 4: //brisanje
            sscanf(poruka, "%s", ime);
            sent = 4;
            pthread_mutex_lock(&mutex);
            temp = head;
            while (temp != NULL) {
                if (strcmp(temp->ime, ime) == 0) { 
                    snprintf(reply, BUFSIZE, "%s", "Uspesno si izbrisan sa liste! Hvala na igranju i dodji nam ponovo!"); 
                    send(temp->socket, reply, strlen(reply), 0);
                    Sleep(1000);
                    closesocket(temp->socket);
                    printf("[SERVER] Nit za igraca %s se gasi.\n", ime);
                    deleteByName(&head, temp->ime);
                    break;
                }
                temp = temp->next;
            }
            pthread_mutex_unlock(&mutex);
            return NULL; // <--- OVO ZAUSTAVLJA SVE (izlazi iz thread_function)
            break;
        case 5: //ostajanje na serveru
            sscanf(poruka, "%s", ime);
            sent = 0;
            printf("Usao sam u case 5\n");
            pthread_mutex_lock(&mutex);
            temp = head;
            print_server(temp);
            while (temp != NULL) {
                if (strcmp(temp->ime, ime) == 0) { 
                    temp->game = 'S';
                    printf("saljem\n"); 
                    break;
                }
                temp = temp->next;
            }
            pthread_mutex_unlock(&mutex);
            break;
        case 6://ispis
            sscanf(poruka, "%s", ime);
            pthread_mutex_lock(&mutex);
            temp = head;
            while(temp != NULL){
                if(strcmp(temp->ime, ime) == 0) temp->game = 'S';
                temp = temp->next;
            }
            pthread_mutex_unlock(&mutex);
            sent = 0;
            break;
        default:
            break;
        }

        switch (sent)
        {
        case 0:
            strcpy(msg, "");
            if(receive != 6 && receive != 5) strcpy(msg, "Dobro dosli na server!\n");
            else if(receive == 5) strcpy(msg, "Novi krug pocinje!\n");
            strcat(msg, "Lista slobodnih igraca:\n");

            pthread_mutex_lock(&mutex);
            temp = head;
            if(temp->next == NULL){
                strcat(msg, "Nema slobodnih igraca\n");
            }
            else {
                while (temp != NULL) {
                    if (temp->position != broj_igraca && temp->game == 'S') {
                        strcat(msg, temp->ime);
                        strcat(msg, "\n");
                    }
                    temp = temp->next;
                }
            }
            if(strcmp(msg, "Dobro dosli na server!\nLista slobodnih igraca:\n") == 0) strcat(msg, "Nema slobodnih igraca\n"); //ako su svi igraci zauzeti
            pthread_mutex_unlock(&mutex);
            snprintf(reply, BUFSIZE, "%d:%s", sent, msg);
            if(receive == 5) printf("%s\n%d\n%s\n", ime, (int)client_fd, reply);
            send(client_fd, reply, strlen(reply), 0);
            break;
        default:
            break;
        }

    }

}

int main(int argc, char *argv[]) {
    //Konfiguracija i podizanje servera
    WSADATA wsa;
    SOCKET server_fd, client_fd;  // fd-file descriptor
    struct sockaddr_in addr, client;
    int c;
    char buffer[BUFSIZE];
    int custom_port = 0;
    int broj_igraca = 0;

    WSAStartup(MAKEWORD(2,2), &wsa); // inicijalizuj winsock dll

    server_fd = socket(AF_INET, SOCK_STREAM, 0);  // AF_INET - address family, IPv4
    if (server_fd == INVALID_SOCKET) { perror("socket"); exit(1); }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    // Ako je prosledjen port u argv[1], koristi ga; u suprotnom PORT
    if (argc >= 2) {
        custom_port = atoi(argv[1]);
        if (custom_port > 0) addr.sin_port = htons(custom_port);
        else addr.sin_port = htons(PORT);
    } else {
        addr.sin_port = htons(PORT);
    }

    // bind socket na port
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("bind"); exit(1);
    }

    listen(server_fd, 30); // slusaj do 30 konekcija

    c = sizeof(struct sockaddr_in);

    printf("Server uspesno ucitan!\n");
    printf("IP adresa servera je: %s\n", ip_finder());
    custom_port > 0 ? printf("Port je %d\n", custom_port) : printf("Port je %d\n", PORT);
    printf("LOGS:\n");

    //Konfiguracija i podizanje servera

    //Komunikacija sa klijentom

    // ceka klijenta
    while (1) {

        client_fd = accept(server_fd, (struct sockaddr*)&client, &c);
        if (client_fd == INVALID_SOCKET) {
            perror("accept");
            continue;
        }

        pthread_mutex_lock(&mutex);
        ++broj_igraca;
        pthread_mutex_unlock(&mutex);
        thread_args_t* args = malloc(sizeof(thread_args_t));
        args->client_fd = client_fd;
        args->broj = broj_igraca;

        pthread_t thread;
        //NA NA VIVA LA PANAMA
        pthread_create(&thread, NULL, thread_function, args);
        pthread_detach(thread);

        //Za svakog klijenta se kreira thread, kad se kreira on se i otkaci da bi mogao da se napravi thread za drugog igraca

    }

    // kraj igre, zatvori socket-e
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup(); // zavrsi sa winsock dll

    //Komunikacija sa klijentom

    return 0;
}
