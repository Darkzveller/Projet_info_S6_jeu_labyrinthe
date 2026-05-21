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

typedef struct
{
    int x[500];
    int y[500];

} t_case_strat;

int phaseRemontee(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor, int *chemin, int max_chemin)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    // =================================================================
    // 1. AFFICHAGE DU TABLEAU DES DISTANCES (Pour le débuggage)
    // =================================================================
    printf("\n--- MAP DES DISTANCES (laby->copy_laby_update) ---\n");
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            // On affiche un point si la case vaut 0 (inaccessible)
            if (laby->copy_laby_update[x][y] == 0)
            {
                printf("  . ");
            }
            else
            {
                printf("%3d ", laby->copy_laby_update[x][y]);
            }
        }
        printf("\n");
    }
    printf("--------------------------------------------------\n\n");

    // Coordonnées de la destination (le trésor ciblé)
    int destX = tuiles_tresor->x[tuiles_tresor->num_tresor];
    int destY = tuiles_tresor->y[tuiles_tresor->num_tresor];

    printf("[DEBUG] DEPART  (Joueur) : (%d, %d) -> Valeur: %d\n", yek->x, yek->y, laby->copy_laby_update[yek->x][yek->y]);
    printf("[DEBUG] ARRIVEE (Trésor) : (%d, %d) -> Valeur: %d\n", destX, destY, laby->copy_laby_update[destX][destY]);

    // On commence la remontée depuis l'arrivée
    int x = destX;
    int y = destY;
    int idx = 0;

    // Sécurité : si l'arrivée possède une distance de 0, elle est strictement inaccessible
    if (laby->copy_laby_update[x][y] == 0)
    {
        printf("[ERREUR] La destination (%d, %d) vaut 0. Impossible de remonter.\n", x, y);
        return 0;
    }

    // =================================================================
    // 2. BOUCLE DE REMONTEE (De l'arrivée vers le départ)
    // =================================================================
    while (!(x == yek->x && y == yek->y))
    {
        int current_value = laby->copy_laby_update[x][y];
        int found = 0;

        printf("[REMONTEE] Case actuelle: (%d, %d) avec distance = %d\n", x, y, current_value);

        // On cherche un voisin qui possède la valeur (distance actuelle - 1)
        for (int dir = 0; dir < 4; dir++)
        {
            int nx, ny;

            // Vérifie si le voisin est accessible (murs + limites de la grille)
            // voisin_accessible(laby, x, y, dir, &nx, &ny);
            if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
                continue;

            // Si ce voisin valide a bien la valeur inférieure
            if (laby->copy_laby_update[nx][ny] == current_value - 1)
            {
                if (idx < max_chemin)
                {
                    chemin[idx++] = dir; // On enregistre la direction
                }

                // On se déplace sur cette case voisine
                x = nx;
                y = ny;
                found = 1;
                break; // Quitte la boucle des directions pour passer à la case suivante
            }
        }

        // Si aucun voisin avec (current_value - 1) n'a été trouvé : blocage logique
        if (!found)
        {
            printf("[ERREUR REMONTEE] Bloqué sur la case (%d, %d) de valeur %d. Aucun voisin ne vaut %d\n",
                   x, y, current_value, current_value - 1);
            return 0;
        }
    }

    printf("[REMONTEE] Succès ! Arrivé au joueur à la case (%d, %d)\n", x, y);

    // =================================================================
    // 3. INVERSION DU CHEMIN
    // =================================================================
    // Le chemin a été construit à l'envers (Arrivée -> Départ)
    // On l'inverse pour obtenir l'ordre logique (Départ -> Arrivée)
    for (int i = 0; i < idx / 2; i++)
    {
        int tmp = chemin[i];
        chemin[i] = chemin[idx - 1 - i];
        chemin[idx - 1 - i] = tmp;
    }

    return idx; // Renvoie la taille finale du chemin trouvé
}
void essaie_insertion(t_laby *laby, int type_insertion, int indice, int rotation)
{
    // 1. Gestion de la rotation (Ton code est parfait ici)
    if (rotation != 0)
    {
        int nord = (laby->copy_extra.presence_mur >> SHIFT_BIT_NORD) & 1;
        int est = (laby->copy_extra.presence_mur >> SHIFT_BIT_EST) & 1;
        int sud = (laby->copy_extra.presence_mur >> SHIFT_BIT_SUD) & 1;
        int ouest = (laby->copy_extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
        int item = (laby->copy_extra.presence_mur >> 0) & 0xFF;

        for (int i = 0; i < rotation; i++)
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
        laby->copy_extra.presence_mur =
            (nord << SHIFT_BIT_NORD) |
            (est << SHIFT_BIT_EST) |
            (sud << SHIFT_BIT_SUD) |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
    }

    // 2. Décalages et insertions avec le format [x][y]
    if (type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y = indice;

        // Sauvegarde de la tuile expulsée à DROITE (X maximum)
        int temp = laby->copy_laby_update[laby->sizeX - 1][y];

        // Décalage de la gauche vers la droite
        for (int x = laby->sizeX - 1; x > 0; x--)
        {
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x - 1][y];
        }

        // Insertion de la tuile externe à GAUCHE (X = 0)
        laby->copy_laby_update[0][y] = laby->copy_extra.presence_mur;

        laby->copy_extra.presence_mur = temp;
    }
    else if (type_insertion == INSERT_LIGNE_DROITE)
    {
        int y = indice;

        // Sauvegarde de la tuile expulsée à GAUCHE (X = 0)
        int temp = laby->copy_laby_update[0][y];

        // Décalage de la droite vers la gauche
        for (int x = 0; x < laby->sizeX - 1; x++)
        {
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x + 1][y];
        }

        // Insertion de la tuile externe à DROITE (X maximum)
        laby->copy_laby_update[laby->sizeX - 1][y] = laby->copy_extra.presence_mur;

        laby->copy_extra.presence_mur = temp;
    }
    else if (type_insertion == INSERT_COLONNE_HAUT)
    {
        int x = indice;

        // Sauvegarde de la tuile expulsée en BAS (Y maximum)
        int temp = laby->copy_laby_update[x][laby->sizeY - 1];

        // Décalage du haut vers le bas
        for (int y = laby->sizeY - 1; y > 0; y--)
        {
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x][y - 1];
        }

        // Insertion de la tuile externe en HAUT (Y = 0)
        laby->copy_laby_update[x][0] = laby->copy_extra.presence_mur;

        laby->copy_extra.presence_mur = temp;
    }
    else if (type_insertion == INSERT_COLONNE_BAS)
    {
        int x = indice;

        // Sauvegarde de la tuile expulsée en HAUT (Y = 0)
        int temp = laby->copy_laby_update[x][0];

        // Décalage du bas vers le haut
        for (int y = 0; y < laby->sizeY - 1; y++)
        {
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x][y + 1];
        }

        // Insertion de la tuile externe en BAS (Y maximum)
        laby->copy_laby_update[x][laby->sizeY - 1] = laby->copy_extra.presence_mur;

        laby->copy_extra.presence_mur = temp;
    }
}
void simulate_chemin_court(t_joueur *joueur_actuel)
{
    if (joueur_actuel->x == tuiles_tresor.x[tuiles_tresor.num_tresor] && joueur_actuel->y == tuiles_tresor.y[tuiles_tresor.num_tresor])
    {
        printf("[INFO] Déjà sur le trésor en (%d, %d) ! On valide sans bouger.\n", joueur_actuel->x, joueur_actuel->y);

        // On définit un coup par défaut (rester sur place / insertion neutre)
        joueur_actuel->type_insertion = 0;
        joueur_actuel->indice = 1;
        joueur_actuel->rotation = 0;


        // yek->x et yek->y restent inchangés
        return; // ON QUITTE LA FONCTION DIRECTEMENT pour éviter la boucle infinie !
    }
    int chemin[500];

    int meilleur_len = 99999; // On commence très haut pour gérer les pénalités
    int meilleur_type = -1;
    int meilleur_indice = -1;
    int meilleure_rotation = -1;
    bool meilleur_est_complet = false;

    // Coordonnées réelles du trésor (selon le stockage choisi dans ton projet)
    int destX = tuiles_tresor.x[tuiles_tresor.num_tresor];
    int destY = tuiles_tresor.y[tuiles_tresor.num_tresor];

    printf("===== DEBUT SIMULATION (Calcul du prochain coup) =====\n");

    for (int type = 0; type < 4; type++)
    {
        int limite_indice = (type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE) ? laby.sizeY : laby.sizeX;
        for (int indice = 1; indice < limite_indice; indice += 2)
        {
            for (int rotation = 0; rotation < 4; rotation++)
            {
                // 1. Réinitialisation sur la grille actuelle brute
                copie_laby(&laby);

                // 2. Test de l'insertion sur la copie
                essaie_insertion(&laby, type, indice, rotation);

                // 3. Lancement du BFS
                bool chemin_existe = phaseExpansion(&laby, joueur_actuel, &tuiles_tresor);

                int score_courant = 0;

                if (chemin_existe)
                {
                    // --- CAS A : UN CHEMIN COMPLET EXISTE ---
                    int len = phaseRemontee(&laby, joueur_actuel, &tuiles_tresor, chemin, 500);
                    if (len <= 0)
                        continue;

                    score_courant = len; // Le score est le nombre de pas réels
                }
                else
                {
                    // --- CAS B : PAS DE CHEMIN COMPLET (Rapprochement) ---
                    if (meilleur_est_complet)
                        continue;

                    int min_distance_manhattan = 9999;

                    for (int y = 0; y < laby.sizeY; y++)
                    {
                        for (int x = 0; x < laby.sizeX; x++)
                        {
                            if (laby.copy_laby_update[x][y] > 0)
                            {
                                int dist_manhattan = abs(x - destX) + abs(y - destY);
                                if (dist_manhattan < min_distance_manhattan)
                                {
                                    min_distance_manhattan = dist_manhattan;
                                }
                            }
                        }
                    }

                    // Score pénalisé : 1000 + la distance restante de Manhattan
                    score_courant = 1000 + min_distance_manhattan;
                }

                // 5. Sauvegarde si c'est le meilleur choix trouvé jusqu'ici
                if (score_courant < meilleur_len)
                {
                    meilleur_len = score_courant;
                    meilleur_type = type;
                    meilleur_indice = indice;
                    meilleure_rotation = rotation;
                    meilleur_est_complet = chemin_existe;

                    // Affichage des logs et de la carte
                    if (chemin_existe)
                    {
                        printf("\n>>> CHEMIN COMPLET TROUVÉ ! Longueur = %d pas\n", score_courant);
                    }
                    else
                    {
                        printf("\n>>> MEILLEUR RAPPROCHEMENT ! Distance restante = %d\n", score_courant - 1000);
                    }

                    printf("--- MAP DU MEILLEUR COUP TOURNÉ (Type:%d, Indice:%d, Rot:%d) ---\n", type, indice, rotation);
                    for (int y = 0; y < laby.sizeY; y++)
                    {
                        for (int x = 0; x < laby.sizeX; x++)
                        {
                            if (laby.copy_laby_update[x][y] == 0)
                                printf("  . ");
                            else
                                printf("%3d ", laby.copy_laby_update[x][y]);
                        }
                        printf("\n");
                    }
                    printf("----------------------------------------------------------------------\n\n");
                }
            }
        }
    }

    // =================================================================
    // ÉCRITURE DU PROCHAIN MOVE DIRECTEMENT DANS LE JOUEUR
    // =================================================================
    if (meilleur_type != -1)
    {
        joueur_actuel->type_insertion = meilleur_type;
        joueur_actuel->indice = meilleur_indice;
        joueur_actuel->rotation = meilleure_rotation;

        if (meilleur_est_complet)
        {
            // Chemin direct trouvé : le joueur va directement sur le trésor
            joueur_actuel->x = destX;
            joueur_actuel->y = destY;
        }
        else
        {
            // Rapprochement : On cherche quelle case visitée (> 0) était la plus proche du trésor
            int min_dist = 9999;
            int meilleur_x_possible = joueur_actuel->x;
            int meilleur_y_possible = joueur_actuel->y;

            for (int y = 0; y < laby.sizeY; y++)
            {
                for (int x = 0; x < laby.sizeX; x++)
                {
                    // Si la case a été atteinte par l'expansion
                    if (laby.copy_laby_update[x][y] > 0)
                    {
                        int dist = abs(x - destX) + abs(y - destY);
                        if (dist < min_dist)
                        {
                            min_dist = dist;
                            meilleur_x_possible = x;
                            meilleur_y_possible = y;
                        }
                    }
                }
            }
            // Le joueur marche jusqu'à la case accessible la plus proche
            joueur_actuel->x = meilleur_x_possible;
            joueur_actuel->y = meilleur_y_possible;
        }
    }
    else
    {
        // Sécurité si tout est bloqué à 100%
        joueur_actuel->type_insertion = 0;
        joueur_actuel->indice = 1;
        joueur_actuel->rotation = 0;
    }

    printf("\n========== MOVE APPLIQUÉ AU JOUEUR ==========\n");
    printf("Type choisi : %d\n", joueur_actuel->type_insertion);
    printf("Indice      : %d\n", joueur_actuel->indice);
    printf("Rotation    : %d\n", joueur_actuel->rotation);
    printf("Nouvel X/Y  : (%d, %d)\n", joueur_actuel->x, joueur_actuel->y);
    printf("==============================================\n");
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
    transfer_labydata_to_laby_update(&laby);
    print_laby(&laby, true);

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
        delay(1);
        printLabyrinth();
        delay(1);

        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);
        // printf("labydata \n%d\n", laby.labyData);
        // printf("laby_update \n%d\n", laby.laby_update);

        position_tresor(&laby, &tuiles_tresor);

        if (laby.tour_joueur == 0)
        {
            simulate_chemin_court(&yek);

            update_labyV2(&laby, &yek, &yek);
            printf("Entre coup chef : ");
            // scanf("%d %d %d %d %d", &yek.type_insertion, &yek.indice, &yek.rotation, &yek.x, &yek.y);
            sprintf(yek.coup_envoi, "%d %d %d %d %d",
                    yek.type_insertion,
                    yek.indice,
                    yek.rotation,
                    yek.x,
                    yek.y);

            resultat_move = sendMove(yek.coup_envoi, laby.message_serveur);
            laby.tour_joueur = 1;
        }
        else
        {

            printf("Attente du coup de l'adversaire\n");

            // On récupère le coup de l'adversaire
            resultat_move = getMove(adversaire.coup_recu, laby.message_serveur);
            printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

            laby.tour_joueur = 0;
            // On met à jour les coordonnées de l'adversaire
            sscanf(adversaire.coup_recu, "%d %d %d %d %d", &adversaire.type_insertion, &adversaire.indice, &adversaire.rotation, &adversaire.x, &adversaire.y);
            // update_laby(&laby, &adversaire);
            update_labyV2(&laby, &adversaire, &yek);

            printf("L'adversaire a joué vers : type : %d indice : %d rotation: %d x : %d y : %d\n", adversaire.type_insertion, adversaire.indice, adversaire.rotation, adversaire.x, adversaire.y);
        }
        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);

        print_laby(&laby, true);
    }

    // Affichage de la raison de la fin (gagné ou autre)
    printf("Fin de la partie ! Raison : %s\n", laby.message_serveur);

    free(laby.labyData);
    closeConnection();
    return 0;
}
