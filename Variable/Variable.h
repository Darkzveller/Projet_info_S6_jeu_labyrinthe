#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clientAPI.h"
#include "labyrinthAPI.h"

#ifndef Variable_H
#define Variable_H

#define ACTIVE_AFFICHAGE_LABY 1

// Information du serveur
extern const char *nom_serveur;
extern const int port_serveur;
extern char *nom_bot_moi;


// Information du labyrinthe
#define BOT_BOUGE_PAS 0
#define BOT_ALEATOIRE 1
#define BOT_BASIC     2
extern char *type_partie_choisi[4];
// extern char labyrinthName[50]; 
// extern int sizeX ;
// extern int sizeY ;

// Données en temps réels du labyrinthe
// extern char *labyData; 
extern int numero_joueur ; 
extern int taille_buffer ;

// Structure de donnees des joueurs
typedef struct
{
    int x;
    int y;
    int type_insertion;
    int indice;
    int rotation;
    char coup_envoi[MAX_GET_MOVE];
    char coup_recu[MAX_GET_MOVE];
} t_joueur;

extern t_joueur yek;
extern t_joueur adversaire ;
#endif
