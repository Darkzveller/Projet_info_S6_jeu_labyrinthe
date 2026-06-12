/*******************************************************
 * AFFICHAGE.C
 *
 * DESCRIPTION :
 *   Ce fichier était initialement prévu pour gérer un
 *   affichage graphique du labyrinthe.
 *
 * CONTEXTE :
 *   J'avais commencé à utiliser la bibliothèque X11 afin
 *   de créer une interface graphique personnalisée, car
 *   l'affichage dans le terminal (scroll, défilement,
 *   manque de lisibilité) ne me convenait pas.
 *
 * PROBLEME RENCONTRE :
 *   La mise en place de X11 et la gestion de l'affichage
 *   graphique ont pris trop de temps par rapport au reste
 *   du projet (logique du bot et communication serveur).
 *
 * DECISION :
 *   J'ai donc désactivé et vidé ces fonctions pour me
 *   concentrer sur le développement de mon bot et du gameplay.
 *
 * REMARQUE :
 *   Les fonctions restent présentes pour conserver mais leurs contenus vidé
 *   l'architecture du projet, mais ne font rien.
 *******************************************************/
#include "affichage.h"
#include <stdio.h>

int initAffichage(int TAILLE_X, int TAILLE_Y)
{
    return 1;
}

int afficheLabyrinthe(char *labyData, int tempo, int TAILLE_X, int TAILLE_Y,
                      int x1, int y1, int x2, int y2)
{
    return 1;
}