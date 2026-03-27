#include "Variable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"
#include "affichage.h"
typedef struct
{
    char labyrinthName[50];
    int sizeX;
    int sizeY;
    char *labyData;
} t_laby;

int main()
{

    t_laby laby;
    printf("caca\n");

    printf("Tentative de connexion au serveur \n");
    connectToServer(nom_serveur, port_serveur, nom_bot_moi);
    printf("Connecter au serveur de jeu en tant que %s \n", nom_bot_moi);

    printf("Attente d'une partie %s...\n", type_partie_choisi[BOT_BOUGE_PAS]);
    // waitForLabyrinth(type_partie_choisi[BOT_BOUGE_PAS], labyrinthName, &sizeX, &sizeY);
    // printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", labyrinthName, sizeX, sizeY);

    waitForLabyrinth(type_partie_choisi[BOT_BOUGE_PAS], laby.labyrinthName, &laby.sizeX, &laby.sizeY);
    printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", laby.labyrinthName, laby.sizeX, laby.sizeY);

    // taille_buffer = (sizeX * sizeY + 5) * 11;
    taille_buffer = (laby.sizeX * laby.sizeY + 5) * 11;

    // labyData = malloc(taille_buffer * sizeof(char));
    // numero_joueur = getLabyrinth(labyData);
    laby.labyData = malloc(taille_buffer * sizeof(char));
    numero_joueur = getLabyrinth(laby.labyData);

    printf("Données recu du serveur sont numéro du joueur : %d\n", numero_joueur);
    printf("Les data du labyrinthe : \n%s\n\n", laby.labyData);
    initAffichage(laby.sizeX, laby.sizeY);

    t_return_code resultat = NORMAL_MOVE;
    int c_mon_tour_jouer = numero_joueur;
    // char coup_envoi[MAX_GET_MOVE];
    // char coup_recu[MAX_GET_MOVE];
    char message_serveur[MAX_MESSAGE];

    // int p0_x = 0, p0_y = 0;
    yek.x = 0;
    yek.y = 0;

    // int p1_x = sizeX - 1, p1_y = sizeY - 1;
    adversaire.x = laby.sizeX - 1;
    adversaire.y = laby.sizeY - 1;

    while (resultat == NORMAL_MOVE)
    {
        printf("\nAffichage du labyrinthe :\n");
        printLabyrinth();

        afficheLabyrinthe(laby.labyData, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);

        if (c_mon_tour_jouer == 0)
        {
            // int type, indice, rot, x, y;
            printf("Entre coup chef : ");
            scanf("%d %d %d %d %d", &yek.type_insertion, &yek.indice, &yek.rotation, &yek.x, &yek.y);
            // p0_x = x;
            // p0_y = y;
            // sprintf(coup_envoi, "%d %d %d %d %d", type, indice, rot, x, y);
            sprintf(yek.coup_envoi, "%d %d %d %d %d", yek.type_insertion, yek.indice, yek.rotation, yek.x, yek.y);

            resultat = sendMove(yek.coup_envoi, message_serveur);

            c_mon_tour_jouer = 1;
        }
        else
        {
            printf("Attente du coup de l'adversaire\n");

            // On récupère le coup de l'adversaire
            resultat = getMove(adversaire.coup_recu, message_serveur);

            printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

            c_mon_tour_jouer = 0;

            // int type_adversaire, indide_adversaire, rotation__adversaire, ax_adversaire, ay_adversaire;
            // sscanf(coup_recu, "%d %d %d %d %d", &type_adversaire, &indide_adversaire, &rotation__adversaire, &ax_adversaire, &ay_adversaire);
            sscanf(adversaire.coup_recu, "%d %d %d %d %d", &adversaire.type_insertion, &adversaire.indice, &adversaire.rotation, &adversaire.x, &adversaire.y);

            // // On met à jour les coordonnées de l'adversaire
            // p1_x = ax_adversaire;
            // p1_y = ay_adversaire;

            printf("L'adversaire a joué vers : type : %d indice : %d rotation: %d x : %d y : %d\n", adversaire.type_insertion, adversaire.indice, adversaire.rotation, adversaire.x, adversaire.y);
        }
    }

    // Affichage de la raison de la fin (gagné ou autre)
    printf("Fin de la partie ! Raison : %s\n", message_serveur);

    free(laby.labyData);
    closeConnection();
    return 0;
}