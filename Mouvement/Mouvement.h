#include "Variable.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"


#ifndef Strategie_H
#define Strategie_H

void position_tresor(t_laby *laby, t_tuiles *tuiles);
int phaseExpansion(t_laby *laby, int coord_x_depart, int coord_y_depart, int coord_x_arrivee, int coord_y_arrivee);
bool est_passage_possible(t_laby *laby, int x, int y, int nx, int ny, int dir);
 void coord_case_voisine(int coord_x_actu, int coord_y_actu, int direction_voulue, int *coord_x_voisine, int *coord_y_voisine);
int phaseRemontee(int TX, int TY, int tab[TX][TY], int x_arr, int y_arr, t_coord *chemin);

#endif
