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
void transfer_labydata_to_laby_update(t_laby *laby);
void print_laby(t_laby *laby, bool activate);
void update_laby(t_laby *laby, t_joueur *adversaire);


#endif
