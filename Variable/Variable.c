#include "Variable.h"

// Information du Serveur
const char *nom_serveur = "vps-1a2cee88.vps.ovh.net";
const int port_serveur = 1234;
char *nom_bot_moi = "Pepe";

// Information du labyrinthe
char *type_partie_choisi[4] = {"TRAINING DONTMOVE display=debug timeout=10000 seed=1", "TRAINING RANDOM timeout=10000 seed=0 start=0", "TRAINING BASIC timeout=10000 seed=0 start=0", 0};

// Données en temps réels du labyrinthe
int taille_buffer = 0;

// Structure de donnees des joueurs
t_joueur yek = {0};
t_joueur adversaire = {0};

// Structure de donnees du labyrinthe
t_laby laby = {0};

t_tuiles tuiles_tresor= {0};
