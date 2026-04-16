#include "Variable.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"


#ifndef Strategie_H
#define Strategie_H
//Fonctionne
void position_tresor(t_laby *laby, t_tuiles *tuiles);
void transfer_labydata_to_laby_update(t_laby *laby, bool activate_print);


// A tester
int phaseExpansion(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor);
void coord_case_voisine(int coord_x_actu, int coord_y_actu, int direction_voulue, int *coord_x_voisine, int *coord_y_voisine);
int phaseRemontee(t_laby *laby, t_tuiles *tuiles_tresor, t_joueur *yek);

#endif
