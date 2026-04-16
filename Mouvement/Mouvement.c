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
/*


int main(void) {

    int tab_representative_labyrinthe[TAILLE_X][TAILLE_Y];

    printf("Caca test print\n");

    //pré-charger le labyrinthe depuis le fichier
    if (!build_labyrinthe("laby2.txt", tab_representative_labyrinthe)) {
        printf("Error chargement fichier\n");
        return 1;
    }

    //Initialisation affichage 
    if (!initAffichage()) {
        printf("Error initialisation de l'affichage\n");
        return 1;
    }

    // Coordonnées de départ et d'arrivée qu'on archive
    int couple_cordonne_depart[2] = {coord_x_depart, coord_y_depart};
    int couple_cordonne_arrivee[2] = {coord_x_arrivee, coord_y_arrivee};

    // Phase d'expansion
    bool chemin_existe = phaseExpansion(tab_representative_labyrinthe, coord_x_depart, coord_y_depart, coord_x_arrivee, coord_y_arrivee);

    if (chemin_existe) {
        printf("Chemin trouvé !!! B)\n");
    } else {
        printf("Pas de chemin possible entre départ et arrivée\n");
    }

    // Phase de remontée
    bool remontée_ok = phaseRemontee(tab_representative_labyrinthe, coord_x_depart, coord_y_depart, coord_x_arrivee, coord_y_arrivee);
    if (!remontée_ok) {
        printf("Erreur lors de la remontée du chemin\n");
    } else {
        printf("Chemin correctement tracé dans le labyrinthe\n");
    }

    // Nettoie le labyrinthe
    nettoyerLabyrinthe(tab_representative_labyrinthe);

    // Afficher le labyrinthe (avec le chemin marqué)
    afficheLabyrinthe(tab_representative_labyrinthe, (int[2]){coord_x_depart, coord_y_depart},(int[2]){coord_x_arrivee, coord_y_arrivee}, 0);

    return 0;
}
*/

// Renvoie les coordonnées de la case voisine à partir d'une case (x, y) et d'une direction d
// d = 0 : Nord, 1 : Est, 2 : Sud, 3 : Ouest
 void coord_case_voisine(int coord_x_actu, int coord_y_actu, int direction_voulue, int *coord_x_voisine, int *coord_y_voisine)
{
    if (direction_voulue == 0) 
    {  
        // Nort
        *coord_x_voisine = coord_x_actu;
        *coord_y_voisine = coord_y_actu - 1;

    } else if (direction_voulue == 1) 
    {     
        // Est
        *coord_x_voisine = coord_x_actu + 1;
        *coord_y_voisine = coord_y_actu;

    } else if (direction_voulue == 2) 
    {    
        // Sud       
        *coord_x_voisine = coord_x_actu;
        *coord_y_voisine = coord_y_actu + 1;

    } else if (direction_voulue == 3) 
    {    
        // Ouest
        *coord_x_voisine= coord_x_actu - 1;
        *coord_y_voisine= coord_y_actu;

    } else 
    {
        // Au cas ou, mauvaise détection
        *coord_x_voisine = coord_x_actu;
        *coord_y_voisine = coord_y_actu;
    }
}  



/**
 * Vérifie si un passage est physiquement possible entre (x,y) et (nx,ny)
 * en lisant directement dans les données brutes du labyrinthe.
 */
