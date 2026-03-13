#include "Variable.h"
#include <stdio.h>
#include <stdlib.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"

int main()
{
    printf("caca\n");

    printf("Tentative de connexion au serveur \n");
    connectToServer(nom_serveur, port_serveur, nom_bot_moi);
    printf("Connecter au serveur de jeu en tant que %s \n", nom_bot_moi);

    printf("Attente d'une partie %s...\n", type_partie_choisi[BOT_BOUGE_PAS]);
    waitForLabyrinth(type_partie_choisi[BOT_BOUGE_PAS], labyrinthName, &sizeX, &sizeY);
    printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", labyrinthName, sizeX, sizeY);
    
    
    taille_buffer = (sizeX * sizeY + 5) * 11;
    labyData = malloc(taille_buffer * sizeof(char));
    numero_joueur = getLabyrinth(labyData);
    printf("Données recu du serveur sont numéro du joueur : %d\n", numero_joueur);
    printf("Les data du labyrinthe : \n%s\n\n",labyData);



    printf("Affichage du labyrinthe \n");
    printLabyrinth();



    free(labyData);
    closeConnection();
    return 0;
}