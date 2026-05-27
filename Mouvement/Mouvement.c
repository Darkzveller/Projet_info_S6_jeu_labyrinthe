#include "Mouvement.h"

void position_tresor(t_laby *laby, t_tuiles *tuiles)
{
    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            int tuile_cle = laby->laby_update[x][y];
            int item = tuile_cle & 0xFF;
            if (item >= 1)
            {
                tuiles->x[item] = x;
                tuiles->y[item] = y;
                tuiles->presence_mur[item] = tuile_cle;
            }
        }
    }
}

void print_laby(t_laby *laby, bool activate)
{
    if (!activate) return;
    printf("Laby data que j'obtiens\n");
    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            int nord  = (laby->laby_update[x][y] >> SHIFT_BIT_NORD)  & 1;
            int est   = (laby->laby_update[x][y] >> SHIFT_BIT_EST)   & 1;
            int sud   = (laby->laby_update[x][y] >> SHIFT_BIT_SUD)   & 1;
            int ouest = (laby->laby_update[x][y] >> SHIFT_BIT_OUEST) & 1;
            int item  = (laby->laby_update[x][y] >> 0) & 0xFF;

            if ((x == (laby->sizeX) - 1) && (y == (laby->sizeY - 1)))
            {
                int nord_extra  = (laby->extra.presence_mur >> SHIFT_BIT_NORD)  & 1;
                int est_extra   = (laby->extra.presence_mur >> SHIFT_BIT_EST)   & 1;
                int sud_extra   = (laby->extra.presence_mur >> SHIFT_BIT_SUD)   & 1;
                int ouest_extra = (laby->extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
                int item_extra  = (laby->extra.presence_mur >> 0) & 0xFF;
                printf("\nLa tuile supplementaire est celle ci : %d%d%d%d %d ",
                       nord_extra, est_extra, sud_extra, ouest_extra, item_extra);
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
    int nord = 0, est = 0, sud = 0, ouest = 0, item = 0, offset = 0;

    int alloc_size = (laby->sizeX * laby->sizeY + 5) * 11;

    laby->laby_update      = (int **)malloc(alloc_size * sizeof(int *));
    laby->copy_laby_update = (int **)malloc(alloc_size * sizeof(int *));
    laby->dist             = (int **)malloc(alloc_size * sizeof(int *));

    for (int i = 0; i < alloc_size; i++)
    {
        laby->laby_update[i]      = malloc(alloc_size * sizeof(int));
        laby->copy_laby_update[i] = malloc(alloc_size * sizeof(int));
        laby->dist[i]             = malloc(alloc_size * sizeof(int));
    }

    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            if (sscanf(ptr, "%d %d %d %d %d%n", &nord, &est, &sud, &ouest, &item, &offset) >= 5)
            {
                laby->laby_update[x][y] =
                    (nord  << SHIFT_BIT_NORD)  |
                    (est   << SHIFT_BIT_EST)   |
                    (sud   << SHIFT_BIT_SUD)   |
                    (ouest << SHIFT_BIT_OUEST) |
                    (item);
                ptr += offset;
            }
        }
    }

    if (sscanf(ptr, "%d %d %d %d %d%n", &nord, &est, &sud, &ouest, &item, &offset) >= 5)
    {
        laby->extra.presence_mur =
            (nord  << SHIFT_BIT_NORD)  |
            (est   << SHIFT_BIT_EST)   |
            (sud   << SHIFT_BIT_SUD)   |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
        laby->copy_extra.presence_mur = laby->extra.presence_mur;
    }
}

/* ------------------------------------------------------------------ */
/*  update_labyV2 : applique le coup de joueur_qui_joue sur laby_update
 *  et met à jour la position physique de yek si il est poussé.
 *  IMPORTANT : yek->x/y doit contenir la VRAIE position physique du
 *  joueur AVANT l'appel (pas la destination calculée).               */
