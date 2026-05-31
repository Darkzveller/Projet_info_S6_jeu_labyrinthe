#include "Variable.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"


#ifndef Strategie_H
#define Strategie_H
//Fonctionnez
void position_tresor(t_laby *laby, t_tuiles *tuiles, int mon_numero_joueur);
void transfer_labydata_to_laby_update(t_laby *laby);
void print_laby(t_laby *laby, bool activate);
void update_laby(t_laby *laby, t_joueur *adversaire);
void update_labyV2(t_laby *laby, t_joueur *joueur_qui_joue, t_joueur *yek);
void copie_laby(t_laby *laby);
bool voisin_accessible(t_laby *laby, int x, int y, int dir, int *nx, int *ny);
int phaseExpansion(t_laby *laby, t_joueur *yek, int target_x, int target_y);
void localiser_tresors(t_laby *laby_state, int *tresor_x, int *tresor_y, int mon_numero_joueur);

int phaseRemontee(t_laby *laby, t_joueur *yek, int target_x, int target_y, int *chemin, int max_chemin);
void essaie_insertion(t_laby *laby, int type_insertion, int indice, int rotation);
void simulate_chemin_court(t_joueur *joueur_actuel, int interdit_type, int interdit_indice);
#endif
