#include "Variable.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clientAPI.h"
#include "labyrinthAPI.h"
#include "affichage.h"
#include "Mouvement.h"
#include <time.h>

int nbr_vicoire = 0;
int nbr_defait =  0;
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
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
    while (1)
    {
        // waitForLabyrinth(type_partie_choisi[(BOT_ALEATOIRE)], laby.labyrinthName, &laby.sizeX, &laby.sizeY);
        // waitForLabyrinth(type_partie_choisi[(BOT_BASIC)], laby.labyrinthName, &laby.sizeX, &laby.sizeY);
        // waitForLabyrinth("TRAINING BASIC", laby.labyrinthName, &laby.sizeX, &laby.sizeY);
        // waitForLabyrinth("TRAINING REGULAR", laby.labyrinthName, &laby.sizeX, &laby.sizeY);
        // waitForLabyrinth("", laby.labyrinthName, &laby.sizeX, &laby.sizeY);
        waitForLabyrinth("TOURNAMENT EI3", laby.labyrinthName, &laby.sizeX, &laby.sizeY);

        // printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", laby.labyrinthName, laby.sizeX, laby.sizeY);

        taille_buffer = (laby.sizeX * laby.sizeY + 5) * 11;
        laby.labyData = malloc(taille_buffer * sizeof(char));
        laby.tour_joueur = getLabyrinth(laby.labyData);
        int mon_numero_joueur = laby.tour_joueur;
        transfer_labydata_to_laby_update(&laby);
        print_laby(&laby, false);

#if DEBUG_DATA_STRUCT_LABY
        printf("Données recu du serveur, mon numéro du joueur : %d\n", laby.tour_joueur);
        printf("Les data du labyrinthe : \n%s\n\n", laby.labyData);
#endif

#if ACTIVE_AFFICHAGE_LABY
        initAffichage(laby.sizeX, laby.sizeY);
#endif

        t_return_code resultat_move = NORMAL_MOVE;
        tuiles_tresor.num_tresor = 1; // On commence la partie au trésor 1
        if (mon_numero_joueur == 0)
        {
            yek.x = 0;
            yek.y = 0;
            adversaire.x = laby.sizeX - 1;
            adversaire.y = laby.sizeY - 1;
        }
        else
        {
            yek.x = laby.sizeX - 1;
            yek.y = laby.sizeY - 1;
            adversaire.x = 0;
            adversaire.y = 0;
        }

        int tour_actuel = 0;

        while (resultat_move == NORMAL_MOVE)
        {
            // printf("\nAffichage du labyrinthe :\n");
            // delay(1);
            // printLabyrinth();
            // delay(1);

            // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);
            // printf("labydata \n%d\n", laby.labyData);
            // printf("laby_update \n%d\n", laby.laby_update);

            position_tresor(&laby, &tuiles_tresor, mon_numero_joueur);

            if (tour_actuel == mon_numero_joueur)
            {

                // simulate_chemin_court(&yek, coup_interdit_type, coup_interdit_indice);

                // update_labyV2(&laby, &yek, &yek);
                // printf("Entre coup chef : ");
                // // scanf("%d %d %d %d %d", &yek.type_insertion, &yek.indice, &yek.rotation, &yek.x, &yek.y);
                // sprintf(yek.coup_envoi, "%d %d %d %d %d",
                //         yek.type_insertion,
                //         yek.indice,
                //         yek.rotation,
                //         yek.x,
                //         yek.y);

                // resultat_move = sendMove(yek.coup_envoi, laby.message_serveur);
                // laby.tour_joueur = 1;

                // Sauvegarder la position actuelle du joueur avant que simulate_chemin_court ne l'écrase
                int currentX = yek.x;
                int currentY = yek.y;

                simulate_chemin_court(&yek, coup_interdit_type, coup_interdit_indice);

                // Sauvegarder la destination calculée par la simulation
                int targetX = yek.x;
                int targetY = yek.y;

                // Restaurer la position actuelle pour que update_labyV2 calcule correctement le push
                yek.x = currentX;
                yek.y = currentY;

                update_labyV2(&laby, &yek, &yek);

                // Construction du coup avec les étapes multiples !
                int len_env = sprintf(yek.coup_envoi, "%d %d %d",
                                      yek.type_insertion,
                                      yek.indice,
                                      yek.rotation);
                for (int i = 0; i < meilleur_chemin_complet.count; i++)
                {
                    len_env += sprintf(yek.coup_envoi + len_env, " %d %d", meilleur_chemin_complet.x[i], meilleur_chemin_complet.y[i]);
                }

                // printf("Entre coup chef : ");

                resultat_move = sendMove(yek.coup_envoi, laby.message_serveur);

                // Mettre à jour la position avec la vraie destination finale
                yek.x = targetX;
                yek.y = targetY;

                // Mettre à jour le numéro de trésor visé
                tuiles_tresor.num_tresor += meilleur_treasures_collected;

                // laby.tour_joueur = 1;
                tour_actuel = (mon_numero_joueur == 0) ? 1 : 0;
            }
            else
            {

                // printf("Attente du coup de l'adversaire\n");

                // On récupère le coup de l'adversaire
                resultat_move = getMove(adversaire.coup_recu, laby.message_serveur);
                // printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

                // laby.tour_joueur = 0;
                tour_actuel = mon_numero_joueur;
                // On met à jour les coordonnées de l'adversaire en tenant compte des étapes multiples
                int type_ins, ind_ins, rot_ins;
                int offset = 0;
                if (sscanf(adversaire.coup_recu, "%d %d %d%n", &type_ins, &ind_ins, &rot_ins, &offset) >= 3)
                {
                    adversaire.type_insertion = type_ins;
                    adversaire.indice = ind_ins;
                    adversaire.rotation = rot_ins;
                    char *ptr = adversaire.coup_recu + offset;
                    int tx, ty;
                    int read_bytes;
                    while (sscanf(ptr, "%d %d%n", &tx, &ty, &read_bytes) == 2)
                    {
                        adversaire.x = tx;
                        adversaire.y = ty;
                        ptr += read_bytes;
                    }
                }
                update_labyV2(&laby, &adversaire, &yek);

                determiner_coup_interdit(adversaire.type_insertion, adversaire.indice, &coup_interdit_type, &coup_interdit_indice);
            }
            // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);

            print_laby(&laby, false);
        }

        // if (resultat_move == WINNING_MOVE)
        // {

        //     printf("vous avez gagner \n");
        //     // while (1)
        //     //     ;
        // }else if(resultat_move == LOSING_MOVE){
        //     printf("Vous avez perdu \n");
        // }
        // // Affichage de la raison de la fin (gagné ou autre)
        // printf("Fin de la partie ! Raison : %s\n", laby.message_serveur);
        bool dernier_joueur_actif = (tour_actuel == mon_numero_joueur) ? ((mon_numero_joueur == 0) ? 1 : 0) : mon_numero_joueur;

        if (resultat_move == WINNING_MOVE)
        {
            if (dernier_joueur_actif == mon_numero_joueur)
            {
                printf("Félicitations ! Vous avez GAGNÉ la partie ! 🎉\n");
                // printf("Nombre de victoire actuelle est de %d \n", nbr_vicoire++);
                nbr_vicoire+=1;

            }
            else
            {
                printf("Dommage... L'adversaire a trouvé son dernier trésor et a GAGNÉ. 😞\n");
                // printf("Nombre de defaite actuelle est de %d \n", nbr_defait++);
                nbr_defait+=1;
            }
        }
        else if (resultat_move == LOSING_MOVE)
        {
            if (dernier_joueur_actif == mon_numero_joueur)
            {
                // printf("Vous avez PERDU... Vous avez joué un coup invalide ou fait un timeout. ❌\n");
            }
            else
            {
                // printf("Victoire par forfait ! L'adversaire a commis une erreur et a PERDU. 🏆\n");
            }
        }
        else
        {
            printf("Fin de partie inconnue ou interruption serveur.\n");
        }
        printf("Resultat : %d victoire et %d defaite \n",nbr_vicoire,nbr_defait);
        int size = ((laby.sizeX * laby.sizeY + 5) * 11);

        for (int i = 0; i < size; i++)
        {
            free(laby.laby_update[i]);
            free(laby.copy_laby_update[i]);
        }

        free(laby.laby_update);
        free(laby.copy_laby_update);

        laby.laby_update = NULL;
        laby.copy_laby_update = NULL;

        free(laby.labyData);
    // Affichage de la raison officielle envoyée par le serveur
    printf("Raison du serveur : %s\n", laby.message_serveur);
    }
    closeConnection();
    delay(5000);

    return 0;
}
