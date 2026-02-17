// // C Program for Implementation of Singly Linked List
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>
#include "biblioteka.h"

// Define the Node structure

// Function to create a new node 
struct Node* createNode(char* ime, char* ip, char game, int position, SOCKET socket) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->ime, ime);
    strcpy(newNode->ip, ip);
    newNode->game = game;
    newNode->position = position;
    newNode->socket = socket;   // <<< BITNO
    newNode->next = NULL;
    return newNode;
}
// Function to insert a new element at the end of the singly linked list
void insertAtEnd(struct Node** head, char* ime, char* ip, char game, int position, SOCKET socket) {
    struct Node* newNode = createNode(ime, ip, game, position, socket);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    struct Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Function to delete the first node of the singly linked list
void deleteFromFirst(struct Node** head) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }
    struct Node* temp = *head;
    *head = temp->next;
    free(temp);
}

// Function to delete a node at a specific position in the singly linked list
void deleteByName(struct Node** head, char* ime) {
    if (*head == NULL) {
        printf("Lista je prazna.\n");
        return;
    }

    struct Node* temp = *head;
    struct Node* prev = NULL;

    // Slučaj 1: Ako je igrač koga brišemo na samom početku (glava liste)
    if (temp != NULL && strcmp(temp->ime, ime) == 0) {
        *head = temp->next; // Pomeri glavu na sledeći čvor
        printf("Igrac %s je uspesno obrisan sa pocetka liste.\n", ime);
        free(temp);         // Oslobodi memoriju obrisanog čvora
        return;
    }

    // Slučaj 2: Tražimo igrača u ostatku liste
    while (temp != NULL && strcmp(temp->ime, ime) != 0) {
        prev = temp;
        temp = temp->next;
    }

    // Ako nismo našli igrača do kraja liste
    if (temp == NULL) {
        printf("Igrac sa imenom %s nije pronadjen u listi.\n", ime);
        return;
    }

    // Prespajanje: Prethodni čvor sada pokazuje na onaj posle obrisanog
    prev->next = temp->next;

    printf("Igrac %s je uspesno obrisan iz liste.\n", ime);
    free(temp); // Oslobađanje memorije
}


// Function to print the LinkedList
void print_server(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("Igrac broj %d: [%s, %s]\n", temp->position, temp->ime, temp->ip);
        temp = temp->next;
    }
}

void print_client(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("Igrac broj %d: [%s]\n", temp->position, temp->ime);
        temp = temp->next;
    }
}