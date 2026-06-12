/*******************************************************
 * VARIABLE.H - DEFINITIONS GLOBALES DU PROJET
 *
 * OBJECTIF :
 *   Centraliser toutes les définitions, structures et
 *   variables globales utilisées par le bot :
 *   - communication serveur
 *   - représentation du labyrinthe
 *   - joueurs
 *   - simulation et IA
 *
 * REMARQUE IMPORTANTE :
 *   Ce fichier est inclus partout dans le projet.
 *   Toute modification impacte l'ensemble du programme.
 *******************************************************/


/*******************************************************
 * INCLUDES SYSTEME ET PROJET
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clientAPI.h"
#include "labyrinthAPI.h"

#ifndef Variable_H
#define Variable_H

/*******************************************************
 * FLAGS DE DEBUG
 *
 * DESCRIPTION :
 *   Permettent d’activer ou désactiver certains affichages
 *   pour faciliter le débogage du programme.
 * Remarques : 
 *   J'ai oublié d'inclure ceux des fonctions de simulation
 *******************************************************/
#define DEBUG_CONNECT_SERV 0
#define DEBUG_DATA_STRUCT_LABY 0
#define ACTIVE_AFFICHAGE_LABY 0
#define DEBUG_POS_TUILES 1

/*******************************************************
 * INFORMATIONS SERVEUR
 *
 * DESCRIPTION :
 *   Paramètres de connexion au serveur de jeu.
 *******************************************************/
/* Adresse du serveur de jeu */
extern const char *nom_serveur;
/* Port de connexion */
extern const int port_serveur;
/* Nom du bot utilisé pour la connexion */
extern char *nom_bot_moi;
/*******************************************************
 * MODES DE PARTIE DISPONIBLES
 *
 * DESCRIPTION :
 *   Liste des configurations possibles envoyées au serveur
 *   pour lancer une partie (dontmove, regular etc.).
 *******************************************************/
#define BOT_BOUGE_PAS 0
#define BOT_ALEATOIRE 1
#define BOT_BASIC 2
#define BOT_REGULAR 2
extern char *type_partie_choisi[4];

/*******************************************************
 * TYPES D’INSERTION DE TUILES
 *
 * DESCRIPTION :
 *   Définit les actions possibles sur le labyrinthe :
 *   - lignes gauche/droite
 *   - colonnes haut/bas
 *******************************************************/
#define INSERT_LIGNE_GAUCHE 0
#define INSERT_LIGNE_DROITE INSERT_LIGNE_GAUCHE + 1
#define INSERT_COLONNE_HAUT INSERT_LIGNE_DROITE + 1
#define INSERT_COLONNE_BAS INSERT_COLONNE_HAUT + 1

/*******************************************************
 * DIRECTIONS ET MURS
 *
 * DESCRIPTION :
 *   Représentation des directions et des murs
 *   sous forme de bits pour la grille.
 * Remarquer : 
 *  J'ai mélanger du code du dernier semestre et celui que j'ai fait pour ce projet
 *******************************************************/
#define NORD 0
#define EST 1
#define SUD 2
#define OUEST 3
#define MUR_NORD (1 << SHIFT_BIT_NORD)
#define MUR_EST (1 << SHIFT_BIT_EST)
#define MUR_SUD (1 << SHIFT_BIT_SUD)
#define MUR_OUEST (1 << SHIFT_BIT_OUEST)



/*******************************************************
 * ENCODAGE DES TUILES (BITMASK)
 *
 * DESCRIPTION :
 *   Chaque case du labyrinthe est encodée sur 16 bits :
 *
 *   [ N E S O ] [ ITEM ]
 *     4 bits      8 bits
 *
 *   - 4 bits pour les murs
 *   - 8 bits pour l'identifiant du trésor
 * 
 *  Chaque mur vaut 0 ou 1
*   ITEM peut aller de 0 à 256

 *******************************************************/
/* Positions des bits pour les murs */
#define SHIFT_BIT_OUEST 8
#define SHIFT_BIT_SUD (SHIFT_BIT_OUEST + 1)
#define SHIFT_BIT_EST (SHIFT_BIT_SUD + 1)
#define SHIFT_BIT_NORD (SHIFT_BIT_EST + 1)
/* Masque pour extraire l’item */
#define MASK_ITEM 0xFF
// Mon buffer
extern int taille_buffer;

/*******************************************************
 * STRUCTURE JOUEUR
 *
 * DESCRIPTION :
 *   Représente un joueur 
 *  *******************************************************/
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
/*******************************************************
 * TUILE EXTRA
 *
 * DESCRIPTION :
 *   Représente la tuile hors du plateau,
 *   utilisée lors des insertions.
 *******************************************************/
typedef struct
{
    int presence_mur;

} t_tuiles_extra;
// extern t_tuiles_extra tuiles_extra;

/*******************************************************
 * STRUCTURE LABYRINTHE
 *
 * DESCRIPTION :
 *   Contient toutes les données du plateau :
 *   - grille principale
 *   - copie pour simulation
 *   - tuile extra
 *******************************************************/
typedef struct
{
    char labyrinthName[50];
    int sizeX;
    int sizeY;
    char *labyData;
    char message_serveur[MAX_MESSAGE];
    int tour_joueur;
    int **laby_update;
    int **copy_laby_update;// Permet de tester les essaie insertion depuis un autre tableau

    t_tuiles_extra extra;
    t_tuiles_extra copy_extra;

    int **dist;


} t_laby;

extern t_laby laby;
/*******************************************************
 * STRUCTURE DES TRESORS
 *
 * DESCRIPTION :
 *   Stocke les positions des trésors détectés
 *   ainsi que leur état.
 *
 * NOTE :
 *   Cette structure est partiellement abandonnée
 *   au profit de num_tresor.
 *******************************************************/
#define NBR_TUILES 25
typedef struct
{
    int x[NBR_TUILES];
    int y[NBR_TUILES];
    int presence_mur[NBR_TUILES];
    int num_tresor;
    bool tresor_recuperer[NBR_TUILES];
    t_tuiles_extra tuiles_extra;

} t_tuiles;

extern t_tuiles tuiles_tresor;


/*******************************************************
 * STRUCTURE DE CHEMIN MULTI-OBJECTIF
 *
 * DESCRIPTION :
 *   Stocke le meilleur chemin trouvé lors des simulations.
 *******************************************************/
typedef struct {
    int count;          // number of coordinate pairs
    int x[30];          // x coordinates of the stops
    int y[30];          // y coordinates of the stops
    int total_bonus_dist; // distance from first treasure to last stop
} t_multi_path;

extern t_multi_path meilleur_chemin_complet;
extern int meilleur_treasures_collected;
/*******************************************************
 * COUP INTERDIT
 *
 * DESCRIPTION :
 *   Stocke le coup interdit basé sur le dernier
 *   mouvement de l’adversaire.
 *******************************************************/
extern int coup_interdit_type ;
extern int coup_interdit_indice;

#endif
