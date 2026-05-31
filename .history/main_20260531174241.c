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
int coup_interdit_type = -1;
int coup_interdit_indice = -1;

void determiner_coup_interdit(int type_adversaire, int indice_adversaire, int *coup_interdit_type, int *coup_interdit_indice)
{
    // On utilise l'astérisque pour modifier la valeur stockée à l'adresse mémoire
    *coup_interdit_indice = indice_adversaire;

    // Inversion binaire (0 <-> 1 et 2 <-> 3)
    *coup_interdit_type = type_adversaire ^ 1;
}

int phaseRemontee(t_laby *laby, t_joueur *yek, int target_x, int target_y, int *chemin, int max_chemin)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    // =================================================================
    // 1. AFFICHAGE DU TABLEAU DES DISTANCES (Pour le débuggage)
    // =================================================================
    // printf("\n--- MAP DES DISTANCES (laby->copy_laby_update) ---\n");
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            // On affiche un point si la case vaut 0 (inaccessible)
            if (laby->copy_laby_update[x][y] == 0)
            {
                // printf("  . ");
            }
            else
            {
                // printf("%3d ", laby->copy_laby_update[x][y]);
            }
        }
        // printf("\n");
    }
    // printf("--------------------------------------------------\n\n");

    // Coordonnées de la destination (le trésor ciblé)
    int destX = target_x;
    int destY = target_y;

    // printf("[DEBUG] DEPART  (Joueur) : (%d, %d) -> Valeur: %d\n", yek->x, yek->y, laby->copy_laby_update[yek->x][yek->y]);
    // printf("[DEBUG] ARRIVEE (Trésor) : (%d, %d) -> Valeur: %d\n", destX, destY, laby->copy_laby_update[destX][destY]);

    // On commence la remontée depuis l'arrivée
    int x = destX;
    int y = destY;
    int idx = 0;

    // Sécurité : si l'arrivée possède une distance de 0, elle est strictement inaccessible
    if (laby->copy_laby_update[x][y] == 0)
    {
        // printf("[ERREUR] La destination (%d, %d) vaut 0. Impossible de remonter.\n", x, y);
        return 0;
    }

    // =================================================================
    // 2. BOUCLE DE REMONTEE (De l'arrivée vers le départ)
    // =================================================================
    while (!(x == yek->x && y == yek->y))
    {
        int current_value = laby->copy_laby_update[x][y];
        int found = 0;

        // printf("[REMONTEE] Case actuelle: (%d, %d) avec distance = %d\n", x, y, current_value);

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
            // printf("[ERREUR REMONTEE] Bloqué sur la case (%d, %d) de valeur %d. Aucun voisin ne vaut %d\n",
                //    x, y, current_value, current_value - 1);
            return 0;
        }
    }

    // printf("[REMONTEE] Succès ! Arrivé au joueur à la case (%d, %d)\n", x, y);

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

