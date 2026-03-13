#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__


/* Définition des constantes de labyrinthe */
/* Modifié pour correspondre au plateau de jeu Labyrinthe typique */

/* Fonction 'initAffichage'
But: Permet d'initialiser tout l'affichage X11, de créer la fenêtre, etc...
Retour: renvoit 1 si tout c'est bien passé, et 0 en cas d'erreur */
int initAffichage(int TAILLE_X, int TAILLE_Y);


/* Fonction afficheLabyrinthe
But: Afficher le labyrinthe et gèrer tous les évènements graphiques (déplacement
de fenêtre, touches pressées, etc...). Après l'affichage du labyrinthe, la
fonction attend un certain temps (indiqué par tempo) avant de se terminer. Cela
permet de par exemple d'afficher pas-à-pas l'évolution du labyrinthe, ou de
faire une pause pendant l'affichage (en attente que la touche 'espace' soit
pressée
Paramètres:
 - labyData : la chaîne de caractères brute reçue par l'API (contenant les murs et items)
 - tempo : temporisation en ms. Mettre à zéro pour que cette fonction soit 
 bloquante et attend que l'utilisateur appuie sur la touche 'espace' pour
 continuer
Retour: renvoie une valeur dépendant de la touche pressée par l'utilisateur
		0 si aucune touche n'a été pressée
		1 si l'utilisateur a pressé la touche 'espace'
 */
int afficheLabyrinthe( char* labyData, int tempo,int TAILLE_X, int TAILLE_Y);


#endif