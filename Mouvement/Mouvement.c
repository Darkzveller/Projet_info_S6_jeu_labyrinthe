#include "Mouvement.h"

void position_tresor(t_laby *laby, t_tuiles *tuiles)
{
    // On parcourt la matrice réelle [x][y] mise à jour
    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            // 1. On récupère la valeur binaire de la tuile actuelle
            int tuile_cle = laby->laby_update[x][y];

            // 2. On extrait l'ID de l'item (les 8 bits de poids faible)
            int item = tuile_cle & 0xFF;

            // 3. Si un trésor (item >= 1) est présent sur cette tuile
            if (item >= 1)
            {
                tuiles->x[item] = x;
                tuiles->y[item] = y;

                // On garde l'information des murs de la tuile au cas où
                tuiles->presence_mur[item] = tuile_cle;
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

void update_laby(t_laby *laby, t_joueur *adversaire)
{
    // 1. Gestion de la rotation
    if (adversaire->rotation != 0)
    {
        int nord = (laby->extra.presence_mur >> SHIFT_BIT_NORD) & 1;
        int est = (laby->extra.presence_mur >> SHIFT_BIT_EST) & 1;
        int sud = (laby->extra.presence_mur >> SHIFT_BIT_SUD) & 1;
        int ouest = (laby->extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
        int item = (laby->extra.presence_mur >> 0) & 0xFF;

        for (int i = 0; i < adversaire->rotation; i++)
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
        laby->extra.presence_mur =
            (nord << SHIFT_BIT_NORD) |
            (est << SHIFT_BIT_EST) |
            (sud << SHIFT_BIT_SUD) |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
    }

    // 2. Décalages et insertions réelles (Correction [x][y])
    if (adversaire->type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y = adversaire->indice;

        // Sauvegarde de la tuile expulsée à DROITE (X maximum)
        int temp = laby->laby_update[laby->sizeX - 1][y];

        // Décalage de la gauche vers la droite
        for (int x = laby->sizeX - 1; x > 0; x--)
        {
            laby->laby_update[x][y] = laby->laby_update[x - 1][y];
        }

        // Insertion de la tuile externe à GAUCHE (X = 0)
        laby->laby_update[0][y] = laby->extra.presence_mur;

        laby->extra.presence_mur = temp;
    }
    else if (adversaire->type_insertion == INSERT_LIGNE_DROITE)
    {
        int y = adversaire->indice;

        // Sauvegarde de la tuile expulsée à GAUCHE (X = 0)
        int temp = laby->laby_update[0][y];

        // Décalage de la droite vers la gauche
        for (int x = 0; x < laby->sizeX - 1; x++)
        {
            laby->laby_update[x][y] = laby->laby_update[x + 1][y];
        }

        // Insertion de la tuile externe à DROITE (X maximum)
        laby->laby_update[laby->sizeX - 1][y] = laby->extra.presence_mur;

        laby->extra.presence_mur = temp;
    }
    else if (adversaire->type_insertion == INSERT_COLONNE_HAUT)
    {
        int x = adversaire->indice;

        // Sauvegarde de la tuile expulsée en BAS (Y maximum)
        int temp = laby->laby_update[x][laby->sizeY - 1];

        // Décalage du haut vers le bas
        for (int y = laby->sizeY - 1; y > 0; y--)
        {
            laby->laby_update[x][y] = laby->laby_update[x][y - 1];
        }

        // Insertion de la tuile externe en HAUT (Y = 0)
        laby->laby_update[x][0] = laby->extra.presence_mur;

        laby->extra.presence_mur = temp;
    }
    else if (adversaire->type_insertion == INSERT_COLONNE_BAS)
    {
        int x = adversaire->indice;

        // Sauvegarde de la tuile expulsée en HAUT (Y = 0)
        int temp = laby->laby_update[x][0];

        // Décalage du bas vers le haut
        for (int y = 0; y < laby->sizeY - 1; y++)
        {
            laby->laby_update[x][y] = laby->laby_update[x][y + 1];
        }

        // Insertion de la tuile externe en BAS (Y maximum)
        laby->laby_update[x][laby->sizeY - 1] = laby->extra.presence_mur;

        laby->extra.presence_mur = temp;
    }
}

void update_labyV2(t_laby *laby, t_joueur *joueur_qui_joue, t_joueur *yek)
{
    // 1. Gestion de la rotation de l'extra
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
            yek->x = (yek->x + 1) % laby->sizeX;

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
            yek->x = (yek->x - 1 + laby->sizeX) % laby->sizeX;

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
            yek->y = (yek->y + 1) % laby->sizeY;

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
            yek->y = (yek->y - 1 + laby->sizeY) % laby->sizeY;

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

int phaseExpansion(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    int coord_x_arrivee = tuiles_tresor->x[tuiles_tresor->num_tresor];
    int coord_y_arrivee = tuiles_tresor->y[tuiles_tresor->num_tresor];

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
