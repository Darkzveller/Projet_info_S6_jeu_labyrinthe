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
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds)
        ;
}

int coup_interdit_type = -1;
int coup_interdit_indice = -1;

void determiner_coup_interdit(int type_adversaire, int indice_adversaire,
                              int *coup_interdit_type, int *coup_interdit_indice)
{
    *coup_interdit_indice = indice_adversaire;
    *coup_interdit_type = type_adversaire ^ 1;
}

/* ------------------------------------------------------------------ */
/*  phaseRemontee : remonte le chemin depuis dist[][]                  */
/*  copy_laby_update doit contenir les TUILES (murs).                  */
/* ------------------------------------------------------------------ */
int phaseRemontee(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor,
                  int *chemin, int max_chemin)
{
    int destX = tuiles_tresor->x[tuiles_tresor->num_tresor];
    int destY = tuiles_tresor->y[tuiles_tresor->num_tresor];

    if (laby->dist[destX][destY] == 0)
        return 0;

    int x = destX, y = destY, idx = 0;

    while (!(x == yek->x && y == yek->y))
    {
        int current_value = laby->dist[x][y];
        int found = 0;

        for (int dir = 0; dir < 4; dir++)
        {
            int nx, ny;
            if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
                continue;
            if (laby->dist[nx][ny] == current_value - 1)
            {
                if (idx < max_chemin)
                    chemin[idx++] = dir;
                x = nx;
                y = ny;
                found = 1;
                break;
            }
        }

        if (!found)
            return 0;
    }

    /* Inversion du chemin (construit Arrivée→Départ) */
    for (int i = 0; i < idx / 2; i++)
    {
        int tmp = chemin[i];
        chemin[i] = chemin[idx - 1 - i];
        chemin[idx - 1 - i] = tmp;
    }
    return idx;
}

/* ------------------------------------------------------------------ */
/*  essaie_insertion : applique une insertion sur copy_laby_update     */
/* ------------------------------------------------------------------ */
void essaie_insertion(t_laby *laby, int type_insertion, int indice, int rotation)
{
    if (rotation != 0)
    {
        int nord = (laby->copy_extra.presence_mur >> SHIFT_BIT_NORD) & 1;
        int est = (laby->copy_extra.presence_mur >> SHIFT_BIT_EST) & 1;
        int sud = (laby->copy_extra.presence_mur >> SHIFT_BIT_SUD) & 1;
        int ouest = (laby->copy_extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
        int item = (laby->copy_extra.presence_mur >> 0) & 0xFF;

        for (int i = 0; i < rotation; i++)
        {
            int n = nord, e = est, s = sud, o = ouest;
            nord = o;
            est = n;
            sud = e;
            ouest = s;
        }
        laby->copy_extra.presence_mur =
            (nord << SHIFT_BIT_NORD) |
            (est << SHIFT_BIT_EST) |
            (sud << SHIFT_BIT_SUD) |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
    }

    if (type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y = indice;
        int temp = laby->copy_laby_update[laby->sizeX - 1][y];
        for (int x = laby->sizeX - 1; x > 0; x--)
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x - 1][y];
        laby->copy_laby_update[0][y] = laby->copy_extra.presence_mur;
        laby->copy_extra.presence_mur = temp;
    }
    else if (type_insertion == INSERT_LIGNE_DROITE)
    {
        int y = indice;
        int temp = laby->copy_laby_update[0][y];
        for (int x = 0; x < laby->sizeX - 1; x++)
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x + 1][y];
        laby->copy_laby_update[laby->sizeX - 1][y] = laby->copy_extra.presence_mur;
        laby->copy_extra.presence_mur = temp;
    }
    else if (type_insertion == INSERT_COLONNE_HAUT)
    {
        int x = indice;
        int temp = laby->copy_laby_update[x][laby->sizeY - 1];
        for (int y = laby->sizeY - 1; y > 0; y--)
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x][y - 1];
        laby->copy_laby_update[x][0] = laby->copy_extra.presence_mur;
        laby->copy_extra.presence_mur = temp;
    }
    else if (type_insertion == INSERT_COLONNE_BAS)
    {
        int x = indice;
        int temp = laby->copy_laby_update[x][0];
        for (int y = 0; y < laby->sizeY - 1; y++)
            laby->copy_laby_update[x][y] = laby->copy_laby_update[x][y + 1];
        laby->copy_laby_update[x][laby->sizeY - 1] = laby->copy_extra.presence_mur;
        laby->copy_extra.presence_mur = temp;
    }
}

