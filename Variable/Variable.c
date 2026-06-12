#include "Variable.h"
/*******************************************************
 * VARIABLES GLOBALES - CONFIGURATION DU PROGRAMME
 *
 * OBJECTIF :
 *   Définir les paramètres globaux du programme :
 *   - connexion serveur
 *   - configuration des parties
 *   - structures principales du jeu
 *
 * REMARQUE IMPORTANTE :
 *   - Ces variables sont partagées dans tout le programme (dasn les faits, seulement utiliser dans le main et pas dans les autres codes).
 *   - Elles représentent l’état global du bot.
 * 
 *******************************************************/

/*******************************************************
 * CONFIGURATION SERVEUR
 *******************************************************/
// http://vps-1a2cee88.vps.ovh.net:8899/player/disconect/Pepe
const char *nom_serveur = "vps-1a2cee88.vps.ovh.net";
const int port_serveur = 1234;
char *nom_bot_moi = "Bot_d_EL_KATTOUFI";

/*******************************************************
 * TYPES DE PARTIES DISPONIBLES
 *
 * DESCRIPTION :
 *   Liste des configurations de jeu possibles envoyées
 *   au serveur (mode, seed, debug, timeout, etc.)
 *******************************************************/
char *type_partie_choisi[4] = {"TRAINING DONTMOVE display=debug timeout=10000 seed=1 start=0", "TRAINING RANDOM timeout=10000", "TRAINING BASIC timeout=10000 display=debug seed=0 start=0", "TRAINING REGULAR"};

// Données en temps réels du labyrinthe
int taille_buffer = 0;

/*******************************************************
 * STRUCTURES DES JOUEURS
 *
 * DESCRIPTION :
 *   Stocke les informations du joueur IA (yek)
 *   et de l’adversaire.
 *******************************************************/
t_joueur yek = {0};
t_joueur adversaire = {0};

/*******************************************************
 * STRUCTURE PRINCIPALE DU LABYRINTHE
 *
 * DESCRIPTION :
 *   Contient l’état complet du plateau :
 *   - grille actuelle
 *   - copie de simulation
 *   - tuile extra
 *******************************************************/
t_laby laby = {0};

/*******************************************************
 * DONNEES DES TRESORS
 *
 * DESCRIPTION :
 *   Stocke les positions des trésors détectés
 *   ainsi que leur état d’avancement.
 *******************************************************/
t_tuiles tuiles_tresor = {0};

/*******************************************************
 * RESULTAT DE LA MEILLEURE SIMULATION
 *
 * DESCRIPTION :
 *   Stocke le meilleur chemin trouvé par l’algorithme
 *   de simulation (multi-objectifs).
 *******************************************************/
t_multi_path meilleur_chemin_complet = {0};
int meilleur_treasures_collected = 0;
/*******************************************************
 * COUP INTERDIT
 *
 * DESCRIPTION :
 *   Stocke le coup interdit déterminé à partir
 *   de l’action de l’adversaire.
 *******************************************************/
int coup_interdit_type = -1;
int coup_interdit_indice = -1;

