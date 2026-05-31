#include "Mouvement.h"

// void position_tresor(t_laby *laby, t_tuiles *tuiles)
// {
//     // On parcourt la matrice réelle [x][y] mise à jour
//     for (int y = 0; y < laby->sizeY; y++)
//     {
//         for (int x = 0; x < laby->sizeX; x++)
//         {
//             // 1. On récupère la valeur binaire de la tuile actuelle
//             int tuile_cle = laby->laby_update[x][y];

//             // 2. On extrait l'ID de l'item (les 8 bits de poids faible)
//             int item = tuile_cle & 0xFF;

//             // 3. Si un trésor (item >= 1) est présent sur cette tuile
//             if (item >= 1)
//             {
//                 tuiles->x[item] = x;
//                 tuiles->y[item] = y;

//                 // On garde l'information des murs de la tuile au cas où
//                 tuiles->presence_mur[item] = tuile_cle;
//             }
//         }
//     }

//     // Surtout PAS de "tuiles->num_tresor = 1;" ici !
// }
void position_tresor(t_laby *laby, t_tuiles *tuiles, int mon_numero_joueur)
{
    // 1. On parcourt la matrice réelle [x][y] mise à jour
    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            // On récupère la valeur binaire de la tuile actuelle
            int tuile_cle = laby->laby_update[x][y];

            // On extrait l'ID réel du trésor sur le plateau (les 8 bits de poids faible)
            int item_reel = tuile_cle & 0xFF;

            // Si un trésor (item_reel >= 1) est présent sur cette tuile
            if (item_reel >= 1 && item_reel <= 24)
            {
                int item_virtuel;

                if (mon_numero_joueur == 0)
                {
                    // Joueur 0 : Pas de changement (Trésor 1 va dans l'indice 1) 
                    item_virtuel = item_reel;
                }
                else
                {
                    // Joueur 1 : Inversion (Trésor 24 va dans l'indice 1, le 23 dans l'indice 2, etc.) 
                    item_virtuel = 25 - item_reel;
                }

                // On stocke les coordonnées à l'indice virtuel attendu par ton simulateur
                tuiles->x[item_virtuel] = x;
                tuiles->y[item_virtuel] = y;

                // On garde l'information des murs à l'indice virtuel au cas où
                tuiles->presence_mur[item_virtuel] = tuile_cle;
            }
        }
    }

    // Surtout PAS de "tuiles->num_tresor = 1;" ici !
}