void simulate_chemin_court(t_joueur *joueur_actuel, int interdit_type, int interdit_indice)
{
    int chemin[500];
    int meilleur_score = 99999;
    int meilleur_type = -1;
    int meilleur_indice = -1;
    int meilleure_rotation = -1;

    meilleur_chemin_complet.count = 0;
    meilleur_chemin_complet.total_bonus_dist = 0;
    meilleur_treasures_collected = 0;
    int bonus_max = (tuiles_tresor.num_tresor == 1) ? 9 : 0;



    // printf("===== DEBUT SIMULATION (Calcul du prochain coup) =====\n");

    for (int type = 0; type < 4; type++)
    {
        int limite_indice = (type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE) ? laby.sizeY : laby.sizeX;
        for (int indice = 1; indice < limite_indice; indice += 2)
        {
            if (type == interdit_type && indice == interdit_indice)
            {
                continue;
            }
            for (int rotation = 0; rotation < 4; rotation++)
            {
                copie_laby(&laby);
                essaie_insertion(&laby, type, indice, rotation);

                // --- CALCUL DE LA POSITION DU JOUEUR VIRTUEL APRÈS POUSSÉE ---
                t_joueur joueur_virtuel = *joueur_actuel;

                if ((type == INSERT_LIGNE_GAUCHE) && (joueur_virtuel.y == indice))
                {
                    joueur_virtuel.x++;
                    if (joueur_virtuel.x >= laby.sizeX)
                        joueur_virtuel.x = 0;
                }
                else if ((type == INSERT_LIGNE_DROITE) && (joueur_virtuel.y == indice))
                {
                    joueur_virtuel.x--;
                    if (joueur_virtuel.x < 0)
                        joueur_virtuel.x = laby.sizeX - 1;
                }
                else if ((type == INSERT_COLONNE_HAUT) && (joueur_virtuel.x == indice))
                {
                    joueur_virtuel.y++;
                    if (joueur_virtuel.y >= laby.sizeY)
                        joueur_virtuel.y = 0;
                }
                else if ((type == INSERT_COLONNE_BAS) && (joueur_virtuel.x == indice))
                {
                    joueur_virtuel.y--;
                    if (joueur_virtuel.y < 0)
                        joueur_virtuel.y = laby.sizeY - 1;
                }

                // Sauvegarde de l'état propre du labyrinthe post-insertion
                int grid_backup[21][13];
                for (int y = 0; y < laby.sizeY; y++) {
                    for (int x = 0; x < laby.sizeX; x++) {
                        grid_backup[x][y] = laby.copy_laby_update[x][y];
                    }
                }

                // Localiser tous les trésors sur la grille après insertion
                int tresor_x[26], tresor_y[26];
                localiser_tresors(&laby, tresor_x, tresor_y, laby.tour_joueur);

                t_multi_path path_courant;
                path_courant.count = 0;
                path_courant.total_bonus_dist = 0;

                int current_target = tuiles_tresor.num_tresor;
                int target_x = tresor_x[current_target];
                int target_y = tresor_y[current_target];

                bool premier_atteint = false;
                int dist_to_first = 9999;

                if (target_x != -1 && target_y != -1) {
                    // Restaurer la grille propre avant le BFS
                    for (int y = 0; y < laby.sizeY; y++) {
                        for (int x = 0; x < laby.sizeX; x++) {
                            laby.copy_laby_update[x][y] = grid_backup[x][y];
                        }
                    }

                    premier_atteint = phaseExpansion(&laby, &joueur_virtuel, target_x, target_y);
                    if (premier_atteint) {
                        int path_len = phaseRemontee(&laby, &joueur_virtuel, target_x, target_y, chemin, 500);
                        if (path_len >= 0) {
                            path_courant.x[path_courant.count] = target_x;
                            path_courant.y[path_courant.count] = target_y;
                            path_courant.count++;

                            current_target++;
                            while (current_target <= 24) {
                                int next_x = tresor_x[current_target];
                                int next_y = tresor_y[current_target];
                                if (next_x == -1 || next_y == -1) break;

                                t_joueur pos_last;
                                pos_last.x = path_courant.x[path_courant.count - 1];
                                pos_last.y = path_courant.y[path_courant.count - 1];

                                // Restaurer la grille propre avant chaque nouveau BFS
                                for (int y = 0; y < laby.sizeY; y++) {
                                    for (int x = 0; x < laby.sizeX; x++) {
                                        laby.copy_laby_update[x][y] = grid_backup[x][y];
                                    }
                                }

                                if (phaseExpansion(&laby, &pos_last, next_x, next_y)) {
                                    int next_len = phaseRemontee(&laby, &pos_last, next_x, next_y, chemin, 500);
                                    if (bonus_max > 0 && path_courant.total_bonus_dist + next_len <= bonus_max) {
                                        path_courant.x[path_courant.count] = next_x;
                                        path_courant.y[path_courant.count] = next_y;
                                        path_courant.count++;
                                        path_courant.total_bonus_dist += next_len;
                                        current_target++;
                                    } else {
                                        break;
                                    }
                                } else {
                                    break;
                                }
                            }

                            if (current_target <= 24) {
                                int next_x = tresor_x[current_target];
                                int next_y = tresor_y[current_target];
                                if (next_x != -1 && next_y != -1) {
                                    t_joueur pos_last;
                                    pos_last.x = path_courant.x[path_courant.count - 1];
                                    pos_last.y = path_courant.y[path_courant.count - 1];

                                    // Restaurer la grille propre avant le BFS de rapprochement
                                    for (int y = 0; y < laby.sizeY; y++) {
                                        for (int x = 0; x < laby.sizeX; x++) {
                                            laby.copy_laby_update[x][y] = grid_backup[x][y];
                                        }
                                    }

                                    phaseExpansion(&laby, &pos_last, next_x, next_y);

                                    int min_dist_manhattan = 9999;
                                    int best_cx = pos_last.x;
                                    int best_cy = pos_last.y;
                                    int remaining_steps = bonus_max - path_courant.total_bonus_dist;


                                    for (int y = 0; y < laby.sizeY; y++) {
                                        for (int x = 0; x < laby.sizeX; x++) {
                                            int val = laby.copy_laby_update[x][y];
                                            if (val > 0) {
                                                int steps = val - 1;
                                                if (steps <= remaining_steps) {
                                                    int dist = abs(x - next_x) + abs(y - next_y);
                                                    if (dist < min_dist_manhattan) {
                                                        min_dist_manhattan = dist;
                                                        best_cx = x;
                                                        best_cy = y;
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if (best_cx != pos_last.x || best_cy != pos_last.y) {
                                        path_courant.x[path_courant.count] = best_cx;
                                        path_courant.y[path_courant.count] = best_cy;
                                        path_courant.count++;
                                        path_courant.total_bonus_dist += (laby.copy_laby_update[best_cx][best_cy] - 1);
                                    }
                                }
                            }
                        }
                    } else {
                        // Restaurer la grille propre avant le BFS de rapprochement initial
                        for (int y = 0; y < laby.sizeY; y++) {
                            for (int x = 0; x < laby.sizeX; x++) {
                                 laby.copy_laby_update[x][y] = grid_backup[x][y];
                            }
                        }

                        phaseExpansion(&laby, &joueur_virtuel, target_x, target_y);
                        int min_dist_manhattan = 9999;
                        int best_cx = joueur_virtuel.x;
                        int best_cy = joueur_virtuel.y;

                        for (int y = 0; y < laby.sizeY; y++) {
                            for (int x = 0; x < laby.sizeX; x++) {
                                if (laby.copy_laby_update[x][y] > 0) {
                                    int dist = abs(x - target_x) + abs(y - target_y);
                                    if (dist < min_dist_manhattan) {
                                        min_dist_manhattan = dist;
                                        best_cx = x;
                                        best_cy = y;
                                    }
                                }
                            }
                        }
                        path_courant.x[0] = best_cx;
                        path_courant.y[0] = best_cy;
                        path_courant.count = 1;
                        dist_to_first = min_dist_manhattan;
                    }
                }

                int score_courant = 0;
                int treasures_collected = 0;
                if (premier_atteint) {
                    treasures_collected = current_target - tuiles_tresor.num_tresor;
                    int dist_to_next = 0;
                    if (bonus_max > 0 && current_target <= 24) {
                        int final_x = path_courant.x[path_courant.count - 1];
                        int final_y = path_courant.y[path_courant.count - 1];
                        int next_x = tresor_x[current_target];
                        int next_y = tresor_y[current_target];
                        dist_to_next = abs(final_x - next_x) + abs(final_y - next_y);
                    }
                    score_courant = - (treasures_collected * 10000) + dist_to_next;
                } else {
                    score_courant = 1000 + dist_to_first;
                }

                if (score_courant < meilleur_score)
                {
                    meilleur_score = score_courant;
                    meilleur_type = type;
                    meilleur_indice = indice;
                    meilleure_rotation = rotation;

                    meilleur_chemin_complet = path_courant;
                    meilleur_treasures_collected = treasures_collected;
                }
            }
        }
    }

    if (meilleur_type != -1)
    {
        joueur_actuel->type_insertion = meilleur_type;
        joueur_actuel->indice = meilleur_indice;
        joueur_actuel->rotation = meilleure_rotation;

        joueur_actuel->x = meilleur_chemin_complet.x[meilleur_chemin_complet.count - 1];
        joueur_actuel->y = meilleur_chemin_complet.y[meilleur_chemin_complet.count - 1];
    }
    else
    {
        joueur_actuel->type_insertion = 0;
        joueur_actuel->indice = 1;
        joueur_actuel->rotation = 0;
        
        meilleur_chemin_complet.x[0] = joueur_actuel->x;
        meilleur_chemin_complet.y[0] = joueur_actuel->y;
        meilleur_chemin_complet.count = 1;
        meilleur_chemin_complet.total_bonus_dist = 0;
        meilleur_treasures_collected = 0;
    }
    //DEBUG
    printf("[DEBUG] num_tresor=%d bonus_max=%d collected=%d\n",

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

    // waitForLabyrinth(type_partie_choisi[(BOT_ALEATOIRE)], laby.labyrinthName, &laby.sizeX, &laby.sizeY);
    // waitForLabyrinth(type_partie_choisi[(BOT_BASIC)], laby.labyrinthName, &laby.sizeX, &laby.sizeY);
    waitForLabyrinth("TRAINING REGULAR", laby.labyrinthName, &laby.sizeX, &laby.sizeY);

#if DEBUG_DATA_STRUCT_LABY
    printf("Partie trouveer : %s il est de taille en x : %d et y : %d)\n", laby.labyrinthName, laby.sizeX, laby.sizeY);
#endif

    taille_buffer = (laby.sizeX * laby.sizeY + 5) * 11;
    laby.labyData = malloc(taille_buffer * sizeof(char));
    laby.tour_joueur = getLabyrinth(laby.labyData);
int mon_numero_joueur = laby.tour_joueur;
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
        delay(1);
        printLabyrinth();
        delay(1);

        // afficheLabyrinthe(laby.laby_update, 500, laby.sizeX, laby.sizeY, yek.x, yek.y, adversaire.x, adversaire.y);
        // printf("labydata \n%d\n", laby.labyData);
        // printf("laby_update \n%d\n", laby.laby_update);

        position_tresor(&laby, &tuiles_tresor,mon_numero_joueur);

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
            for (int i = 0; i < meilleur_chemin_complet.count; i++) {
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
            printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

            // laby.tour_joueur = 0;
            tour_actuel = mon_numero_joueur;
            // On met à jour les coordonnées de l'adversaire en tenant compte des étapes multiples
            int type_ins, ind_ins, rot_ins;
            int offset = 0;
            if (sscanf(adversaire.coup_recu, "%d %d %d%n", &type_ins, &ind_ins, &rot_ins, &offset) >= 3) {
                adversaire.type_insertion = type_ins;
                adversaire.indice = ind_ins;
                adversaire.rotation = rot_ins;
                char *ptr = adversaire.coup_recu + offset;
                int tx, ty;
                int read_bytes;
                while (sscanf(ptr, "%d %d%n", &tx, &ty, &read_bytes) == 2) {
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
        }
        else
        {
            printf("Dommage... L'adversaire a trouvé son dernier trésor et a GAGNÉ. 😞\n");
        }
    }
    else if (resultat_move == LOSING_MOVE)
    {
        if (dernier_joueur_actif == mon_numero_joueur)
        {
            printf("Vous avez PERDU... Vous avez joué un coup invalide ou fait un timeout. ❌\n");
        }
        else
        {
            printf("Victoire par forfait ! L'adversaire a commis une erreur et a PERDU. 🏆\n");
        }
    }
    else
    {
        printf("Fin de partie inconnue ou interruption serveur.\n");
    }

    // Affichage de la raison officielle envoyée par le serveur
    printf("Raison du serveur : %s\n", laby.message_serveur);
    free(laby.labyData);
    closeConnection();
    return 0;
}