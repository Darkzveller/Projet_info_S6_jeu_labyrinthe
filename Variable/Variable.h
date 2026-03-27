#ifndef Variable_H
#define Variable_H

#define ACTIVE_AFFICHAGE_LABY 1

extern const char *nom_serveur;
extern const int port_serveur;
extern char *nom_bot_moi;


// Information du labyrinthe
#define BOT_BOUGE_PAS 0
#define BOT_ALEATOIRE 1
#define BOT_BASIC     2
extern char *type_partie_choisi[4];
extern char labyrinthName[50]; 
extern int sizeX ;
extern int sizeY ;

// Données en temps réels du labyrinthe
extern char *labyData; 
extern int numero_joueur ; 
extern int taille_buffer ;

#endif