void print_laby(t_laby *laby, bool activate)
{
    if (!activate)
    {
        return;
    }
    printf("Laby data que j'obtiens\n");

    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {

            // int v = laby->laby_update[x][y];
            int nord = (laby->laby_update[x][y] >> SHIFT_BIT_NORD) & 1;
            int est = (laby->laby_update[x][y] >> SHIFT_BIT_EST) & 1;
            int sud = (laby->laby_update[x][y] >> SHIFT_BIT_SUD) & 1;
            int ouest = (laby->laby_update[x][y] >> SHIFT_BIT_OUEST) & 1;
            int item = (laby->laby_update[x][y] >> 0) & 0xFF;

            if ((x == (laby->sizeX) - 1) && (y == (laby->sizeY - 1)))
            {
                // printf("\nLa tuile supplementaire est celle ci : %d%d%d%d %d ", nord, est, sud, ouest, item);
                int nord_extra = (laby->extra.presence_mur >> SHIFT_BIT_NORD) & 1;
                int est_extra = (laby->extra.presence_mur >> SHIFT_BIT_EST) & 1;
                int sud_extra = (laby->extra.presence_mur >> SHIFT_BIT_SUD) & 1;
                int ouest_extra = (laby->extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
                int item_extra = (laby->extra.presence_mur >> 0) & 0xFF;

                printf("\nLa tuile supplementaire est celle ci : %d%d%d%d %d ", nord_extra, est_extra, sud_extra, ouest_extra, item_extra);
            }
            else
            {
                printf("%d%d%d%d%d ", nord, est, sud, ouest, item);
            }
        }
        printf("\n");
    }
}
void transfer_labydata_to_laby_update(t_laby *laby)
{
    char *ptr = laby->labyData;
    int nord = 0;
    int est = 0;
    int sud = 0;
    int ouest = 0;
    int item = 0;
    int offset = 0;

    laby->laby_update = (int **)malloc(((laby->sizeX * laby->sizeY + 5) * 11) * sizeof(int *));
    laby->copy_laby_update = (int **)malloc(((laby->sizeX * laby->sizeY + 5) * 11) * sizeof(int *));

    for (int i = 0; i < ((laby->sizeX * laby->sizeY + 5) * 11); i++)
    {
        laby->laby_update[i] = malloc(((laby->sizeX * laby->sizeY + 5) * 11) * sizeof(int));
        laby->copy_laby_update[i] = malloc(((laby->sizeX * laby->sizeY + 5) * 11) * sizeof(int));
    }

    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            if (sscanf(ptr, "%d %d %d %d %d%n", &nord, &est, &sud, &ouest, &item, &offset) >= 5)
            {
                /*
                 On utilise 16 bits au total :
                 - 4 bits pour les murs (Nord, Est, Sud, Ouest)
                 - 12 bits pour l'identifiant de l'item (trésor)

                 Structure des bits :
                 [ N E S O ] [     ITEM     ]
                   4 bits        8 bits

                 Chaque mur vaut 0 ou 1
                 ITEM peut aller de 0 à 256
                */
                laby->laby_update[x][y] =
                    (nord << SHIFT_BIT_NORD) |
                    (est << SHIFT_BIT_EST) |
                    (sud << SHIFT_BIT_SUD) |
                    (ouest << SHIFT_BIT_OUEST) |
                    (item);
                ptr += offset;
            }
        }
    }
    // 2. SEULEMENT ICI (après les boucles), on lit la TUILE EXTRA
    if (sscanf(ptr, "%d %d %d %d %d%n", &nord, &est, &sud, &ouest, &item, &offset) >= 5)
    {
        laby->extra.presence_mur = (nord << SHIFT_BIT_NORD) |
                                   (est << SHIFT_BIT_EST) |
                                   (sud << SHIFT_BIT_SUD) |
                                   (ouest << SHIFT_BIT_OUEST) |
                                   (item);

        laby->copy_extra.presence_mur = laby->extra.presence_mur;
    }
}

