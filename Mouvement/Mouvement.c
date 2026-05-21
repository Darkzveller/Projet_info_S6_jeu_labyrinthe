#include "Mouvement.h"

void position_tresor(t_laby *laby, t_tuiles *tuiles)
{
    char *ptr = laby->labyData;
    int nord = 0;
    int est = 0;
    int sud = 0;
    int ouest = 0;
    int item = 0;
    int offset = 0;

    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            if (sscanf(ptr, "%d %d %d %d %d%n", &nord, &est, &sud, &ouest, &item, &offset) >= 5)
            {

                if (item >= 1)
                {
                    tuiles->x[item] = x + 1;
                    tuiles->y[item] = y + 1;
                    tuiles->presence_mur[item] = (nord << 3) | (sud << 2) | (ouest << 1) | (est << 0);
                }

                ptr += offset;
            }
        }
    }
    tuiles->num_tresor = 1;
#if DEBUG_POS_TUILES
    for (int i = 1; i < NBR_TUILES; i++)
    {
        printf("item %2d  px: %2d py: %2d", i, tuiles->x[i], tuiles->y[i]);
        printf("\n");
    }
#endif
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
            if ((x == (laby->sizeX) - 1) && (y == (laby->sizeY - 1)))
            {
                laby->extra.presence_mur = (nord << SHIFT_BIT_NORD) |
                                           (est << SHIFT_BIT_EST) |
                                           (sud << SHIFT_BIT_SUD) |
                                           (ouest << SHIFT_BIT_OUEST) |
                                           (item);
                laby->copy_extra.presence_mur = (nord << SHIFT_BIT_NORD) |
                                           (est << SHIFT_BIT_EST) |
                                           (sud << SHIFT_BIT_SUD) |
                                           (ouest << SHIFT_BIT_OUEST) |
                                           (item);
            }
        }
    }
}

void update_laby(t_laby *laby, t_joueur *adversaire)
{
    int var_temp = 0;
    if (adversaire->rotation != 0)
    {
        int nord = (laby->copy_extra.presence_mur >> SHIFT_BIT_NORD) & 1;
        int est =  (laby->copy_extra.presence_mur >> SHIFT_BIT_EST) & 1;
        int sud =  (laby->copy_extra.presence_mur >> SHIFT_BIT_SUD) & 1;
        int ouest = (laby->copy_extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
        int item =  (laby->copy_extra.presence_mur >> 0) & 0xFF;

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
        laby->copy_extra.presence_mur =
            (nord << SHIFT_BIT_NORD) |
            (est << SHIFT_BIT_EST) |
            (sud << SHIFT_BIT_SUD) |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
    }

    if (adversaire->type_insertion == INSERT_LIGNE_DROITE)
    {

        int y = adversaire->indice;

        // sauvegarde de la tuile expulsée
        int temp = laby->laby_update[y][0];

        // décalage gauche -> droite
        for (int x = 0; x < laby->sizeX - 1; x++)
        {
            laby->laby_update[y][x] =
                laby->laby_update[y][x + 1];
        }

        // insertion de la tuile externe
        laby->laby_update[y][laby->sizeX - 1] =
            laby->extra.presence_mur;

        // la tuile expulsée devient la nouvelle tuile externe
        laby->extra.presence_mur = temp;
    }
    else if (adversaire->type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y = adversaire->indice;

        // sauvegarde tuile expulsée
        int temp = laby->laby_update[y][laby->sizeX - 1];

        // décalage droite -> gauche
        for (int x = laby->sizeX - 1; x > 0; x--)
        {
            laby->laby_update[y][x] =
                laby->laby_update[y][x - 1];
        }

        // insertion de la tuile externe
        laby->laby_update[y][0] =
            laby->extra.presence_mur;

        // mise à jour de la tuile externe
        laby->extra.presence_mur = temp;
    }
    else if (adversaire->type_insertion == INSERT_COLONNE_HAUT)
    {
        int x = adversaire->indice;

        // sauvegarde de la tuile expulsée (en bas)
        int temp = laby->laby_update[laby->sizeY - 1][x];

        // décalage bas <- haut
        for (int y = laby->sizeY - 1; y > 0; y--)
        {
            laby->laby_update[y][x] =
                laby->laby_update[y - 1][x];
        }

        // insertion de la tuile externe en haut
        laby->laby_update[0][x] =
            laby->extra.presence_mur;

        // la tuile expulsée devient la nouvelle tuile externe
        laby->extra.presence_mur = temp;
    }
    else if (adversaire->type_insertion == INSERT_COLONNE_BAS)
    {
        int x = adversaire->indice;

        // sauvegarde de la tuile expulsée (en haut)
        int temp = laby->laby_update[0][x];

        // décalage haut <- bas
        for (int y = 0; y < laby->sizeY - 1; y++)
        {
            laby->laby_update[y][x] =
                laby->laby_update[y + 1][x];
        }

        // insertion de la tuile externe en bas
        laby->laby_update[laby->sizeY - 1][x] =
            laby->extra.presence_mur;

        // mise à jour de la tuile externe
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
    int cell_actu = laby->copy_laby_update[x][y]; // ou laby_update selon votre structure
    if (dir == NORD && (cell_actu & MUR_NORD))
        return false;
    if (dir == EST && (cell_actu & MUR_EST))
        return false;
    if (dir == SUD && (cell_actu & MUR_SUD))
        return false;
    if (dir == OUEST && (cell_actu & MUR_OUEST))
        return false;

    // // Vérification des murs de la case VOISINE
    int cell_voisine = laby->copy_laby_update[*nx][*ny];
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

        for (int y = 0; y < sizeY - 1; y++)
        {

            for (int x = 0; x < sizeX - 1; x++)
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
