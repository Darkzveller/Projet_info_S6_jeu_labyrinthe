#ifndef Variable_H
#define Variable_H


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
#endif