void update_labyV2(t_laby *laby, t_joueur *joueur_qui_joue, t_joueur *yek)
{
    // 1. Gestion de la rotation de l'extra (Inchangé, c'est très bien)
    if (joueur_qui_joue->rotation != 0)
    {
        int nord = (laby->extra.presence_mur >> SHIFT_BIT_NORD) & 1;
        int est = (laby->extra.presence_mur >> SHIFT_BIT_EST) & 1;
        int sud = (laby->extra.presence_mur >> SHIFT_BIT_SUD) & 1;
        int ouest = (laby->extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
        int item = (laby->extra.presence_mur >> 0) & 0xFF;

        for (int i = 0; i < joueur_qui_joue->rotation; i++)
        {
            int nord_old = nord, est_old = est, sud_old = sud, ouest_old = ouest;
            nord = ouest_old;
            est = nord_old;
            sud = est_old;
            ouest = sud_old;
        }
        laby->extra.presence_mur = (nord << SHIFT_BIT_NORD) | (est << SHIFT_BIT_EST) |
                                   (sud << SHIFT_BIT_SUD) | (ouest << SHIFT_BIT_OUEST) | (item);
    }

    // 2. Décalages, insertions et mise à jour de TA position (yek)
    if (joueur_qui_joue->type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y_cible = joueur_qui_joue->indice;
        int temp = laby->laby_update[laby->sizeX - 1][y_cible];

        // Si TU es sur la ligne poussée vers la droite
        if (yek->y == y_cible)
        {
            if (yek->x == laby->sizeX - 1) // Tu étais sur la case qui sort à droite
                yek->x = 0;                // Tu réapparais sur la case insérée à gauche
            else
                yek->x++;                  // Sinon, tu glisses simplement vers la droite
        }

        for (int x = laby->sizeX - 1; x > 0; x--)
            laby->laby_update[x][y_cible] = laby->laby_update[x - 1][y_cible];

        laby->laby_update[0][y_cible] = laby->extra.presence_mur;
        laby->extra.presence_mur = temp;
    }
    else if (joueur_qui_joue->type_insertion == INSERT_LIGNE_DROITE)
    {
        int y_cible = joueur_qui_joue->indice;
        int temp = laby->laby_update[0][y_cible];

        // Si TU es sur la ligne poussée vers la gauche
        if (yek->y == y_cible)
        {
            if (yek->x == 0)               // Tu étais sur la case qui sort à gauche
                yek->x = laby->sizeX - 1;  // Tu réapparais sur la case insérée à droite
            else
                yek->x--;                  // Sinon, tu glisses vers la gauche
        }

        for (int x = 0; x < laby->sizeX - 1; x++)
            laby->laby_update[x][y_cible] = laby->laby_update[x + 1][y_cible];

        laby->laby_update[laby->sizeX - 1][y_cible] = laby->extra.presence_mur;
        laby->extra.presence_mur = temp;
    }
    else if (joueur_qui_joue->type_insertion == INSERT_COLONNE_HAUT)
    {
        int x_cible = joueur_qui_joue->indice;
        int temp = laby->laby_update[x_cible][laby->sizeY - 1];

        // Si TU es sur la colonne poussée vers le bas
        if (yek->x == x_cible)
        {
            if (yek->y == laby->sizeY - 1) // Tu étais sur la case qui sort en bas
                yek->y = 0;                // Tu réapparais sur la case insérée en haut
            else
                yek->y++;                  // Sinon, tu glisses vers le bas
        }

        for (int y = laby->sizeY - 1; y > 0; y--)
            laby->laby_update[x_cible][y] = laby->laby_update[x_cible][y - 1];

        laby->laby_update[x_cible][0] = laby->extra.presence_mur;
        laby->extra.presence_mur = temp;
    }
    else if (joueur_qui_joue->type_insertion == INSERT_COLONNE_BAS)
    {
        int x_cible = joueur_qui_joue->indice;
        int temp = laby->laby_update[x_cible][0];

        // Si TU es sur la colonne poussée vers le haut
        if (yek->x == x_cible)
        {
            if (yek->y == 0)               // Tu étais sur la case qui sort en haut
                yek->y = laby->sizeY - 1;  // Tu réapparais sur la case insérée en bas
            else
                yek->y--;                  // Sinon, tu glisses vers le haut
        }

        for (int y = 0; y < laby->sizeY - 1; y++)
            laby->laby_update[x_cible][y] = laby->laby_update[x_cible][y + 1];

        laby->laby_update[x_cible][laby->sizeY - 1] = laby->extra.presence_mur;
        laby->extra.presence_mur = temp;
    }
}
void copie_laby(t_laby *laby)
{
    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {

            laby->copy_laby_update[x][y] = laby->laby_update[x][y];
        }
    }
    laby->copy_extra = laby->extra;
}

// Renvoie les coordonnées de la case voisine à partir d'une case (x, y) et d'une direction d
// d = 0 : Nord, 1 : Est, 2 : Sud, 3 : Ouest
bool voisin_accessible(t_laby *laby, int x, int y, int dir, int *nx, int *ny)
{
    static const int dx[4] = {0, 1, 0, -1};
    static const int dy[4] = {-1, 0, 1, 0};

    // Détermine les coordonnées de la case voisine
    *nx = x + dx[dir];
    *ny = y + dy[dir];

    // Vérifie si on sort du labyrinthe
    if (*nx < 0 || *ny < 0 || *nx >= laby->sizeX || *ny >= laby->sizeY)
    {
        return false;
    }

    // // 1. Récupération et vérification des murs de la case actuelle
    int cell_actu = laby->copy_laby_update[x][y] & ~MASK_ITEM;
    int cell_voisine = laby->copy_laby_update[*nx][*ny] & ~MASK_ITEM;
    if (dir == NORD && (cell_actu & MUR_NORD))
        return false;
    if (dir == EST && (cell_actu & MUR_EST))
        return false;
    if (dir == SUD && (cell_actu & MUR_SUD))
        return false;
    if (dir == OUEST && (cell_actu & MUR_OUEST))
        return false;

    // // On vérifie le mur opposé sur la case voisine
    if (dir == NORD && (cell_voisine & MUR_SUD))
        return false;
    if (dir == EST && (cell_voisine & MUR_OUEST))
        return false;
    if (dir == SUD && (cell_voisine & MUR_NORD))
        return false;
    if (dir == OUEST && (cell_voisine & MUR_EST))
        return false;

    return true;
}

