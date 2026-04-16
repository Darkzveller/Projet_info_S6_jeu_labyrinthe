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

                if (item > 1)
                {
                    tuiles->x[item] = x;
                    tuiles->y[item] = y;
                    tuiles->presence_mur[item] = (nord << 3) | (sud << 2) | (ouest << 1) | (est << 0);
                }

                ptr += offset;
            }
        }
    }
#if DEBUG_POS_TUILES
    for (int i = 1; i < NBR_TUILES; i++)
    {
        printf("item %2d px : %2d py %2d", i, tuiles->x[i], tuiles->y[i]);
        printf("\n");
    }
#endif
}

void transfer_labydata_to_laby_update(t_laby *laby, bool activate_print)
{
    char *ptr = laby->labyData;
    int nord = 0;
    int est = 0;
    int sud = 0;
    int ouest = 0;
    int item = 0;
    int offset = 0;

    laby->laby_update = (int **)malloc(((laby->sizeX * laby->sizeY + 5) * 11) * sizeof(int *));

    for (int i = 0; i < ((laby->sizeX * laby->sizeY + 5) * 11); i++)
    {
        laby->laby_update[i] = malloc(((laby->sizeX * laby->sizeY + 5) * 11) * sizeof(int));
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
    if (activate_print == true)
    {
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
                if ((x == (laby->sizeX)-1) && (y == (laby->sizeY-1)))
                {
                printf("\nLa tuile supplementaire est celle ci : %d%d%d%d %d ", nord, est, sud, ouest, item);

                }
                else
                {
                    printf("%d%d%d%d %d ", nord, est, sud, ouest, item);
                }
            }
            printf("\n");
        }
    }
}

// Renvoie les coordonnées de la case voisine à partir d'une case (x, y) et d'une direction d
// d = 0 : Nord, 1 : Est, 2 : Sud, 3 : Ouest
//  void coord_case_voisine(int coord_x_actu, int coord_y_actu, int direction_voulue, int *coord_x_voisine, int *coord_y_voisine)
#define NORD 0
#define EST 1
#define SUD 2
#define OUEST 3
void coord_case_voisine(int coord_x_actu, int coord_y_actu, int direction_voulue, int *coord_x_voisine, int *coord_y_voisine)
{
    if (direction_voulue == NORD)
    {
        // Nort
        *coord_x_voisine = coord_x_actu;
        *coord_y_voisine = coord_y_actu - 1;
    }
    else if (direction_voulue == EST)
    {
        // Est
        *coord_x_voisine = coord_x_actu + 1;
        *coord_y_voisine = coord_y_actu;
    }
    else if (direction_voulue == SUD)
    {
        // Sud
        *coord_x_voisine = coord_x_actu;
        *coord_y_voisine = coord_y_actu + 1;
    }
    else if (direction_voulue == OUEST)
    {
        // Ouest
        *coord_x_voisine = coord_x_actu - 1;
        *coord_y_voisine = coord_y_actu;
    }
    else
    {
        // Au cas ou, mauvaise détection
        *coord_x_voisine = coord_x_actu;
        *coord_y_voisine = coord_y_actu;
    }
}

/*
  Question 5 :

  Si on cherche à utiliser la fonction `coord_case_voisine` pour une case
  située sur le bord du tableau (coordonnées x=0, x=TAILLE_X-1, y=0, y=TAILLE_Y-1),
  certaines directions pointeraient **horss du tableau** :

  Cela provoquerait un accès mémoire invalide si on essayait de lire/écrire
  dans le tableau avec ces coordonnées.

  Pour se prémunir de ce problème, on considère que toutes les cases de la périphérie du labyrinthe sont bloquées,
  et donc on n'appellera pas la fonction coord_case_voisine depuis une case
  située sur la bordure.
*/

// Renvoie true ssi un chemin existe, false sinon
// int phaseExpansion(int tab_representative_labyrinthe[TAILLE_X][TAILLE_Y], int coord_x_depart, int coord_y_depart, int coord_x_arrivee, int coord_y_arrivee)

int phaseExpansion(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor)
{
    int distance_parcourue = 1; // distance initiale
    int nouvelles_cases_marquees = 1;
    int coord_x_arrivee = tuiles_tresor->x[tuiles_tresor->num_tresor];
    int coord_y_arrivee = tuiles_tresor->y[tuiles_tresor->num_tresor];

    int tab_representative_labyrinthe[laby->sizeX][laby->sizeY];

    tab_representative_labyrinthe[yek->x][yek->y] = distance_parcourue;

    while (tab_representative_labyrinthe[coord_x_arrivee][coord_y_arrivee] == 0 && nouvelles_cases_marquees)
    {
        nouvelles_cases_marquees = 0;

        // Parcours de toutes les cases (ignore les bordures)
        for (int y = 1; y < laby->sizeY - 1; y++)
        {
            for (int x = 1; x < laby->sizeX - 1; x++)
            {

                if (tab_representative_labyrinthe[x][y] == distance_parcourue)
                {
                    // Vérifie les 4 voisins
                    for (int d = 0; d < 4; d++)
                    {
                        int nx, ny;
                        coord_case_voisine(x, y, d, &nx, &ny);

                        if (tab_representative_labyrinthe[nx][ny] == 0)
                        { // case libre non visitée
                            tab_representative_labyrinthe[nx][ny] = distance_parcourue + 1;
                            nouvelles_cases_marquees = 1;
                        }
                    }
                }
            }
        }

        distance_parcourue++;
    }

    // Renvoie true si l'arrivée a été atteinte, false sinon
    return tab_representative_labyrinthe[coord_x_arrivee][coord_y_arrivee] > 0;
}

// Renvoie true si le chemin a été correctement remonté, false sinon
// int phaseRemontee(int lab[TAILLE_X][TAILLE_Y], int coord_x_depart, int coord_y_depart, int coord_x_arrivee, int coord_y_arrivee)

int phaseRemontee(t_laby *laby, t_tuiles *tuiles_tresor, t_joueur *yek)
{
    int coord_x_depart = yek->x;
    int coord_y_depart = yek->y;
    int coord_x_arrivee = tuiles_tresor->x[tuiles_tresor->num_tresor];
    int coord_y_arrivee = tuiles_tresor->y[tuiles_tresor->num_tresor];

    int lab[laby->sizeX][laby->sizeY];
    int x_courant = coord_x_arrivee;
    int y_courant = coord_y_arrivee;
    int distance_actuelle = lab[x_courant][y_courant];

    if (distance_actuelle <= 0)
    {
        // L'arrivée n'a pas été atteinte pendant l'expansion
        return false;
    }

    // Marque la case d'arrivée comme faisant partie du chemin
    lab[x_courant][y_courant] = tuiles_tresor->num_tresor;

    while (!(x_courant == coord_x_depart && y_courant == coord_y_depart))
    {
        bool voisin_trouve = false;

        // Vérifie les 4 voisins
        for (int d = 0; d < 4; d++)
        {
            int nx, ny;
            coord_case_voisine(x_courant, y_courant, d, &nx, &ny);

            // Si voisin a la distance précédente
            if (lab[nx][ny] == distance_actuelle - 1)
            {
                // Marque le voisin comme faisant partie du chemin
                lab[nx][ny] = -2;

                // Déplace la case courante
                x_courant = nx;
                y_courant = ny;
                distance_actuelle--;
                voisin_trouve = true;
                break;
            }
        }

        if (!voisin_trouve)
        {
            // Si aucun voisin à distance r-1 n'a été trouvé, la remontée échoue
            return false;
        }
    }

    return true;
}