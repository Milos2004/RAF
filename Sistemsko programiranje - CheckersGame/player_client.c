#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h>
#include <signal.h>
#include "biblioteka.h"

#pragma comment(lib, "ws2_32.lib") // ukljuci winsock library
#define PORT 8080
#define BUFSIZE 1024

volatile sig_atomic_t stop_c = 0; 
static SOCKET global_sock;

BOOL WINAPI handle_sigint_client(DWORD fdwCtrlType) {
    if (fdwCtrlType == CTRL_C_EVENT) {
        printf("\n[SISTEM] Ctrl+C detektovan! Prekidam partiju...\n");
        send(global_sock, "rage", 4, 0);
    }
    else if(fdwCtrlType == CTRL_CLOSE_EVENT){
        closesocket(global_sock);
        WSACleanup();
    }
    Sleep(500);
    stop_c = 1;
    return TRUE;
}

int player_client(char* ip, char* ime, char* gost) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { perror("socket"); exit(1); }

    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr = inet_addr(ip);

    server_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); exit(1);
    }

    //konektovao se. krece igra
    int board[8][8] = {{0,2,0,2,0,2,0,2}, 
                       {2,0,2,0,2,0,2,0},
                       {0,2,0,2,0,2,0,2},
                       {0,0,0,0,0,0,0,0},
                       {0,0,0,0,0,0,0,0},
                       {1,0,1,0,1,0,1,0},
                       {0,1,0,1,0,1,0,1},
                       {1,0,1,0,1,0,1,0}}; //0-slobodno, 1-X, 2-O

    ispis(board);

    global_sock = sock;

    int od_x, od_y, do_x, do_y;
    char od_y_c, do_y_c;

    //ide komunikacija
    while(1){
        SetConsoleCtrlHandler(handle_sigint_client, TRUE);
        //salji potez
        printf("Moli se %s da upise svoj potez u formatu od_x od_y do_x do_y : ", ime);
        int x = 1;
        while(x){
            scanf("%d %c %d %c", &od_x, &od_y_c, &do_x, &do_y_c);

            od_y = od_y_c - 'A' + 1;
            do_y = do_y_c - 'A' + 1;
            
            if(!(board[od_x-1][od_y-1] == 1 || board[od_x-1][od_y-1] == 3)){
                printf("Igras sa X ne sa O, unesi koordinate od X: ");
                continue;
            }
            if(validnost_client(od_x, od_y, do_x, do_y, board)){
                printf("Potez nije validan, pokusajte ponovo: ");
                continue;
            }
            board[do_x-1][do_y-1] == 0 ? x = 0 : printf("Potez nije validan, pokusajte ponovo: ");
        }
        if(do_x == 1 || board[od_x-1][od_y-1] == 3) board[do_x-1][do_y-1] = 3; //postavi x na tablu
        else board[do_x-1][do_y-1] = 1;//postavi X na tablu
        
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
        
        char msg[BUFSIZE];
        snprintf(msg, BUFSIZE, "%d,%c,%d,%c", od_x, od_y_c, do_x, do_y_c);
        
        send(sock, msg, strlen(msg), 0);

        if(provera_kraja(board) > 0){ //ispisuje ako je kraj
            printf("X je pobedio!\n");
            break;
        }
        else if(provera_kraja(board) == 0){ //ispisuje ako je kraj
            break;
        }
        //primi potez

        printf("Cekamo potez protivnika: \n");

        memset(buffer, 0, BUFSIZE);
        recv(sock, buffer, BUFSIZE - 1, 0);
        if(stop_c) break;

        if(strcmp(buffer, "rage") == 0){
            printf("Protivnik je napustio igru, pobedili ste!\n");
            break;
        }
        printf("Stigao potez od igraca zvanog %s: %s\n", gost, buffer);
        sscanf(buffer, "%d,%c,%d,%c", &od_x, &od_y_c, &do_x, &do_y_c);

        od_y = od_y_c - 'A' + 1;
        do_y = do_y_c - 'A' + 1;
            
        if(do_x == 8 || board[od_x-1][od_y-1] == 4) board[do_x-1][do_y-1] = 4; //dama oks - o
        else board[do_x-1][do_y-1] = 2;//postavi O na tablu igrao je server

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

        if(provera_kraja(board) > 0){
            printf("O je pobedio!\n"); //ispisuje ako je kraj
            break;
        }
        else if(provera_kraja(board) == 0){
            printf("Nereseno!\n"); //ispisuje ako je kraj
            break;
        }
    }
    closesocket(sock);
    WSACleanup();

    return 0;
}
