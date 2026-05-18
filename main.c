#include "Variable.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"
#include "affichage.h"
#include "Mouvement.h"
#define INSERT_LIGNE_GAUCHE 0
#define INSERT_LIGNE_DROITE INSERT_LIGNE_GAUCHE + 1
#define INSERT_COLONNE_GAUCHE INSERT_LIGNE_DROITE + 1
#define INSERT_COLONNE_DROITE INSERT_COLONNE_GAUCHE + 1

#define ROTATION_TUILE 0
#define INSERTION_A_EFFECTUER 1

void update_laby(t_laby *laby, t_joueur *adversaire)
{
    int var_temp = 0;
    if (adversaire->rotation != 0)
    {
        int nord = (laby->laby_update[laby->sizeX - 1][laby->sizeY - 1] >> SHIFT_BIT_NORD) & 1;
        int est = (laby->laby_update[laby->sizeX - 1][laby->sizeY - 1] >> SHIFT_BIT_EST) & 1;
        int sud = (laby->laby_update[laby->sizeX - 1][laby->sizeY - 1] >> SHIFT_BIT_SUD) & 1;
        int ouest = (laby->laby_update[laby->sizeX - 1][laby->sizeY - 1] >> SHIFT_BIT_OUEST) & 1;
        int item = (laby->laby_update[laby->sizeX - 1][laby->sizeY - 1] >> 0) & 0xFF;

        for (int i = 0; i <= adversaire->rotation; i++)
        {
            int nord_old = nord;
            int est_old = est;
            int sud_old = sud;
            int ouest_old = ouest;

            nord = ouest_old;
            est = nord_old;
            sud = est_old;
            ouest = sud_old;
        }
        laby->laby_update[laby->sizeX - 1][laby->sizeY - 1] =
            (nord << SHIFT_BIT_NORD) |
            (est << SHIFT_BIT_EST) |
            (sud << SHIFT_BIT_SUD) |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
    }

    if (adversaire->type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y = adversaire->indice;

        // sauvegarde tuile externe (droite)
        int temp = laby->laby_update[y][laby->sizeX - 1];

        // décalage droite -> gauche
        for (int x = laby->sizeX - 1; x > 0; x--)
        {
            laby->laby_update[y][x] = laby->laby_update[y][x - 1];
        }

        // insertion à gauche
        laby->laby_update[y][0] = temp;
    }

    else if (adversaire->type_insertion == INSERT_LIGNE_DROITE)
    {
        int y = adversaire->indice;

        // sauvegarde tuile externe (gauche)
        int temp = laby->laby_update[y][0];

        // décalage gauche -> droite
        for (int x = 0; x < laby->sizeX - 1; x++)
        {
            laby->laby_update[y][x] = laby->laby_update[y][x + 1];
        }

        // insertion à droite
        laby->laby_update[y][laby->sizeX - 1] = temp;
    }

    else if (adversaire->type_insertion == INSERT_COLONNE_GAUCHE)
    {
        int x = adversaire->indice;

        // sauvegarde tuile externe (bas)
        int temp = laby->laby_update[laby->sizeY - 1][x];

        // décalage bas -> haut
        for (int y = laby->sizeY - 1; y > 0; y--)
        {
            laby->laby_update[y][x] = laby->laby_update[y - 1][x];
        }

        // insertion en haut
        laby->laby_update[0][x] = temp;
    }

    else if (adversaire->type_insertion == INSERT_COLONNE_DROITE)
    {
        int x = adversaire->indice;

        // sauvegarde tuile externe (haut)
        int temp = laby->laby_update[0][x];

        // décalage haut -> bas
        for (int y = 0; y < laby->sizeY - 1; y++)
        {
            laby->laby_update[y][x] = laby->laby_update[y + 1][x];
        }

        // insertion en bas
        laby->laby_update[laby->sizeY - 1][x] = temp;
    }
    /*
        if (adversaire->type_insertion == INSERT_LIGNE_GAUCHE)
        {
            // Stocke la tuiles externe
            var_temp = laby->laby_update[adversaire->indice][laby->sizeY - 1];
            // décalage droite -> gauche
            for (int i = laby->sizeX - 2; i >= 0; i--)
            {
                laby->laby_update[adversaire->indice][i + 1] = laby->laby_update[adversaire->indice][i];
            }

            // insertion de la tuile externe
            laby->laby_update[adversaire->indice][0] = var_temp;

            // mise à jour de la tuile externe
            laby->laby_update[adversaire->indice][laby->sizeY - 1] = laby->laby_update[adversaire->indice][laby->sizeX - 1];
        }
        else if (adversaire->type_insertion == INSERT_LIGNE_DROITE)
        {
            // Stocke la tuiles externe
            var_temp = laby->laby_update[adversaire->indice][0];
            // décalage gauche -> droite
            for (int i = laby->sizeX - 2; i >= 0; i++)
            {
                laby->laby_update[adversaire->indice][i + 1] = laby->laby_update[adversaire->indice][i];
            }

            // insertion de la tuile externe
            laby->laby_update[adversaire->indice][laby->sizeY - 1] = var_temp;

            // mise à jour de la tuile externe
            laby->laby_update[adversaire->indice][laby->sizeY - 1] = laby->laby_update[adversaire->indice][0];
        }
        else if (adversaire->type_insertion == INSERT_COLONNE_GAUCHE)
        {
            // Stocke la tuiles externe
            var_temp = laby->laby_update[laby->sizeX - 1][adversaire->indice];
            // décalage bas -> haut
            for (int i = laby->sizeY - 2; i >= 0; i--)
            {
                laby->laby_update[i + 1][adversaire->indice] = laby->laby_update[i][adversaire->indice];
            }

            // insertion de la tuile externe
            laby->laby_update[0][adversaire->indice] = var_temp;

            // mise à jour de la tuile externe
            laby->laby_update[laby->sizeX - 1][adversaire->indice] = laby->laby_update[laby->sizeX - 1][adversaire->indice];
        }
        else if (adversaire->type_insertion == INSERT_COLONNE_DROITE)
        {
            // Stocke la tuiles externe
            var_temp = laby->laby_update[0][adversaire->indice];
            // décalage haut -> bas
            for (int i = laby->sizeY - 2; i >= 0; i++)
            {
                laby->laby_update[i + 1][adversaire->indice] = laby->laby_update[i][adversaire->indice];
            }

            // insertion de la tuile externe
            laby->laby_update[0][adversaire->indice] = var_temp;

            // mise à jour de la tuile externe
            laby->laby_update[laby->sizeX - 1][adversaire->indice] = laby->laby_update[laby->sizeX - 1][adversaire->indice];
        }*/
}

