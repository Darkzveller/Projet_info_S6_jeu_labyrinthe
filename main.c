#include "Variable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"
#include "affichage.h"
    typedef struct
    {
        int x;
        int y;
        int type_insertion;
        int indice;
        int rotation;
    } t_joueur;

int main()
{

    t_joueur yek = {0, 0, 0, 0, 0};

    printf("caca\n");

    printf("Tentative de connexion au serveur \n");
    connectToServer(nom_serveur, port_serveur, nom_bot_moi);
    printf("Connecter au serveur de jeu en tant que %s \n", nom_bot_moi);

    printf("Attente d'une partie %s...\n", type_partie_choisi[BOT_BOUGE_PAS]);
    waitForLabyrinth(type_partie_choisi[BOT_BOUGE_PAS], labyrinthName, &sizeX, &sizeY);
    printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", labyrinthName, sizeX, sizeY);

    taille_buffer = (sizeX * sizeY + 5) * 11;
    labyData = malloc(taille_buffer * sizeof(char));
    numero_joueur = getLabyrinth(labyData);
    printf("Données recu du serveur sont numéro du joueur : %d\n", numero_joueur);
    printf("Les data du labyrinthe : \n%s\n\n", labyData);
    initAffichage(sizeX, sizeY);

    t_return_code resultat = NORMAL_MOVE;
    int c_mon_tour_jouer = numero_joueur;
    char coup_envoi[MAX_GET_MOVE];
    char coup_recu[MAX_GET_MOVE];
    char message_serveur[MAX_MESSAGE];

    // int p0_x = 0, p0_y = 0;
    yek.x = 0;
    yek.y = 0;

    int p1_x = sizeX - 1, p1_y = sizeY - 1;

    while (resultat == NORMAL_MOVE)
    {
        printf("\nAffichage du labyrinthe :\n");
        printLabyrinth();

        afficheLabyrinthe(labyData, 500, sizeX, sizeY, yek.x, yek.y, p1_x, p1_y);

        if (c_mon_tour_jouer == 0)
        {
            int type, indice, rot, x, y;
            printf("Entre coup chef : ");
            scanf("%d %d %d %d %d", &yek.type_insertion, &yek.indice, &yek.rotation, &yek.x, &yek.y);
            // p0_x = x;
            // p0_y = y;
            // sprintf(coup_envoi, "%d %d %d %d %d", type, indice, rot, x, y);
            sprintf(coup_envoi, "%d %d %d %d %d", yek.type_insertion, yek.indice, yek.rotation, yek.x, yek.y);

            resultat = sendMove(coup_envoi, message_serveur);

            c_mon_tour_jouer = 1;
        }
        else
        {
            printf("Attente du coup de l'adversaire\n");

            // On récupère le coup de l'adversaire
            resultat = getMove(coup_recu, message_serveur);

            printf("L'adversaire a joué : %s\n", coup_recu);

            c_mon_tour_jouer = 0;

            int type_adversaire, indide_adversaire, rotation__adversaire, ax_adversaire, ay_adversaire;
            sscanf(coup_recu, "%d %d %d %d %d", &type_adversaire, &indide_adversaire, &rotation__adversaire, &ax_adversaire, &ay_adversaire);

            // On met à jour les coordonnées de l'adversaire
            p1_x = ax_adversaire;
            p1_y = ay_adversaire;

            printf("L'adversaire a joué vers : type : %d indice : %d rotation: %d x : %d y : %d\n", type_adversaire, indide_adversaire, rotation__adversaire, p1_x, p1_y);
        }
    }

    // Affichage de la raison de la fin (gagné ou autre)
    printf("Fin de la partie ! Raison : %s\n", message_serveur);

    free(labyData);
    closeConnection();
    return 0;
}