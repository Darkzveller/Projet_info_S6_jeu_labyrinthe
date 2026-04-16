#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clientAPI.h"
#include "labyrinthAPI.h"

#ifndef Variable_H
#define Variable_H

// Define permettant d'activer ou desactiver les prints
#define DEBUG_CONNECT_SERV 0
#define DEBUG_DATA_STRUCT_LABY 0
#define ACTIVE_AFFICHAGE_LABY 1
#define DEBUG_POS_TUILES 0

// Information du serveur
extern const char *nom_serveur;
extern const int port_serveur;
extern char *nom_bot_moi;

#define BOT_BOUGE_PAS 0
#define BOT_ALEATOIRE 1
#define BOT_BASIC 2
extern char *type_partie_choisi[4];

/*
 On utilise 16 bits au total :
 - 4 bits pour les murs (Nord, Est, Sud, Ouest)
 - 12 bits pour l'identifiant de l'item (trésor)

Structure des bits :
[ N E S O ] [     ITEM     ]
  4 bits        8 bits

 Chaque mur vaut 0 ou 1
 ITEM peut aller de 0 à 256
*/

#define SHIFT_BIT_OUEST  8
#define SHIFT_BIT_SUD    SHIFT_BIT_OUEST+1
#define SHIFT_BIT_EST    SHIFT_BIT_SUD+1
#define SHIFT_BIT_NORD   SHIFT_BIT_EST+1
#define MASK_ITEM       0xFF
// Données en temps réels du labyrinthe
extern int taille_buffer;

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
extern t_joueur adversaire;

// Structure de donnees du labyrinthe
typedef struct
{
    char labyrinthName[50];
    int sizeX;
    int sizeY;
    char *labyData;
    char message_serveur[MAX_MESSAGE];
    int tour_joueur;
    int **laby_update;
    
    
    
} t_laby;

extern t_laby laby;
// Structure de donnees des tuile
#define NBR_TUILES 25

typedef struct
{
    int x[NBR_TUILES];
    int y[NBR_TUILES];
    int presence_mur[NBR_TUILES];
    int num_tresor;
    int tresor_recuperer[NBR_TUILES];

} t_tuiles;

extern t_tuiles tuiles_tresor;

typedef struct
{
    int x;
    int y;
} t_coord;
extern t_coord chemin_a_dessiner[500];

#endif
