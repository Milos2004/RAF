#ifndef BIBLIOTEKA_H
#define BIBLIOTEKA_H
#include <stdbool.h>
#include <winsock2.h>

void ispis(int board[8][8]);
int provera_kraja(int board[8][8]);
bool validnost_client(int od_x, int od_y, int do_x, int do_y, int board[8][8]);
bool validnost_server(int od_x, int od_y, int do_x, int do_y, int board[8][8]);
char* ip_finder();
struct Node {
    char ime[32];
    char ip[32];
    char game;
    int position;
    SOCKET socket; 
    struct Node* next;
};
struct Node* createNode(char* ime, char* ip, char game, int position, SOCKET socket);
void insertAtEnd(struct Node** head, char* ime, char* ip, char game, int position, SOCKET socket);
void deleteByName(struct Node** head, char* ime);
void print_server(struct Node* head);
int player_server(char* ip, char* ime, char* gost);
int player_client(char* ip, char* ime, char* gost);

#endif