int phaseExpansion(t_laby *laby, t_joueur *yek, int target_x, int target_y)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    int coord_x_arrivee = target_x;
    int coord_y_arrivee = target_y;

    int tab[sizeX + 1][sizeY + 1];
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            tab[x][y] = 0;
        }
    }

    int startX = yek->x;
    int startY = yek->y;
    tab[startX][startY] = 1;

    int distance = 1;
    int nouvelles_cases_marquees = 1;
    while (tab[coord_x_arrivee][coord_y_arrivee] == 0 && nouvelles_cases_marquees)
    {
        nouvelles_cases_marquees = 0;

        for (int y = 0; y < sizeY; y++)
        {

            for (int x = 0; x < sizeX; x++)
            {
                if (tab[x][y] == distance)
                {
                    for (int dir = 0; dir < 4; dir++)
                    {
                        int nx, ny;
                        // voisin_accessible(laby, x, y, dir, &nx, &ny);
                        if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
                            continue;

                        if (tab[nx][ny] == 0)
                        {
                            tab[nx][ny] = distance + 1;
                            nouvelles_cases_marquees = 1;
                        }
                    }
                }
            }
        }
        distance++;
        // printf("distance %d \n", distance);
    }

    // Sauvegarde de la carte des distances
    for (int y = 0; y < sizeY; y++)
    {
        for (int x = 0; x < sizeX; x++)
        {
            laby->copy_laby_update[x][y] = tab[x][y];
        }
    }

    return tab[coord_x_arrivee][coord_y_arrivee] > 0;
}

