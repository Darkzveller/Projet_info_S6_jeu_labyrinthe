#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__

int initAffichage(int TAILLE_X, int TAILLE_Y);

/* Ajout des coordonnées des joueurs : x1, y1 (Joueur 0) et x2, y2 (Joueur 1) */
int afficheLabyrinthe(char* labyData, int tempo, int TAILLE_X, int TAILLE_Y, 
                      int x1, int y1, int x2, int y2);

#endif