int main()
{
    printf("caca\n");
#if DEBUG_CONNECT_SERV
    printf("Tentative de connexion au serveur \n");
#endif
    connectToServer(nom_serveur, port_serveur, nom_bot_moi);
#if DEBUG_CONNECT_SERV
    printf("Connecter au serveur de jeu en tant que %s \n", nom_bot_moi);
    printf("Choix partie %s\n", type_partie_choisi[BOT_BOUGE_PAS]);
#endif

    waitForLabyrinth(type_partie_choisi[BOT_BOUGE_PAS], laby.labyrinthName, &laby.sizeX, &laby.sizeY);

#if DEBUG_DATA_STRUCT_LABY
    printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", laby.labyrinthName, laby.sizeX, laby.sizeY);
#endif

    taille_buffer = (laby.sizeX * laby.sizeY + 5) * 11;
    laby.labyData = malloc(taille_buffer * sizeof(char));
    laby.tour_joueur = getLabyrinth(laby.labyData);
    transfer_labydata_to_laby_update(&laby, true);
    print_laby(&laby);

#if DEBUG_DATA_STRUCT_LABY
    printf("Données recu du serveur, mon numéro du joueur : %d\n", laby.tour_joueur);
    printf("Les data du labyrinthe : \n%s\n\n", laby.labyData);
#endif

#if ACTIVE_AFFICHAGE_LABY
    initAffichage(laby.sizeX, laby.sizeY);
#endif

    t_return_code resultat_move = NORMAL_MOVE;

    yek.x = 0;
    yek.y = 0;
    adversaire.x = laby.sizeX - 1;
    adversaire.y = laby.sizeY - 1;

    while (resultat_move == NORMAL_MOVE)
    {
        printf("\nAffichage du labyrinthe :\n");
        printLabyrinth();
        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);
        // printf("labydata \n%d\n", laby.labyData);
        // printf("laby_update \n%d\n", laby.laby_update);

        position_tresor(&laby, &tuiles_tresor);

        if (laby.tour_joueur == 0)
        {
                                update_laby(&laby, &yek);

            printf("Entre coup chef : ");
            scanf("%d %d %d %d %d", &yek.type_insertion, &yek.indice, &yek.rotation, &yek.x, &yek.y);
            sprintf(yek.coup_envoi, "%d %d %d %d %d", yek.type_insertion, yek.indice, yek.rotation, yek.x, yek.y);

            resultat_move = sendMove(yek.coup_envoi, laby.message_serveur);
            laby.tour_joueur = 1;
        }
        else
        {
                    update_laby(&laby, &adversaire);

            printf("Attente du coup de l'adversaire\n");

            // On récupère le coup de l'adversaire
            resultat_move = getMove(adversaire.coup_recu, laby.message_serveur);
            printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

            laby.tour_joueur = 0;
            // On met à jour les coordonnées de l'adversaire
            sscanf(adversaire.coup_recu, "%d %d %d %d %d", &adversaire.type_insertion, &adversaire.indice, &adversaire.rotation, &adversaire.x, &adversaire.y);

            printf("L'adversaire a joué vers : type : %d indice : %d rotation: %d x : %d y : %d\n", adversaire.type_insertion, adversaire.indice, adversaire.rotation, adversaire.x, adversaire.y);
        }
        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);

        print_laby(&laby);
    }

    // Affichage de la raison de la fin (gagné ou autre)
    printf("Fin de la partie ! Raison : %s\n", laby.message_serveur);

    free(laby.labyData);
    closeConnection();
    return 0;
}