bool est_passage_possible(t_laby *laby, int x, int y, int nx, int ny, int dir) {
    if (nx < 0 || nx >= laby->sizeX || ny < 0 || ny >= laby->sizeY) {
        return false;
    }

    int m_actu[4], item_actu;
    int m_voisin[4], item_voisin;
    int memo_murs_actu[4] = {0,0,0,0}; // Correction ici
    int memo_murs_voisin[4] = {0,0,0,0};
    
    char *ptr = laby->labyData;
    int offset = 0;
    int compteur = 0;
    int cible_actuelle = y * laby->sizeX + x;
    int cible_voisine = ny * laby->sizeX + nx;
    
    while (sscanf(ptr, "%d %d %d %d %d%n", &m_actu[0], &m_actu[1], &m_actu[2], &m_actu[3], &item_actu, &offset) == 5) {
        if (compteur == cible_actuelle) {
            for(int i=0; i<4; i++) memo_murs_actu[i] = m_actu[i];
        }
        if (compteur == cible_voisine) {
            for(int i=0; i<4; i++) memo_murs_voisin[i] = m_actu[i];
        }
        ptr += offset;
        compteur++;
        if (compteur > laby->sizeX * laby->sizeY) break;
    }

    int dir_opposee = (dir + 2) % 4;

    // Si mur en sortie de la case actuelle
    if (memo_murs_actu[dir] == 1) return false;
    // Si mur en entrée de la case voisine
    if (memo_murs_voisin[dir_opposee] == 1) return false;

    return true;
}
int phaseExpansion(t_laby *laby, int coord_x_depart, int coord_y_depart, int coord_x_arrivee, int coord_y_arrivee)
{
    int TX = laby->sizeX;
    int TY = laby->sizeY;

    // Création d'un tableau de travail dynamique à la taille du laby
    int tab[TX][TY];

    // 1. Initialisation : tout à 0
    for (int j = 0; j < TY; j++) {
        for (int i = 0; i < TX; i++) {
            tab[i][j] = 0;
        }
    }

    int distance = 1;
    int nouvelles_cases = 1;
    tab[coord_x_depart][coord_y_depart] = distance;

    // 2. Boucle d'expansion (Algorithme de Lee)
    while (tab[coord_x_arrivee][coord_y_arrivee] == 0 && nouvelles_cases) {
        nouvelles_cases = 0;

        for (int y = 0; y < TY; y++) {
            for (int x = 0; x < TX; x++) {
                
                // Si c'est une case marquée à la distance actuelle
                if (tab[x][y] == distance) {
                    
                    // On regarde les 4 voisins
                    for (int d = 0; d < 4; d++) {
                        int nx, ny;
                        coord_case_voisine(x, y, d, &nx, &ny);

                        // Vérification des limites du plateau
                        if (nx >= 0 && nx < TX && ny >= 0 && ny < TY) {
                            
                            // Si la case n'est pas encore visitée
                            if (tab[nx][ny] == 0) {
                                
                                // /!\ C'est ici qu'on doit vérifier les MURS /!\
                                // Pour l'instant on vérifie juste que c'est possible
                                if (est_passage_possible(laby, x, y, nx, ny, d)) {
                                    tab[nx][ny] = distance + 1;
                                    nouvelles_cases = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        distance++;
        
        // Sécurité pour éviter les boucles infinies
        if (distance > TX * TY) break;
    }

    // Renvoie 1 (true) si une distance a été inscrite à l'arrivée
    return (tab[coord_x_arrivee][coord_y_arrivee] > 0);
}

int phaseRemontee(int TX, int TY, int tab[TX][TY], int x_arr, int y_arr, t_coord *chemin) {
    int x = x_arr;
    int y = y_arr;
    int d = tab[x][y];
    int index = 0;

    while (d > 1) {
        chemin[index].x = x;
        chemin[index].y = y;
        index++;

        // On cherche le voisin qui a la distance d-1
        for (int i = 0; i < 4; i++) {
            int nx, ny;
            coord_case_voisine(x, y, i, &nx, &ny);
            
            if (nx >= 0 && nx < TX && ny >= 0 && ny < TY) {
                if (tab[nx][ny] == d - 1) {
                    // ATTENTION : Vérifie aussi s'il n'y a pas de mur entre (x,y) et (nx,ny)
                    // car deux cases peuvent être distantes de d et d-1 sans être reliées !
                    if (est_passage_possible(&laby, x, y, nx, ny, i)) {
                        x = nx;
                        y = ny;
                        d--;
                        break;
                    }
                }
            }
        }
    }
    // Ajouter la case de départ (distance 1)
    chemin[index].x = x;
    chemin[index].y = y;
    return index + 1; // Retourne la taille du chemin
}