/* ------------------------------------------------------------------ */
void update_labyV2(t_laby *laby, t_joueur *joueur_qui_joue, t_joueur *yek)
{
    /* 1. Rotation de la tuile extra */
    if (joueur_qui_joue->rotation != 0)
    {
        int nord  = (laby->extra.presence_mur >> SHIFT_BIT_NORD)  & 1;
        int est   = (laby->extra.presence_mur >> SHIFT_BIT_EST)   & 1;
        int sud   = (laby->extra.presence_mur >> SHIFT_BIT_SUD)   & 1;
        int ouest = (laby->extra.presence_mur >> SHIFT_BIT_OUEST) & 1;
        int item  = (laby->extra.presence_mur >> 0) & 0xFF;

        for (int i = 0; i < joueur_qui_joue->rotation; i++)
        {
            int n = nord, e = est, s = sud, o = ouest;
            nord = o; est = n; sud = e; ouest = s;
        }
        laby->extra.presence_mur =
            (nord  << SHIFT_BIT_NORD)  |
            (est   << SHIFT_BIT_EST)   |
            (sud   << SHIFT_BIT_SUD)   |
            (ouest << SHIFT_BIT_OUEST) |
            (item);
    }

    /* 2. Décalage + mise à jour position yek */
    if (joueur_qui_joue->type_insertion == INSERT_LIGNE_GAUCHE)
    {
        int y_cible = joueur_qui_joue->indice;
        int temp = laby->laby_update[laby->sizeX - 1][y_cible];

        if (yek->y == y_cible)
        {
            if (yek->x == laby->sizeX - 1) yek->x = 0;
            else                            yek->x++;
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

        if (yek->y == y_cible)
        {
            if (yek->x == 0) yek->x = laby->sizeX - 1;
            else             yek->x--;
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

        if (yek->x == x_cible)
        {
            if (yek->y == laby->sizeY - 1) yek->y = 0;
            else                            yek->y++;
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

        if (yek->x == x_cible)
        {
            if (yek->y == 0) yek->y = laby->sizeY - 1;
            else             yek->y--;
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
        for (int x = 0; x < laby->sizeX; x++)
            laby->copy_laby_update[x][y] = laby->laby_update[x][y];
    laby->copy_extra = laby->extra;
}

/* ------------------------------------------------------------------ */
/*  voisin_accessible : lit les MURS dans copy_laby_update            */
/* ------------------------------------------------------------------ */
bool voisin_accessible(t_laby *laby, int x, int y, int dir, int *nx, int *ny)
{
    static const int dx[4] = {0, 1, 0, -1};
    static const int dy[4] = {-1, 0, 1, 0};

    *nx = x + dx[dir];
    *ny = y + dy[dir];

    if (*nx < 0 || *ny < 0 || *nx >= laby->sizeX || *ny >= laby->sizeY)
        return false;

    int cell_actu   = laby->copy_laby_update[x][y]     & ~MASK_ITEM;
    int cell_voisine = laby->copy_laby_update[*nx][*ny] & ~MASK_ITEM;

    if (dir == NORD  && (cell_actu    & MUR_NORD))  return false;
    if (dir == EST   && (cell_actu    & MUR_EST))   return false;
    if (dir == SUD   && (cell_actu    & MUR_SUD))   return false;
    if (dir == OUEST && (cell_actu    & MUR_OUEST)) return false;

    if (dir == NORD  && (cell_voisine & MUR_SUD))   return false;
    if (dir == EST   && (cell_voisine & MUR_OUEST)) return false;
    if (dir == SUD   && (cell_voisine & MUR_NORD))  return false;
    if (dir == OUEST && (cell_voisine & MUR_EST))   return false;

    return true;
}

/* ------------------------------------------------------------------ */
/*  phaseExpansion : BFS depuis yek, résultat dans laby->dist         */
/*  copy_laby_update doit contenir les TUILES (murs) avant l'appel.   */
/* ------------------------------------------------------------------ */
int phaseExpansion(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor)
{
    int sizeX = laby->sizeX;
    int sizeY = laby->sizeY;

    int coord_x_arrivee = tuiles_tresor->x[tuiles_tresor->num_tresor];
    int coord_y_arrivee = tuiles_tresor->y[tuiles_tresor->num_tresor];

    int tab[sizeX][sizeY];
    for (int y = 0; y < sizeY; y++)
        for (int x = 0; x < sizeX; x++)
            tab[x][y] = 0;

    tab[yek->x][yek->y] = 1;

    int distance = 1;
    int nouvelles_cases = 1;
    while (tab[coord_x_arrivee][coord_y_arrivee] == 0 && nouvelles_cases)
    {
        nouvelles_cases = 0;
        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; x++)
            {
                if (tab[x][y] == distance)
                {
                    for (int dir = 0; dir < 4; dir++)
                    {
                        int nx, ny;
                        if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
                            continue;
                        if (tab[nx][ny] == 0)
                        {
                            tab[nx][ny] = distance + 1;
                            nouvelles_cases = 1;
                        }
                    }
                }
            }
        }
        distance++;
    }

    /* Sauvegarde dans laby->dist (séparé de copy_laby_update) */
    for (int y = 0; y < sizeY; y++)
        for (int x = 0; x < sizeX; x++)
            laby->dist[x][y] = tab[x][y];

    return tab[coord_x_arrivee][coord_y_arrivee] > 0;
}