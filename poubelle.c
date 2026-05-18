// int phaseExpansion(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor)
// {
//     int sizeX = laby->sizeX;
//     int sizeY = laby->sizeY;

//     int coord_x_arrivee = tuiles_tresor->x[tuiles_tresor->num_tresor];
//     int coord_y_arrivee = tuiles_tresor->y[tuiles_tresor->num_tresor];
//     copie_laby(&laby);

//     int tab[sizeX][sizeY];

//     for (int y = 0; y < sizeY; y++)
//     {
//         for (int x = 0; x < sizeX; x++)
//         {
//             tab[x][y] = 0;
//         }
//     }

//     // directions
//     static const int dx[4] = {0, 1, 0, -1};
//     static const int dy[4] = {-1, 0, 1, 0};

//     // position de départ
//     int startX = yek->x;
//     int startY = yek->y;

//     tab[startX][startY] = 1;

//     int distance = 1;
//     int changed = 1;

//     while (!tab[coord_x_arrivee][coord_y_arrivee] && changed)
//     {
//         changed = 0;

//         for (int y = 0; y < sizeY - 1; y++)
//         {
//             for (int x = 0; x < sizeX - 1; x++)
//             {
//                 /*
//                 if (tab[x][y] == distance)
//                 {
//                     for (int dir = 0; dir < 4; dir++)
//                     {
//                         int nx = 0;
//                         int ny = 0;

//                         if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
//                         {
//                             continue;
//                         }

//                         if (tab[nx][ny] == 0)
//                         {
//                             tab[nx][ny] = distance + 1;
//                             changed = 1;
//                         }
//                     }
//                 }
//             */ for (int dir = 0; dir < 4; dir++)
//                 {

//                     if (tab[x][y] == distance)
//                     {
                        
//                         int nx = x;
//                         int ny = y;

//                         if (!voisin_accessible(laby, x, y, dir, &nx, &ny))
//                             continue;

//                         if (tab[nx][ny] == 0)
//                         {
//                             tab[nx][ny] = distance + 1;
//                             changed = 1;
//                         }
//                     }
//                 }
//             }
//         }

//         distance++;
//     }

//     for (int y = 0; y < laby->sizeY; y++)
//     {
//         for (int x = 0; x < laby->sizeX; x++)
//         {

//             laby->copy_laby_update[x][y] = tab[x][y];
//         }
//     }
//     return tab[coord_x_arrivee][coord_y_arrivee] > 0;
// }

// int phaseRemontee(t_laby *laby, t_joueur *yek, t_tuiles *tuiles_tresor, int *chemin, int max_chemin)
// {
//     int sizeX = laby->sizeX;
//     int sizeY = laby->sizeY;

//     int destX = tuiles_tresor->x[tuiles_tresor->num_tresor];
//     int destY = tuiles_tresor->y[tuiles_tresor->num_tresor];
//     printf("START = %d\n", laby->copy_laby_update[yek->x][yek->y]);
//     printf("DEST = %d\n", laby->copy_laby_update[destX][destY]);

//     int x = destX;
//     int y = destY;

//     int idx = 0;

//     // directions
//     static const int dx[4] = {0, 1, 0, -1};
//     static const int dy[4] = {-1, 0, 1, 0};

//     // sécurité : si arrivée inaccessible
//     if (laby->laby_update[x][y] == 0)
//         return 0;

//     while (!(x == yek->x && y == yek->y))
//     {
//         int current_value = laby->laby_update[x][y];

//         int found = 0;

//         // chercher un voisin avec distance - 1
//         for (int dir = 0; dir < 4; dir++)
//         {
//             int nx = x + dx[dir];
//             int ny = y + dy[dir];

//             if (nx < 0 || ny < 0 || nx >= sizeX || ny >= sizeY)
//                 continue;

//             if (!voisin_accessible(laby, nx, ny, (dir + 2) % 4, &nx, &ny))
//                 continue;

//             if (laby->laby_update[nx][ny] == current_value - 1)
//             {
//                 // on stocke le mouvement (direction inverse)
//                 chemin[idx++] = dir;

//                 x = nx;
//                 y = ny;
//                 found = 1;
//                 break;
//             }
//         }

//         if (!found)
//             return 0; // échec remontée
//     }

//     // inverser le chemin (car construit de la fin vers le début)
//     for (int i = 0; i < idx / 2; i++)
//     {
//         int tmp = chemin[i];
//         chemin[i] = chemin[idx - 1 - i];
//         chemin[idx - 1 - i] = tmp;
//     }

//     return idx; // longueur du chemin
// }

// bool voisin_accessible(t_laby *laby, int x, int y, int dir, int *nx, int *ny)
// {
//     /*
//         Table de déplacement des directions dans le labyrinthe.

//         On utilise un système de direction codé ainsi :
//             NORD  = 0
//             EST   = 1
//             SUD   = 2
//             OUEST = 3

//         Ces valeurs servent d'index dans les tableaux dx et dy.

//         dx et dy permettent de convertir une direction en déplacement
//         sur une grille 2D :

//             - dx = variation sur l'axe X (horizontal)
//             - dy = variation sur l'axe Y (vertical)

//         Convention utilisée ici :
//             - X augmente vers la droite
//             - Y augmente vers le bas

//         Donc :

//             NORD  (0) -> on monte       -> x + 0, y - 1
//             EST   (1) -> on va à droite -> x + 1, y + 0
//             SUD   (2) -> on descend     -> x + 0, y + 1
//             OUEST (3) -> on va à gauche -> x - 1, y + 0

//     */
//     static const int dx[4] = {0, 1, 0, -1};
//     static const int dy[4] = {-1, 0, 1, 0};
//     // Determine la coordonner de la case voisine
//     *nx = x + dx[dir];
//     *ny = y + dy[dir];
//     // Vérifie si on sort du labyrinthe
//     if (*nx < 0 || *ny < 0 || *nx >= laby->sizeX || *ny >= laby->sizeY)
//     {
//         return false;
//     }
//     // récupération de la case actuelle
//     int cell_actu = laby->copy_laby_update[x][y];

//     // Vérifie les murs
//     if (dir == NORD && (cell_actu & MUR_NORD))
//     {
//         return false;
//     }
//     if (dir == EST && (cell_actu & MUR_EST))
//     {
//         return false;
//     }
//     if (dir == SUD && (cell_actu & MUR_SUD))
//     {
//         return false;
//     }
//     if (dir == OUEST && (cell_actu & MUR_OUEST))
//     {
//         return false;
//     }
//     return true;
// }