/* ------------------------------------------------------------------ */
/*  simulate_chemin_court                                              */
/*  joueur_actuel->real_x/real_y = vraie position physique actuelle   */
/*  joueur_actuel->x/y sera écrasé avec la destination calculée       */
/* ------------------------------------------------------------------ */
void simulate_chemin_court(t_joueur *joueur_actuel, int interdit_type, int interdit_indice)
{
    /* --- Cas : déjà sur le trésor --- */
    if (joueur_actuel->real_x == tuiles_tresor.x[tuiles_tresor.num_tresor] &&
        joueur_actuel->real_y == tuiles_tresor.y[tuiles_tresor.num_tresor])
    {
        printf("[INFO] Déjà sur le trésor en (%d, %d) ! On valide sans bouger.\n",
               joueur_actuel->real_x, joueur_actuel->real_y);

        int type_secours = -1;
        int indice_secours = -1;

        for (int type = 0; type < 4 && type_secours == -1; type++)
        {
            int limite = (type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE)
                             ? laby.sizeY
                             : laby.sizeX;
            for (int indice = 1; indice < limite; indice += 2)
            {
                if (type == interdit_type && indice == interdit_indice)
                    continue;
                /* Ne pas pousser notre propre ligne */
                if ((type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE) && indice == joueur_actuel->real_y)
                    continue;
                /* Ne pas pousser notre propre colonne */
                if ((type == INSERT_COLONNE_HAUT || type == INSERT_COLONNE_BAS) && indice == joueur_actuel->real_x)
                    continue;

                type_secours = type;
                indice_secours = indice;
                break;
            }
        }

        if (type_secours == -1)
        {
            type_secours = 0;
            indice_secours = 1;
        }

        joueur_actuel->type_insertion = type_secours;
        joueur_actuel->indice = indice_secours;
        joueur_actuel->rotation = 0;
        /* Destination = position actuelle (on ne bouge pas) */
        joueur_actuel->x = joueur_actuel->real_x;
        joueur_actuel->y = joueur_actuel->real_y;

        tuiles_tresor.num_tresor += 1;
        return;
    }

    /* --- Cas général : cherche le meilleur coup --- */
    int chemin[500];
    int meilleur_len = 99999;
    int meilleur_type = -1;
    int meilleur_indice = -1;
    int meilleure_rotation = -1;
    bool meilleur_est_complet = false;

    int destination_finale_x = joueur_actuel->real_x;
    int destination_finale_y = joueur_actuel->real_y;

    int destX = tuiles_tresor.x[tuiles_tresor.num_tresor];
    int destY = tuiles_tresor.y[tuiles_tresor.num_tresor];

    printf("===== DEBUT SIMULATION (Calcul du prochain coup) =====\n");

    for (int type = 0; type < 4; type++)
    {
        int limite_indice = (type == INSERT_LIGNE_GAUCHE || type == INSERT_LIGNE_DROITE)
                                ? laby.sizeY
                                : laby.sizeX;
        for (int indice = 1; indice < limite_indice; indice += 2)
        {
            if (type == interdit_type && indice == interdit_indice)
                continue;

            for (int rotation = 0; rotation < 4; rotation++)
            {
                copie_laby(&laby);
                essaie_insertion(&laby, type, indice, rotation);

                /* Position virtuelle du joueur après poussée */
                t_joueur joueur_virtuel = *joueur_actuel;
                joueur_virtuel.x = joueur_actuel->real_x;
                joueur_virtuel.y = joueur_actuel->real_y;

                if (type == INSERT_LIGNE_GAUCHE && joueur_virtuel.y == indice)
                {
                    joueur_virtuel.x++;
                    if (joueur_virtuel.x >= laby.sizeX)
                        joueur_virtuel.x = 0;
                }
                else if (type == INSERT_LIGNE_DROITE && joueur_virtuel.y == indice)
                {
                    joueur_virtuel.x--;
                    if (joueur_virtuel.x < 0)
                        joueur_virtuel.x = laby.sizeX - 1;
                }
                else if (type == INSERT_COLONNE_HAUT && joueur_virtuel.x == indice)
                {
                    joueur_virtuel.y++;
                    if (joueur_virtuel.y >= laby.sizeY)
                        joueur_virtuel.y = 0;
                }
                else if (type == INSERT_COLONNE_BAS && joueur_virtuel.x == indice)
                {
                    joueur_virtuel.y--;
                    if (joueur_virtuel.y < 0)
                        joueur_virtuel.y = laby.sizeY - 1;
                }

                /* Position virtuelle du TRÉSOR après la même poussée */
                int destX_local = destX;
                int destY_local = destY;

                if (type == INSERT_LIGNE_GAUCHE && indice == destY_local)
                {
                    destX_local++;
                    if (destX_local >= laby.sizeX)
                        destX_local = 0;
                }
                else if (type == INSERT_LIGNE_DROITE && indice == destY_local)
                {
                    destX_local--;
                    if (destX_local < 0)
                        destX_local = laby.sizeX - 1;
                }
                else if (type == INSERT_COLONNE_HAUT && indice == destX_local)
                {
                    destY_local++;
                    if (destY_local >= laby.sizeY)
                        destY_local = 0;
                }
                else if (type == INSERT_COLONNE_BAS && indice == destX_local)
                {
                    destY_local--;
                    if (destY_local < 0)
                        destY_local = laby.sizeY - 1;
                }

                /* BFS avec la position locale du trésor */
                t_tuiles tuiles_locales = tuiles_tresor;
                tuiles_locales.x[tuiles_locales.num_tresor] = destX_local;
                tuiles_locales.y[tuiles_locales.num_tresor] = destY_local;

                bool chemin_existe = phaseExpansion(&laby, &joueur_virtuel, &tuiles_locales);

                int score_courant = 0;
                int cible_locale_x = joueur_virtuel.x;
                int cible_locale_y = joueur_virtuel.y;

                if (chemin_existe)
                {
                    int len = phaseRemontee(&laby, &joueur_virtuel, &tuiles_locales, chemin, 500);
                    if (len <= 0)
                        continue;
                    score_courant = len;
                    cible_locale_x = destX_local;
                    cible_locale_y = destY_local;
                }
                else
                {
                    if (meilleur_est_complet)
                        continue;

                    int min_dist = 9999;
                    for (int y = 0; y < laby.sizeY; y++)
                    {
                        for (int x = 0; x < laby.sizeX; x++)
                        {
                            if (laby.dist[x][y] > 0)
                            {
                                /* Distance vers la position locale du trésor (après insertion) */
                                int d = abs(x - destX_local) + abs(y - destY_local);
                                if (d < min_dist)
                                {
                                    min_dist = d;
                                    cible_locale_x = x;
                                    cible_locale_y = y;
                                }
                            }
                        }
                    }
                    score_courant = 1000 + min_dist;
                }

                if (score_courant < meilleur_len)
                {
                    meilleur_len = score_courant;
                    meilleur_type = type;
                    meilleur_indice = indice;
                    meilleure_rotation = rotation;
                    meilleur_est_complet = chemin_existe;
                    destination_finale_x = cible_locale_x;
                    destination_finale_y = cible_locale_y;

                    if (chemin_existe)
                        printf("\n>>> CHEMIN COMPLET TROUVÉ ! Longueur = %d pas\n", score_courant);
                    else
                        printf("\n>>> MEILLEUR RAPPROCHEMENT ! Distance restante = %d\n", score_courant - 1000);

                    printf("--- MAP DU MEILLEUR COUP (Type:%d, Indice:%d, Rot:%d) ---\n",
                           type, indice, rotation);
                    for (int y = 0; y < laby.sizeY; y++)
                    {
                        for (int x = 0; x < laby.sizeX; x++)
                            printf(laby.dist[x][y] == 0 ? "  . " : "%3d ", laby.dist[x][y]);
                        printf("\n");
                    }
                    printf("------------------------------------------------------\n\n");
                }
            }
        }
    }

    if (meilleur_type != -1)
    {
        joueur_actuel->type_insertion = meilleur_type;
        joueur_actuel->indice = meilleur_indice;
        joueur_actuel->rotation = meilleure_rotation;
        joueur_actuel->x = destination_finale_x;
        joueur_actuel->y = destination_finale_y;
    }
    else
    {
        /* Coup de secours si rien trouvé */
        joueur_actuel->type_insertion = 0;
        joueur_actuel->indice = (interdit_type == 0 && interdit_indice == 1) ? 3 : 1;
        joueur_actuel->rotation = 0;
        joueur_actuel->x = joueur_actuel->real_x;
        joueur_actuel->y = joueur_actuel->real_y;
    }

    printf("\n========== MOVE APPLIQUÉ AU JOUEUR ==========\n");
    printf("Type choisi : %d\n", joueur_actuel->type_insertion);
    printf("Indice      : %d\n", joueur_actuel->indice);
    printf("Rotation    : %d\n", joueur_actuel->rotation);
    printf("Nouvel X/Y  : (%d, %d)\n", joueur_actuel->x, joueur_actuel->y);
    printf("==============================================\n");
}

