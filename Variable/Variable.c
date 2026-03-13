#include <stdio.h>
#include <stdlib.h>
#include "Variable.h"
// Information du Serveur
const char *nom_serveur = "vps-1a2cee88.vps.ovh.net";
const int port_serveur = 1234;
char *nom_bot_moi = "YEK";



// Information du labyrinthe
char *type_partie_choisi[4] = {"TRAINING DONTMOVE", "TRAINING RANDOM","TRAINING BASIC",0};
char labyrinthName[50]; 
int sizeX = 0;
int sizeY = 0;
