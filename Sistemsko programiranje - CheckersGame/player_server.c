#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>
#include "biblioteka.h"

#pragma comment(lib, "ws2_32.lib") // ukljuci winsock library
#define PORT 8080
#define BUFSIZE 1024

volatile sig_atomic_t stop_s = 0; 
static SOCKET global_client, global_server;

BOOL WINAPI handle_sigint_server(DWORD fdwCtrlType) {
    if (fdwCtrlType == CTRL_C_EVENT) {
        printf("\n[SISTEM] Ctrl+C detektovan! Prekidam partiju...\n");
        send(global_client, "rage", 4, 0);
        // Ne čekaj recv ovde jer ako je protivnik ugašen, program će se opet zamrznuti
    }
    stop_s = 1;
    Sleep(500);
    return TRUE;
}

int player_server(char* ip, char* ime, char* gost) {
    WSADATA wsa;
    SOCKET server_fd, client_fd;  //fd-file descriptor
    struct sockaddr_in addr, client;
    int c;
    char buffer[BUFSIZE];
    WSAStartup(MAKEWORD(2,2), &wsa); // inicijalizuj winsock dll
    server_fd = socket(AF_INET, SOCK_STREAM, 0);  //AF_INET - address family, IPv4
    if (server_fd == INVALID_SOCKET) { perror("socket"); exit(1); }
    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = INADDR_ANY;

    addr.sin_port = htons(PORT);
    

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("bind"); exit(1);
    }
    listen(server_fd, 3);
    //custom_port == 0 ? printf("Server listening on port %d...\n", PORT) : printf("Server listening on port %d...\n", custom_port);
    int board[8][8] = {{0,2,0,2,0,2,0,2}, 
                       {2,0,2,0,2,0,2,0},
                       {0,2,0,2,0,2,0,2},
                       {0,0,0,0,0,0,0,0},
                       {0,0,0,0,0,0,0,0},
                       {1,0,1,0,1,0,1,0},
                       {0,1,0,1,0,1,0,1},
                       {1,0,1,0,1,0,1,0}}; //0-slobodno, 1-X, 2-O //0-slobodno, 1-X, 2-O
    ispis(board);
    c = sizeof(struct sockaddr_in);
    //čeka klijenta i komunicira poteze
    while(1){
        client_fd = accept(server_fd, (struct sockaddr*)&client, &c);
        if(client_fd == INVALID_SOCKET){
            perror("accept");
            continue;
        } else {
            break;
        }
    }
    //docekao klijenta. prima potez, salje potez i tako do kraja partije (necija pobeda ili ukupno 9 poteza)
    //ovde je tabla

    global_client = client_fd;
    global_server = server_fd;

    int od_x, od_y, do_x, do_y;
    char od_y_c, do_y_c;

    //ide komunikacija
    while(1){
        SetConsoleCtrlHandler(handle_sigint_server, TRUE);
        printf("Cekamo potez protivnika: \n");
        memset(buffer, 0, BUFSIZE);
        recv(client_fd, buffer, BUFSIZE - 1, 0);
        if(stop_s) break;

        if(strcmp(buffer, "rage") == 0){
            printf("Protivnik je napustio igru, pobedili ste!\n");
            break;
        }

        printf("Stigao potez od igraca zvanog %s: %s\n", gost, buffer);
        
        sscanf(buffer, "%d,%c,%d,%c", &od_x, &od_y_c, &do_x, &do_y_c);
        
        od_y = od_y_c - 'A' + 1;
        do_y = do_y_c - 'A' + 1;
        
        if(do_x == 1 || board[od_x-1][od_y-1] == 3) board[do_x-1][do_y-1] = 3; //postavi x na tablu
        else board[do_x-1][do_y-1] = 1;//postavi X na tablu igrao je klijent
        
        if (board[od_x-1][od_y-1] == 1)
        {
            if(board[od_x-2][od_y-2] == 2 && do_y < od_y) board[od_x-2][od_y-2] = 0;
            else if(board[od_x-2][od_y] == 2 && do_y > od_y) board[od_x-2][od_y] = 0;
        }
        else {
            if(board[od_x][od_y-2] == 2 && do_y < od_y) board[od_x][od_y-2] = 0;
            else if(board[od_x][od_y] == 2 && do_y > od_y) board[od_x][od_y] = 0;
        }
        board[od_x-1][od_y-1] = 0;

        Sleep(500);
        ispis(board);
        Sleep(1000);

        if(provera_kraja(board) > 0){
            printf("X je pobedio!\n"); //ispisuje ako je kraj
            break;
        }
        else if(provera_kraja(board) == 0){
            printf("Nereseno!\n"); //ispisuje ako je kraj
            break;
        }
        

        //sada je na redu server (O)
        printf("Moli se %s da upise svoj potez u formatu od_x od_y do_x do_y : ", ime);
        int x = 1;
        while(x){
            scanf("%d %c %d %c", &od_x, &od_y_c, &do_x, &do_y_c);

            od_y = od_y_c - 'A' + 1;
            do_y = do_y_c - 'A' + 1;

            if(!(board[od_x-1][od_y-1] == 2 || board[od_x-1][od_y-1] == 4)){
                printf("Igras sa O ne sa X, unesi koordinate od O: ");
                continue;
            } 
            if(validnost_server(od_x, od_y, do_x, do_y, board)){
                printf("Potez nije validan, pokusajte ponovo: ");
                continue;
            } 
            board[do_x-1][do_y-1] == 0 ? x = 0 : printf("Potez nije validan, pokusajte ponovo: ");
        }
        if(do_x == 8 || board[od_x-1][od_y-1] == 4) board[do_x-1][do_y-1] = 4; //dama oks - o
        else board[do_x-1][do_y-1] = 2;//postavi O na tablu

        if(board[od_x-1][od_y-1] == 2){
            if(board[od_x][od_y-2] == 1 && do_y < od_y) board[od_x][od_y-2] = 0;
            else if(board[od_x][od_y] == 1 && do_y > od_y) board[od_x][od_y] = 0;
        }
        else {
            if(board[od_x-2][od_y-2] == 1 && do_y < od_y) board[od_x-2][od_y-2] = 0;
            else if(board[od_x-2][od_y] == 1 && do_y > od_y) board[od_x-2][od_y] = 0;
        }
        board[od_x-1][od_y-1] = 0;

        Sleep(500);
        ispis(board);
        Sleep(1000);
        
        //posalji potez klijentu
        char reply[BUFSIZE];
        snprintf(reply, BUFSIZE, "%d,%c,%d,%c", od_x, od_y_c, do_x, do_y_c);
        send(client_fd, reply, strlen(reply), 0);

        if(provera_kraja(board) > 0){ //ispisuje ako je kraj
            printf("O je pobedio!\n");
            break;
        }
        else if(provera_kraja(board) == 0){ //ispisuje ako je kraj
            printf("Nereseno!\n");
            break;
        }
    }
    //kraj igre, zatvara sve
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup(); // zavrsi sa winsock dll
    return 0;
}