/* ================================================================== */
/*  MAIN                                                               */
/* ================================================================== */
int main()
{
    printf("caca\n");

    connectToServer(nom_serveur, port_serveur, nom_bot_moi);

    waitForLabyrinth(type_partie_choisi[BOT_BASIC], laby.labyrinthName, &laby.sizeX, &laby.sizeY);

    taille_buffer = (laby.sizeX * laby.sizeY + 5) * 11;
    laby.labyData = malloc(taille_buffer * sizeof(char));
    laby.tour_joueur = getLabyrinth(laby.labyData);
    transfer_labydata_to_laby_update(&laby);
    print_laby(&laby, true);

    t_return_code resultat_move = NORMAL_MOVE;
    tuiles_tresor.num_tresor = 1;

    /* Position initiale */
    yek.x = 0;
    yek.y = 0;
    yek.real_x = 0;
    yek.real_y = 0;
    adversaire.x = laby.sizeX - 1;
    adversaire.y = laby.sizeY - 1;

    while (resultat_move == NORMAL_MOVE)
    {
        printf("\nAffichage du labyrinthe :\n");
        delay(1);
        printLabyrinth();
        printf("Position réelle yek : real_x=%d real_y=%d\n", yek.real_x, yek.real_y);
        printf("Trésor cherché #%d en (%d, %d)\n",
               tuiles_tresor.num_tresor,
               tuiles_tresor.x[tuiles_tresor.num_tresor],
               tuiles_tresor.y[tuiles_tresor.num_tresor]);
         delay(1);

        position_tresor(&laby, &tuiles_tresor);

        if (laby.tour_joueur == 0)
        {
            /* ---- C'est notre tour ---- */

            /* 1. Calcule le meilleur coup (utilise real_x/real_y en interne) */
            simulate_chemin_court(&yek, coup_interdit_type, coup_interdit_indice);

            /* 2. Sauvegarde la destination calculée */
            int targetX = yek.x;
            int targetY = yek.y;

            /* 3. Applique l'insertion sur laby_update avec la VRAIE position */
            yek.x = yek.real_x;
            yek.y = yek.real_y;
            update_labyV2(&laby, &yek, &yek);
            /* Après update_labyV2, yek.x/y = position glissée éventuelle
               On l'ignore : la destination reste targetX/Y */

            /* 4. Met à jour real_x/y = destination atteinte */
            yek.x = targetX;
            yek.y = targetY;
            yek.real_x = targetX;
            yek.real_y = targetY;

            /* 5. Envoie le coup au serveur */
            sprintf(yek.coup_envoi, "%d %d %d %d %d",
                    yek.type_insertion, yek.indice, yek.rotation,
                    targetX, targetY);
            printf("Entre coup chef : ");
            resultat_move = sendMove(yek.coup_envoi, laby.message_serveur);
            laby.tour_joueur = 1;
        }
        else
        {
            /* ---- Tour de l'adversaire ---- */
            printf("Attente du coup de l'adversaire\n");
            resultat_move = getMove(adversaire.coup_recu, laby.message_serveur);
            printf("L'adversaire a joué : %s\n", adversaire.coup_recu);

            laby.tour_joueur = 0;
            sscanf(adversaire.coup_recu, "%d %d %d %d %d",
                   &adversaire.type_insertion, &adversaire.indice, &adversaire.rotation,
                   &adversaire.x, &adversaire.y);

            /* Applique l'insertion adverse sur le vrai plateau
               yek.x/y contient real_x/y donc update_labyV2 calcule
               correctement le glissement éventuel */
            yek.x = yek.real_x;
            yek.y = yek.real_y;
            update_labyV2(&laby, &adversaire, &yek);

            /* Synchronise real après glissement éventuel */
            yek.real_x = yek.x;
            yek.real_y = yek.y;

            printf("L'adversaire a joué vers : type:%d indice:%d rot:%d x:%d y:%d\n",
                   adversaire.type_insertion, adversaire.indice, adversaire.rotation,
                   adversaire.x, adversaire.y);
            determiner_coup_interdit(adversaire.type_insertion, adversaire.indice,
                                     &coup_interdit_type, &coup_interdit_indice);
        }

        print_laby(&laby, true);
    }

    if (resultat_move == WINNING_MOVE)
    {
        printf("vous avez gagné !\n");
        // while (1);
    }
    printf("Fin de la partie ! Raison : %s\n", laby.message_serveur);

    free(laby.labyData);
    closeConnection();
    return 0;
}