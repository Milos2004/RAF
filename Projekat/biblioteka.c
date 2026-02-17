#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <winsock2.h>
#include <fcntl.h>
#include <io.h>
#include <wchar.h>
#include <string.h>
#include "biblioteka.h"

void ispis(int board[8][8]) {
    char simboli[] = {' ', 'X', 'O', 'x', 'o'};
    printf("\n");
    printf("   | A | B | C | D | E | F | G | H |   \n");
    printf("---+---+---+---+---+---+---+---+---+---\n");
    for(int i=0; i<8; i++){
        printf(" %c |", '1' + i);
        for(int j=0; j<8; j++){
            printf(" %c ", simboli[board[i][j]]);
            if(j < 7) printf("|");
        }
        printf("| %c", '1' + i);
        printf("\n");
        if(i <= 7) printf("---+---+---+---+---+---+---+---+---+---\n");
        if(i == 7){
            printf("   | A | B | C | D | E | F | G | H |   \n");
        }
    }
    printf("\n");
} 

int provera_kraja(int board[8][8]){
    // proveri X (1)
    int gfla = 1;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if((board[i][j] == 2) || (board[i][j] == 4)) gfla = 0;
        }
    }
    if(gfla) return 1;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] == 1 || board[i][j] == 3) gfla = 0;
        }
    }
    if(gfla) return 2;
    return -1; // igra traje
}

bool validnost_client(int od_x, int od_y, int do_x, int do_y, int board[8][8]){
    if(board[od_x-1][od_y-1] == 3) {
        bool skok1 = ((abs(od_x - do_x) > 1 || abs(od_y - do_y) > 1) && (board[od_x][od_y] != 2 || board[od_x][od_y-2] != 2)); //kad ispred nema nista
        bool skok2 = !(abs(od_x - do_x) == 2 && abs(od_y - do_y) == 2 && (board[od_x][od_y] == 2 || board[od_x][od_y-2] == 2)); //kad su ispred jedan ili dva oksa
        bool skok = abs(od_x - do_x) == 1 ? skok1 : skok2;
        return (od_x == do_x || od_y == do_y || skok || (do_x + do_y) % 2 == 0);
    }
    else{
        bool skok1 = ((abs(od_x - do_x) > 1 || abs(od_y - do_y) > 1) && (board[od_x-2][od_y] != 2 || board[od_x-2][od_y-2] != 2)); //kad ispred nema nista
        bool skok2 = !(abs(od_x - do_x) == 2 && abs(od_y - do_y) == 2 && (board[od_x-2][od_y] == 2 || board[od_x-2][od_y-2] == 2)); //kad su ispred jedan ili dva oksa
        bool skok = abs(od_x - do_x) == 1 ? skok1 : skok2;
        return (od_x == do_x || od_y == do_y || skok || (do_x + do_y) % 2 == 0 || od_x < do_x);
    }
}

bool validnost_server(int od_x, int od_y, int do_x, int do_y, int board[8][8]){
    if (board[od_x-1][od_y-1] == 4){
        bool skok1 = ((abs(od_x - do_x) > 1 || abs(od_y - do_y) > 1) && (board[od_x-2][od_y-2] != 1 || board[od_x-2][od_y] != 1));
        bool skok2 = !(abs(od_x - do_x) == 2 && abs(od_y - do_y) == 2 && (board[od_x-2][od_y-2] == 1 || board[od_x-2][od_y] == 1));
        bool skok = abs(od_x - do_x) == 1 ? skok1 : skok2;
        return (od_x == do_x || od_y == do_y || skok || (do_x + do_y) % 2 == 0);
    }
    else{
        bool skok1 = ((abs(od_x - do_x) > 1 || abs(od_y - do_y) > 1) && (board[od_x][od_y-2] != 1 || board[od_x][od_y] != 1));
        bool skok2 = !(abs(od_x - do_x) == 2 && abs(od_y - do_y) == 2 && (board[od_x][od_y-2] == 1 || board[od_x][od_y] == 1));
        bool skok = abs(od_x - do_x) == 1 ? skok1 : skok2;
        return (od_x == do_x || od_y == do_y || skok || (do_x + do_y) % 2 == 0 || od_x > do_x);
    }
}

char* ip_finder(){
    char linija[256];
    static char ip_adresa[32] = "";
    
    // Otvaramo cev ka komandi ipconfig
    // _popen pokreće komandu i dozvoljava nam da čitamo njen ispis ("r" - read)
    FILE* cev = _popen("ipconfig | findstr /i \"IPv4\"", "r");

    if (cev == NULL) {
        printf("Greška pri pokretanju komande, rucno unesite IP adresu.\n");
        return NULL;
    }

    // Čitamo red po red ispisa
    while (fgets(linija, sizeof(linija), cev) != NULL) {
        // Tražimo gde se u redu nalazi dvotačka, jer IP ide posle nje
        char* pozicija_dvotacke = strrchr(linija, ':');
        
        if (pozicija_dvotacke != NULL) {
            // sscanf čita formatiran tekst nakon dvotačke (preskače razmak)
            sscanf(pozicija_dvotacke + 1, "%s", ip_adresa);
            break; // Uzimamo prvu IP adresu na koju naiđemo
        }
    }

    _pclose(cev); // Obavezno zatvaranje cevi

    return ip_adresa;
}

