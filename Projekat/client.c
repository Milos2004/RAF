#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fcntl.h>
#include <io.h>
#include "biblioteka.h"

#pragma comment(lib, "ws2_32.lib") // za MSVC

#define PORT 8080
#define BUFSIZE 1024
struct Node* head = NULL;
char global_ime[32];
static SOCKET global_client;

BOOL WINAPI ConsoleHandler(DWORD ctrlType) {
    if (ctrlType == CTRL_CLOSE_EVENT || ctrlType == CTRL_C_EVENT) {
        char msg[BUFSIZE];
        // Slanje koda 4 serveru (za brisanje iz liste) pre gasenja
        snprintf(msg, BUFSIZE, "4:%s", global_ime);
        send(global_client, msg, strlen(msg), 0);
        closesocket(global_client);
        WSACleanup();
        
        // Kratka pauza da OS stigne da posalje TCP paket
        Sleep(500);
        return TRUE; 
    }
    return FALSE;
}

int main() {
    //Osnovna konfiguracija
    char ip_server[32];
    int custom_port;

    printf("Dobar dan! Unesite IP servera i njegov port da bi ste igrali dame (u obliku IP PORT): ");
    scanf("%s %d", ip_server, &custom_port);

    //Osnovna konfiguracija

    //Konfiguracija klijenta i veze izmedju njega i servera

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];

    WSAStartup(MAKEWORD(2,2), &wsa); // inicijalizuj winsock dll

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("socket"); 
        exit(1); 
    }

    server_addr.sin_family = AF_INET;

    // Ako korisnik unese IP i port (prosleđeno kroz argv)
    if (strcmp(ip_server, "")) {
        server_addr.sin_addr.s_addr = inet_addr(ip_server);
    } else {
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // default localhost
    }

    if (custom_port > 0) server_addr.sin_port = htons(custom_port);
    else server_addr.sin_port = htons(PORT);
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); 
        exit(1);
    }

    //Konfiguracija klijenta i veze izmedju njega i servera

    //Komunikacija izmedju servera i klijenta u prvoj fazi i dva klijenta u drugoj fazi ali server odrzava vezu

    int sent = 0;
    int receive;
    int flag = 0;
    int igra = 0;
    int c = 0;
    char game;
    char ime[32];
    char odgovor[32];
    char odgovor_gost[32];
    char odgovor_igra[32];
    char gost[32];
    char* ip = ip_finder();
    char ip_igra[32];
    char msg[BUFSIZE];
    char poruka[BUFSIZE];
    global_client = sock;

    while(1){
        SetConsoleCtrlHandler(ConsoleHandler, TRUE);    
        switch (sent)
        {
        case 0: //Prijava na listu
            printf("Unesite korisnicko ime (username) da biste se prijavili na listu: ");
            scanf("%s", ime);
            strcpy(global_ime, ime);
            snprintf(poruka, BUFSIZE, "%s,%s", ime, ip);
            snprintf(msg, BUFSIZE, "%d:%s", sent, poruka);
            send(sock, msg, strlen(msg), 0);
            break;
        case 1: //Trazenje partnera
            printf("Da li hoces da pozoves nekoga ili hoces da sacekas da te neko pozove? (DA ili NE): ");
            scanf("%s", odgovor);
            if(strcmp(odgovor, "DA") == 0){
                flag = 1;
                printf("Unesi ime igraca sa kojim hoces da igras: ");
                scanf("%s", gost);
                snprintf(poruka, BUFSIZE, "%s,%s", ime, gost);
                snprintf(msg, BUFSIZE, "%d:%s", sent, poruka);
                send(sock, msg, strlen(msg), 0);
            }
            else printf("Cekanje na drugog igraca...\n");
            break;
        case 2: //prihvatanje konekcije
            if(!flag){
                sscanf(poruka, "%s", gost);
                scanf("%s", odgovor_gost); //            da          biza milos
                snprintf(poruka, BUFSIZE, "%s,%s,%s", odgovor_gost, gost, ime);
                snprintf(msg, BUFSIZE, "%d:%s", sent, poruka);
                send(sock, msg, strlen(msg), 0);
            }
            break;
        case 3: //Slanje ip radi uspostavljanja veze
            snprintf(poruka, BUFSIZE, "%s", ip);
            snprintf(msg, BUFSIZE, "%d:%s", sent, poruka);
            send(sock, msg, strlen(msg), 0);
            break;
        case 4: //ispis lista nakon neuspelog povezivanja
            snprintf(msg, BUFSIZE, "%d:%s", 6, ime);
            send(sock, msg, strlen(msg), 0);
            break;
        default:
            break;
        }
       
        if(strcmp(odgovor_gost, "NE") == 0) receive = 2;
        else if(!(flag==1 && sent == 3)){
            memset(buffer, 0, BUFSIZE);
            recv(sock, buffer, BUFSIZE - 1, 0);
            sscanf(buffer, "%d:%[^\0]", &receive, poruka);
        }
        else receive = 3;

        switch (receive)
        {
        case 0:
            printf("%s\n", poruka);
            sent = 1;
            break;
        case 1:
            printf("%s", poruka);
            sent = 2;
            break;
        case 2:
            if(flag){
                if(strstr(poruka, "prihvatio")){
                    printf("%s\n", poruka);
                    sent = 3;
                }
                else{
                    printf("%s\n", poruka);
                    flag = 0;
                    sent = 4;
                }
            } 
            else{
                if(strcmp(odgovor_gost, "NE") == 0){
                    printf("Tvoj odgovor: %s \n", odgovor_gost);
                    sent = 4;
                    strcpy(odgovor_gost, "");
                } 
                else sent = 5;
            } 
            break;
        case 3:
            if(flag){
                printf("***Pocinje igra, ti si O, a %s je X, poslao sam ip***\n", gost);
                igra = 1;
                player_server(ip, ime, gost);
            }
            else {
                sscanf(poruka, "%s", ip_igra);
                printf("***Pocinje igra, ti si X, a %s je O, ip je: %s***\n", gost, ip_igra);
                igra = 1;
                player_client(ip_igra, ime, gost);
            }//funkcija za igru, Dragan prosledjuje ip adresu od Milosa u nju da bi se povezo sa Milosem
            break;
        case 4:
            printf("%s\n", poruka);
            sent = 1;
            break;
        default:
            break;
        }

        if(igra){
            printf("Da li hoces da nastavis ili neces? (DA ili NE): ");
            scanf("%s", odgovor_igra);
            if(strcmp(odgovor_igra, "DA") == 0){
                sent = 1;
                flag = 0;
                igra = 0;
                snprintf(msg, BUFSIZE, "%d:%s", 5, ime);
                printf("%s\n", msg);
                send(sock, msg, strlen(msg), 0);

                memset(buffer, 0, BUFSIZE);
                recv(sock, buffer, BUFSIZE - 1, 0);
                sscanf(buffer, "%d:%[^\0]", &c, poruka);

                printf("%s\n", poruka);
            }
            else{
                snprintf(msg, BUFSIZE, "%d:%s", 4, ime);
                send(sock, msg, strlen(msg), 0);

                memset(buffer, 0, BUFSIZE);
                recv(sock, buffer, BUFSIZE - 1, 0);
                sscanf(buffer, "%[^\0]", poruka);

                printf("%s\n", poruka);

                closesocket(sock);

                return 0;
            }
        }
        //kad se zavrsi igra igraci ce se vratiti ovde pa ce im biti postavljeno pitanje da li hoce da nastave ako da onda sent=1 ako ne onda closesocket i salje se signal serveru da se brise iz liste
    }    

}