void localiser_tresors(t_laby *laby_state, int *tresor_x, int *tresor_y, int mon_numero_joueur)
{
    for (int i = 0; i <= 25; i++) {
        tresor_x[i] = -1;
        tresor_y[i] = -1;
    }
    for (int y = 0; y < laby_state->sizeY; y++)
    {
        for (int x = 0; x < laby_state->sizeX; x++)
        {
            int tuile_cle = laby_state->copy_laby_update[x][y];
            int item_reel = tuile_cle & 0xFF;
            if (item_reel >= 1 && item_reel <= 24)
            {
                int item_virtuel;
                if (mon_numero_joueur == 0)
                    item_virtuel = item_reel;
                else
                    item_virtuel = 25 - item_reel;

                tresor_x[item_virtuel] = x;
                tresor_y[item_virtuel] = y;
            }
        }
    }
}

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
    // DEBUG début de tour
    printf("[DEBUG START] num_tresor=%d bonus_max=%d\n", tuiles_tresor.num_tresor, bonus_max);
    fflush(stdout);

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
                for (int y = 0; y < laby.sizeY; y++)
                {
                    for (int x = 0; x < laby.sizeX; x++)
                    {
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

                if (target_x != -1 && target_y != -1)
                {
                    // Restaurer la grille propre avant le BFS
                    for (int y = 0; y < laby.sizeY; y++)
                    {
                        for (int x = 0; x < laby.sizeX; x++)
                        {
                            laby.copy_laby_update[x][y] = grid_backup[x][y];
                        }
                    }

                    premier_atteint = phaseExpansion(&laby, &joueur_virtuel, target_x, target_y);
                    if (premier_atteint)
                    {
                        int path_len = phaseRemontee(&laby, &joueur_virtuel, target_x, target_y, chemin, 500);
                        if (path_len >= 0)
                        {
                            path_courant.x[path_courant.count] = target_x;
                            path_courant.y[path_courant.count] = target_y;
                            path_courant.count++;

                            current_target++;
                            while (current_target <= 24)
                            {
                                int next_x = tresor_x[current_target];
                                int next_y = tresor_y[current_target];
                                if (next_x == -1 || next_y == -1)
                                    break;

                                t_joueur pos_last;
                                pos_last.x = path_courant.x[path_courant.count - 1];
                                pos_last.y = path_courant.y[path_courant.count - 1];

                                // Restaurer la grille propre avant chaque nouveau BFS
                                for (int y = 0; y < laby.sizeY; y++)
                                {
                                    for (int x = 0; x < laby.sizeX; x++)
                                    {
                                        laby.copy_laby_update[x][y] = grid_backup[x][y];
                                    }
                                }

                                if (phaseExpansion(&laby, &pos_last, next_x, next_y))
                                {
                                    int next_len = phaseRemontee(&laby, &pos_last, next_x, next_y, chemin, 500);
                                    if (bonus_max > 0 && path_courant.total_bonus_dist + next_len <= bonus_max)
                                    {
                                        path_courant.x[path_courant.count] = next_x;
                                        path_courant.y[path_courant.count] = next_y;
                                        path_courant.count++;
                                        path_courant.total_bonus_dist += next_len;
                                        current_target++;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }

                            if (current_target <= 24)
                            {
                                int next_x = tresor_x[current_target];
                                int next_y = tresor_y[current_target];
                                if (next_x != -1 && next_y != -1)
                                {
                                    t_joueur pos_last;
                                    pos_last.x = path_courant.x[path_courant.count - 1];
                                    pos_last.y = path_courant.y[path_courant.count - 1];

                                    // Restaurer la grille propre avant le BFS de rapprochement
                                    for (int y = 0; y < laby.sizeY; y++)
                                    {
                                        for (int x = 0; x < laby.sizeX; x++)
                                        {
                                            laby.copy_laby_update[x][y] = grid_backup[x][y];
                                        }
                                    }

                                    phaseExpansion(&laby, &pos_last, next_x, next_y);

                                    int min_dist_manhattan = 9999;
                                    int best_cx = pos_last.x;
                                    int best_cy = pos_last.y;
                                    int remaining_steps = bonus_max - path_courant.total_bonus_dist;

                                    for (int y = 0; y < laby.sizeY; y++)
                                    {
                                        for (int x = 0; x < laby.sizeX; x++)
                                        {
                                            int val = laby.copy_laby_update[x][y];
                                            if (val > 0)
                                            {
                                                int steps = val - 1;
                                                if (steps <= remaining_steps)
                                                {
                                                    int dist = abs(x - next_x) + abs(y - next_y);
                                                    if (dist < min_dist_manhattan)
                                                    {
                                                        min_dist_manhattan = dist;
                                                        best_cx = x;
                                                        best_cy = y;
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if (best_cx != pos_last.x || best_cy != pos_last.y)
                                    {
                                        path_courant.x[path_courant.count] = best_cx;
                                        path_courant.y[path_courant.count] = best_cy;
                                        path_courant.count++;
                                        path_courant.total_bonus_dist += (laby.copy_laby_update[best_cx][best_cy] - 1);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // Restaurer la grille propre avant le BFS de rapprochement initial
                        for (int y = 0; y < laby.sizeY; y++)
                        {
                            for (int x = 0; x < laby.sizeX; x++)
                            {
                                laby.copy_laby_update[x][y] = grid_backup[x][y];
                            }
                        }

                        phaseExpansion(&laby, &joueur_virtuel, target_x, target_y);
                        int min_dist_manhattan = 9999;
                        int best_cx = joueur_virtuel.x;
                        int best_cy = joueur_virtuel.y;

                        for (int y = 0; y < laby.sizeY; y++)
                        {
                            for (int x = 0; x < laby.sizeX; x++)
                            {
                                if (laby.copy_laby_update[x][y] > 0)
                                {
                                    int dist = abs(x - target_x) + abs(y - target_y);
                                    if (dist < min_dist_manhattan)
                                    {
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
                if (premier_atteint)
                {
                    treasures_collected = current_target - tuiles_tresor.num_tresor;
                    int dist_to_next = 0;
                    if (bonus_max > 0 && current_target <= 24)
                    {
                        int final_x = path_courant.x[path_courant.count - 1];
                        int final_y = path_courant.y[path_courant.count - 1];
                        int next_x = tresor_x[current_target];
                        int next_y = tresor_y[current_target];
                        dist_to_next = abs(final_x - next_x) + abs(final_y - next_y);
                    }
                    score_courant = -(treasures_collected * 10000) + dist_to_next;
                }
                else
                {
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
    // DEBUG
    printf("[DEBUG END] num_tresor=%d bonus_max=%d collected=%d\n",
           tuiles_tresor.num_tresor, bonus_max, meilleur_treasures_collected);
    fflush(stdout);
}

